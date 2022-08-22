#include <vector>
#include <fstream>
#include <bit>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <sol/sol.hpp>

#if _MSC_VER == 1932 || _MSC_VER == 1933
#if _MSC_VER == 1932
#define _MSC_OLD_VER 1932
#endif

#if _MSC_VER == 1933
#define _MSC_OLD_VER 1933
#endif

#undef _MSC_VER
#define _MSC_VER 1919
#include <tao/pegtl/demangle.hpp>
#undef _MSC_VER
#define _MSC_VER _MSC_OLD_VER
#endif

#include <Genny.hpp>
#include <GennyParser.hpp>

#include "classes/ClassMacros.hpp"
#include "classes/Sdk.hpp"
#include "classes/Object.hpp"
#include "classes/Type.hpp"
#include "classes/Typename.hpp"
#include "classes/Struct.hpp"
#include "classes/Enum.hpp"
#include "classes/Namespace.hpp"
#include "classes/Variable.hpp"
#include "classes/Reference.hpp"
#include "classes/Function.hpp"
#include "classes/VirtualFunction.hpp"
#include "classes/Parameter.hpp"
#include "classes/Constant.hpp"
#include "classes/Array.hpp"

#include "LuaGenny.hpp"

namespace luagenny {
static constexpr auto hash(std::string_view data) {
    size_t result = 0xcbf29ce484222325;

    for (char c : data) {
        result ^= c;
        result *= (size_t)1099511628211;
    }

    return result;
}

constexpr auto operator "" _fnv(const char* s, size_t) {
    return hash(s);
}

namespace api {
sol::object standalone_parse(sol::this_state s, uintptr_t address, genny::Type* t, genny::Variable* v = nullptr);

sol::object reader(sol::this_state s, uintptr_t address, size_t size) {
    switch(size) {
    case 8:
        return sol::make_object(s, *(uint64_t*)address);
    case 4:
        return sol::make_object(s, *(uint32_t*)address);
    case 2:
        return sol::make_object(s, *(uint16_t*)address);
    case 1:
        return sol::make_object(s, *(uint8_t*)address);
    }
    
    return sol::make_object(s, sol::nil);
}

sol::object string_reader(sol::this_state s, uintptr_t address) {
    return sol::make_object(s, (const char*)address);
}

std::optional<std::tuple<genny::Variable*, size_t>> get_variable(genny::Struct* s, const std::string& name, size_t additional_offset = 0) {
    auto v = s->find<genny::Variable>(name);

    if (v != nullptr) {
        return std::make_tuple(v, additional_offset);
    }

    const auto& parents = s->parents();

    for (auto parent : parents) {
        auto v = get_variable(parent, name, additional_offset);

        if (v) {
            return v;
        }

        additional_offset += parent->size();
    }

    return std::nullopt;
}

template<typename T>
class Overlay {
public:
    Overlay(uintptr_t address, T* t) 
        : m_address(address),
        m_type{t}
    {
    }

    virtual sol::object index(sol::this_state s, sol::object key) = 0;

    sol::object type_(sol::this_state s) const {
        return sol::make_object(s, m_type);
    }

    sol::object address(sol::this_state s) const {
        return sol::make_object(s, m_address);
    }

protected:
    uintptr_t m_address{};
    T* m_type{};
};

class StructOverlay : public Overlay<genny::Struct> {
public:
    StructOverlay(uintptr_t address, genny::Struct* s) 
        : Overlay(address, s)
    {
    }

    sol::object index(sol::this_state s, sol::object key) override {
        if (key.is<int>()) {
            // Pretend it's an inlined array
            return sol::make_object(s, StructOverlay{m_address + (key.as<int>() * m_type->size()), m_type});
        }

        if (!key.is<std::string>()) {
            return sol::make_object(s, sol::nil);
        }

        const auto name = key.as<std::string>();
        const auto pv = get_variable(m_type, name);

        if (!pv) {
            return sol::make_object(s, sol::nil);
        }

        const auto& [v, additional_offset] = *pv;

        if (v == nullptr || v->type() == nullptr) {
            return sol::make_object(s, sol::nil);
        }

        return sol::make_object(s, parse_and_read(s, v, additional_offset));
    }

protected:
    sol::object parse_and_read(sol::this_state s, genny::Variable* v, size_t additional_offset = 0) {
        const auto offset = v->offset();

        if (offset > m_type->size()) {
            throw std::runtime_error("offset out of bounds");
            return sol::make_object(s, sol::nil);
        }

        const auto address = m_address + offset + additional_offset;

        return sol::make_object(s, standalone_parse(s, address, v->type(), v));
    }
};

class PointerOverlay : public Overlay<genny::Pointer> {
public:
    PointerOverlay(uintptr_t address, genny::Pointer* p, genny::Variable* from = nullptr) 
        : Overlay(address, p),
        m_from{from}
    {
    }

    sol::object index(sol::this_state s, sol::object key) override {
        const auto pointed_to = ptr_internal(s);

        if (pointed_to == 0) {
            return sol::make_object(s, sol::nil);
        }

        if (m_type->to()->is_a<genny::Struct>()) {
            return StructOverlay{pointed_to, dynamic_cast<genny::Struct*>(m_type->to())}.index(s, key);
        }

        if (key.is<int>()) {
            const auto adjusted_to = pointed_to + (m_type->to()->size() * key.as<int>());
            return sol::make_object(s, standalone_parse(s, adjusted_to, m_type->to(), m_from));
        }

        return sol::make_object(s, sol::nil);
    }
    
    // Address pointed to, not address of pointer
    sol::object d(sol::this_state s) const {
        const auto pointed_to = ptr_internal(s);

        if (pointed_to == 0) {
            return sol::make_object(s, sol::nil);
        }

        return sol::make_object(s, standalone_parse(s, pointed_to, m_type->to(), m_from));
    }

    uintptr_t ptr(sol::this_state s) const {
        return ptr_internal(s);
    }

protected:
    uintptr_t ptr_internal(sol::this_state s) const {
        sol::function lua_reader = sol::state_view{s}["sdkgenny_reader"];
        uintptr_t deref_address = 0;

        if constexpr (sizeof(void*) == 8) {
            deref_address = (uintptr_t)lua_reader(s, m_address, sizeof(void*)).get<uint64_t>();
        } else {
            deref_address = (uintptr_t)lua_reader(s, m_address, sizeof(void*)).get<uint32_t>();
        }

        return deref_address;
    }

    genny::Variable* m_from{};
};

sol::object standalone_parse(sol::this_state s, uintptr_t address, genny::Type* t, genny::Variable* v) {
    if (t == nullptr) {
        return sol::make_object(s, address);
    }

    if (t->is_a<genny::Struct>()) {
        return sol::make_object(s, StructOverlay{address, dynamic_cast<genny::Struct*>(t)});
    }

    const auto is_pointer = t->is_a<genny::Pointer>();
    genny::Type* pointer_t = nullptr;

    auto metadata = t->metadata();

    if (metadata.empty() && v != nullptr) {
        metadata = v->metadata();
    }

    sol::function lua_reader = sol::state_view{s}["sdkgenny_reader"];
    sol::function lua_string_reader = sol::state_view{s}["sdkgenny_string_reader"];

    if (is_pointer) {
        const auto p = dynamic_cast<genny::Pointer*>(t);
        const auto to = p->to();

        if (to->is_a<genny::Struct>() || to->is_a<genny::Pointer>() || metadata.empty()) {
            return sol::make_object(s, PointerOverlay{address, p, v});
        }

        address = lua_reader(s, address, sizeof(void*)).get<uintptr_t>();

        if (address == 0) {
            return sol::make_object(s, sol::nil);
        }

        if (metadata.empty()) {
            metadata = to->metadata();
        }
    }

    if (metadata.empty()){
        throw std::runtime_error("No metadata for type");
        return sol::make_object(s, sol::nil);
    }

    for (auto&& md : metadata) {
        switch (hash(md)) {
            case "bool"_fnv:
                return sol::make_object(s, (bool)lua_reader(s, address, 1).get<uint8_t>());
            case "u8"_fnv:
                return sol::make_object(s, (uint8_t)lua_reader(s, address, 1).get<uint8_t>());
            case "u16"_fnv:
                return sol::make_object(s, (uint16_t)lua_reader(s, address, 2).get<uint16_t>());
            case "u32"_fnv:
                return sol::make_object(s, (uint32_t)lua_reader(s, address, 4).get<uint32_t>());
            case "u64"_fnv:
                return sol::make_object(s, (uint64_t)lua_reader(s, address, 8).get<uint64_t>());
            case "i8"_fnv:
                return sol::make_object(s, (int8_t)lua_reader(s, address, 1).get<uint8_t>());
            case "i16"_fnv:
                return sol::make_object(s, (int16_t)lua_reader(s, address, 2).get<uint16_t>());
            case "i32"_fnv:
                return sol::make_object(s, (int32_t)lua_reader(s, address, 4).get<uint32_t>());
            case "i64"_fnv:
                return sol::make_object(s, (int64_t)lua_reader(s, address, 8).get<uint64_t>());
            case "f32"_fnv:
                return sol::make_object(s, std::bit_cast<float>(lua_reader(s, address, 4).get<uint32_t>()));
            case "f64"_fnv:
                return sol::make_object(s, std::bit_cast<double>(lua_reader(s, address, 8).get<uint64_t>()));
            case "utf8*"_fnv:
                return sol::make_object(s, lua_string_reader(s, address, 1).get<const char*>());
            default:
                continue;
        }
    }

    return sol::make_object(s, sol::nil);
}

sol::object parse(sol::this_state s, std::string data) {
    auto sdk = std::make_unique<genny::Sdk>();

    genny::parser::State state{};
    state.parents.push_back(sdk->global_ns());

    tao::pegtl::memory_input in{data, "text"};
    tao::pegtl::parse<genny::parser::Grammar, genny::parser::Action>(in, state);

    return sol::make_object(s, std::move(sdk));
}

sol::object parse_file(sol::this_state s, std::string filename) {
    std::ifstream file{filename};
    std::string data{(std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()};
    return parse(s, data);
}
}

int open(lua_State* l) {
    sol::state_view lua{l};

    auto sdkgenny = lua.create_table();

    lua["sdkgenny_reader"] = &api::reader;
    lua["sdkgenny_string_reader"] = &api::string_reader;
    sdkgenny["parse"] = &api::parse;
    sdkgenny["parse_file"] = &api::parse_file;

    sdkgenny.new_usertype<api::StructOverlay>("StructOverlay",
        sol::meta_function::construct, sol::constructors<api::StructOverlay(uintptr_t, genny::Struct*)>(),
        sol::call_constructor, sol::constructors<api::StructOverlay(uintptr_t, genny::Struct*)>(),
        "type", &api::StructOverlay::type_,
        "address", &api::StructOverlay::address,
        sol::meta_function::index, &api::StructOverlay::index
    );

    sdkgenny.new_usertype<api::PointerOverlay>("PointerOverlay",
        sol::meta_function::construct, sol::constructors<api::PointerOverlay(uintptr_t, genny::Pointer*)>(),
        sol::call_constructor, sol::constructors<api::PointerOverlay(uintptr_t, genny::Pointer*)>(),
        "type", &api::PointerOverlay::type_,
        "address", &api::PointerOverlay::address, // address of the pointer, not what it points to. Same as &ptr.
        "d", &api::PointerOverlay::d, // Resolve the pointed to address into a pointer or value. Same as ptr[0].
        "deref", &api::PointerOverlay::d, // Resolve the pointed to address into a pointer or value. Same as ptr[0].
        "dereference", &api::PointerOverlay::d, // Resolve the pointed to address into a pointer or value. Same as ptr[0].
        "p", &api::PointerOverlay::ptr, // address pointed to, like auto var = (uintptr_t)ptr.
        "ptr", &api::PointerOverlay::ptr, // address pointed to, like auto var = (uintptr_t)ptr.
        sol::meta_function::index, &api::PointerOverlay::index // Access like ptr->field in C++ or an array, like ptr[i]
    );

    sdkgenny.push();
    luagenny::open_sdk(l);

    sdkgenny.push();
    luagenny::open_object(l);

    sdkgenny.push();
    luagenny::open_typename(l);

    sdkgenny.push();
    luagenny::open_type(l);

    sdkgenny.new_usertype<genny::GenericType>("GenericType",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>()
    );

    sdkgenny.push();
    luagenny::open_struct(l);

    sdkgenny.new_usertype<genny::Class>("Class",
        sol::base_classes, sol::bases<genny::Struct, GENNY_TYPE_BASES>()
    );

    sdkgenny.push();
    luagenny::open_enum(l);

    sdkgenny.new_usertype<genny::EnumClass>("EnumClass",
        sol::base_classes, sol::bases<genny::Enum, GENNY_TYPE_BASES>()
    );

    sdkgenny.push();
    luagenny::open_namespace(l);

    sdkgenny.push();
    luagenny::open_reference(l);

    sdkgenny.new_usertype<genny::Pointer>("Pointer",
        sol::base_classes, sol::bases<GENNY_REFERENCE_BASES>()
    );

    sdkgenny.push();
    luagenny::open_variable(l);

    sdkgenny.push();
    luagenny::open_function(l);

    sdkgenny.push();
    luagenny::open_virtual_function(l);

    sdkgenny.new_usertype<genny::StaticFunction>("StaticFunction",
        sol::base_classes, sol::bases<genny::Function, genny::Object>()
    );

    sdkgenny.push();
    luagenny::open_array(l);

    sdkgenny.push();
    luagenny::open_parameter(l);

    sdkgenny.push();
    luagenny::open_constant(l);

    sdkgenny.push(l);
    return 1;
} // namespace sdkgennylua
}