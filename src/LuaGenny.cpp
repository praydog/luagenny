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

#include <sdkgenny.hpp>
#include <sdkgenny_parser.hpp>

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

#include "LuaGenny.h"
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
sol::object standalone_parse(sol::this_state s, uintptr_t address, sdkgenny::Type* t, sdkgenny::Variable* v = nullptr);

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
    
    return sol::make_object(s, sol::lua_nil);
}

sol::object string_reader(sol::this_state s, uintptr_t address) {
    return sol::make_object(s, (const char*)address);
}

void writer(sol::this_state s, uintptr_t address, size_t size, sol::object value) {
    switch(size) {
    case 8:
        if (!value.is<sol::lua_nil_t>()) {
            value.push();

            if (lua_isinteger(s, -1)) {
                *(uint64_t*)address = (uint64_t)lua_tointeger(s, -1);
            } else if (lua_isnumber(s, -1)) {
                *(double*)address = lua_tonumber(s, -1);
            }

            value.pop();
        } else {
            *(uint64_t*)address = 0;
        }

        break;
    case 4:
        if (!value.is<sol::lua_nil_t>()) {
            value.push();

            if (lua_isinteger(s, -1)) {
                *(uint32_t*)address = (uint32_t)lua_tointeger(s, -1);
            } else if (lua_isnumber(s, -1)) {
                *(float*)address = (float)lua_tonumber(s, -1);
            }

            value.pop();
        } else {
            *(uint32_t*)address = 0;
        }

        break;
    case 2:
        *(uint16_t*)address = value.as<uint16_t>();
        break;
    case 1:
        value.push();

        if (lua_isboolean(s, -1)) {
            *(bool*)address = lua_toboolean(s, -1);
        } else if (lua_isinteger(s, -1)) {
            *(uint8_t*)address = (uint8_t)lua_tointeger(s, -1);
        }

        value.pop();

        break;
    }
}

std::optional<std::tuple<sdkgenny::Variable*, size_t>> get_variable(sdkgenny::Struct* s, const std::string& name, size_t additional_offset = 0) {
    auto v = s->find<sdkgenny::Variable>(name);

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
    virtual void new_index(sol::this_state s, sol::object key, sol::object value) {};

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

class StructOverlay : public Overlay<sdkgenny::Struct> {
public:
    StructOverlay(uintptr_t address, sdkgenny::Struct* s) 
        : Overlay(address, s)
    {
    }

    sol::object index(sol::this_state s, sol::object key) override {
        if (key.is<int>()) {
            // Pretend it's an inlined array
            return sol::make_object(s, StructOverlay{m_address + (key.as<int>() * m_type->size()), m_type});
        }

        if (!key.is<std::string>()) {
            return sol::make_object(s, sol::lua_nil);
        }

        const auto name = key.as<std::string>();
        const auto pv = get_variable(m_type, name);

        if (!pv) {
            return sol::make_object(s, sol::lua_nil);
        }

        const auto& [v, additional_offset] = *pv;

        if (v == nullptr || v->type() == nullptr) {
            return sol::make_object(s, sol::lua_nil);
        }

        return sol::make_object(s, parse_and_read(s, v, additional_offset));
    }

    void new_index(sol::this_state s, sol::object key, sol::object value) override {
        if (!key.is<std::string>()) {
            return;
        }

        const auto name = key.as<std::string>();
        const auto pv = get_variable(m_type, name);

        if (!pv) {
            return;
        }

        const auto& [v, additional_offset] = *pv;

        if (v == nullptr || v->type() == nullptr) {
            return;
        }

        // TODO: maybe figure out the type pointed to
        // and adjust how we write the value rather than writing
        // the value verbatim to the address (e.g. struct.some_float = 1 will write an integer instead of a float)
        this->write(s, v, additional_offset, value);
    }

protected:
    uintptr_t get_final_address(sdkgenny::Variable* v, size_t additional_offset = 0) const {
         const auto offset = v->offset();

        if (offset > m_type->size()) {
            throw std::runtime_error("offset out of bounds");
            return 0;
        }

        return m_address + offset + additional_offset;
    }

    sol::object parse_and_read(sol::this_state s, sdkgenny::Variable* v, size_t additional_offset = 0) {
        const auto address = get_final_address(v, additional_offset);

        return sol::make_object(s, standalone_parse(s, address, v->type(), v));
    }

    void write(sol::this_state s, sdkgenny::Variable* v, size_t additional_offset, sol::object value) {
        const auto address = get_final_address(v, additional_offset);

        sol::function lua_writer = sol::state_view{s}["sdkgenny_writer"];
        lua_writer(s, address, v->type()->size(), value);
    }
};

class PointerOverlay : public Overlay<sdkgenny::Pointer> {
public:
    PointerOverlay(uintptr_t address, sdkgenny::Pointer* p, sdkgenny::Variable* from = nullptr) 
        : Overlay(address, p),
        m_from{from}
    {
    }

    sol::object index(sol::this_state s, sol::object key) override {
        const auto pointed_to = ptr_internal(s);

        if (pointed_to == 0) {
            return sol::make_object(s, sol::lua_nil);
        }

        if (m_type->to()->is_a<sdkgenny::Struct>()) {
            return StructOverlay{pointed_to, dynamic_cast<sdkgenny::Struct*>(m_type->to())}.index(s, key);
        }

        if (key.is<int>()) {
            const auto adjusted_to = pointed_to + (m_type->to()->size() * key.as<int>());
            return sol::make_object(s, standalone_parse(s, adjusted_to, m_type->to(), m_from));
        }

        return sol::make_object(s, sol::lua_nil);
    }

    void new_index(sol::this_state s, sol::object key, sol::object value) override {
        const auto pointed_to = ptr_internal(s);

        if (pointed_to == 0) {
            return;
        }

        if (m_type->to()->is_a<sdkgenny::Struct>()) {
            StructOverlay{pointed_to, dynamic_cast<sdkgenny::Struct*>(m_type->to())}.new_index(s, key, value);
            return;
        }

        // TODO: maybe figure out the type pointed to
        // and adjust how we write the value rather than writing
        // the value verbatim to the address (e.g. struct.some_float = 1 will write an integer instead of a float)
        if (key.is<int>()) {
            const auto adjusted_to = pointed_to + (m_type->to()->size() * key.as<int>());
            sol::function lua_writer = sol::state_view{s}["sdkgenny_writer"];
            lua_writer(s, adjusted_to, m_type->to()->size(), value);
        }
    }
    
    // Address pointed to, not address of pointer
    sol::object d(sol::this_state s) const {
        const auto pointed_to = ptr_internal(s);

        if (pointed_to == 0) {
            return sol::make_object(s, sol::lua_nil);
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

    sdkgenny::Variable* m_from{};
};

sol::object standalone_parse(sol::this_state s, uintptr_t address, sdkgenny::Type* t, sdkgenny::Variable* v) {
    if (t == nullptr) {
        return sol::make_object(s, address);
    }

    if (t->is_a<sdkgenny::Struct>()) {
        return sol::make_object(s, StructOverlay{address, dynamic_cast<sdkgenny::Struct*>(t)});
    }

    const auto is_pointer = t->is_a<sdkgenny::Pointer>();
    sdkgenny::Type* pointer_t = nullptr;

    auto metadata = t->metadata();

    if (metadata.empty() && v != nullptr) {
        metadata = v->metadata();
    }

    sol::function lua_reader = sol::state_view{s}["sdkgenny_reader"];
    sol::function lua_string_reader = sol::state_view{s}["sdkgenny_string_reader"];

    if (is_pointer) {
        const auto p = dynamic_cast<sdkgenny::Pointer*>(t);
        const auto to = p->to();

        if (to->is_a<sdkgenny::Struct>() || to->is_a<sdkgenny::Pointer>() || metadata.empty()) {
            return sol::make_object(s, PointerOverlay{address, p, v});
        }

        address = lua_reader(s, address, sizeof(void*)).get<uintptr_t>();

        if (address == 0) {
            return sol::make_object(s, sol::lua_nil);
        }

        if (metadata.empty()) {
            metadata = to->metadata();
        }
    }

    if (metadata.empty()){
        throw std::runtime_error("No metadata for type");
        return sol::make_object(s, sol::lua_nil);
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
            case "f32"_fnv: {
#if defined(__cpp_lib_bit_cast)
                return sol::make_object(s, std::bit_cast<float>(lua_reader(s, address, 4).get<uint32_t>()));
#else
                auto value = lua_reader(s, address, 4).get<uint32_t>();
                return sol::make_object(s, *(float*)&value);
#endif
            }
            case "f64"_fnv: {
#if defined(__cpp_lib_bit_cast)
                return sol::make_object(s, std::bit_cast<double>(lua_reader(s, address, 8).get<uint64_t>()));
#else
                auto value = lua_reader(s, address, 8).get<uint64_t>();
                return sol::make_object(s, *(double*)&value);
#endif
            }
            case "utf8*"_fnv:
                return sol::make_object(s, lua_string_reader(s, address, 1).get<const char*>());
            default:
                continue;
        }
    }

    return sol::make_object(s, sol::lua_nil);
}

sol::object parse(sol::this_state s, std::string data) {
    auto sdk = std::make_unique<sdkgenny::Sdk>();

    sdkgenny::parser::State state{};
    state.parents.push_back(sdk->global_ns());

    tao::pegtl::memory_input in{data, "text"};
    tao::pegtl::parse<sdkgenny::parser::Grammar, sdkgenny::parser::Action>(in, state);

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
    lua["sdkgenny_writer"] = &api::writer;
    sdkgenny["parse"] = &api::parse;
    sdkgenny["parse_file"] = &api::parse_file;

    sdkgenny.new_usertype<api::StructOverlay>("StructOverlay",
        sol::meta_function::construct, sol::constructors<api::StructOverlay(uintptr_t, sdkgenny::Struct*)>(),
        sol::call_constructor, sol::constructors<api::StructOverlay(uintptr_t, sdkgenny::Struct*)>(),
        "type", &api::StructOverlay::type_,
        "address", &api::StructOverlay::address,
        sol::meta_function::index, &api::StructOverlay::index,
        sol::meta_function::new_index, &api::StructOverlay::new_index
    );

    sdkgenny.new_usertype<api::PointerOverlay>("PointerOverlay",
        sol::meta_function::construct, sol::constructors<api::PointerOverlay(uintptr_t, sdkgenny::Pointer*)>(),
        sol::call_constructor, sol::constructors<api::PointerOverlay(uintptr_t, sdkgenny::Pointer*)>(),
        "type", &api::PointerOverlay::type_,
        "address", &api::PointerOverlay::address, // address of the pointer, not what it points to. Same as &ptr.
        "d", &api::PointerOverlay::d, // Resolve the pointed to address into a pointer or value. Same as ptr[0].
        "deref", &api::PointerOverlay::d, // Resolve the pointed to address into a pointer or value. Same as ptr[0].
        "dereference", &api::PointerOverlay::d, // Resolve the pointed to address into a pointer or value. Same as ptr[0].
        "p", &api::PointerOverlay::ptr, // address pointed to, like auto var = (uintptr_t)ptr.
        "ptr", &api::PointerOverlay::ptr, // address pointed to, like auto var = (uintptr_t)ptr.
        sol::meta_function::index, &api::PointerOverlay::index, // Access like ptr->field in C++ or an array, like ptr[i]
        sol::meta_function::new_index, &api::PointerOverlay::new_index // Access like ptr->field = value in C++ or an array, like ptr[i] = value
    );

    sdkgenny.push();
    luagenny::open_sdk(l);

    sdkgenny.push();
    luagenny::open_object(l);

    sdkgenny.push();
    luagenny::open_typename(l);

    sdkgenny.push();
    luagenny::open_type(l);

    sdkgenny.new_usertype<sdkgenny::GenericType>("GenericType",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>()
    );

    sdkgenny.push();
    luagenny::open_struct(l);

    sdkgenny.new_usertype<sdkgenny::Class>("Class",
        sol::base_classes, sol::bases<sdkgenny::Struct, GENNY_TYPE_BASES>()
    );

    sdkgenny.push();
    luagenny::open_enum(l);

    sdkgenny.new_usertype<sdkgenny::EnumClass>("EnumClass",
        sol::base_classes, sol::bases<sdkgenny::Enum, GENNY_TYPE_BASES>()
    );

    sdkgenny.push();
    luagenny::open_namespace(l);

    sdkgenny.push();
    luagenny::open_reference(l);

    sdkgenny.new_usertype<sdkgenny::Pointer>("Pointer",
        sol::base_classes, sol::bases<GENNY_REFERENCE_BASES>()
    );

    sdkgenny.push();
    luagenny::open_variable(l);

    sdkgenny.push();
    luagenny::open_function(l);

    sdkgenny.push();
    luagenny::open_virtual_function(l);

    sdkgenny.new_usertype<sdkgenny::StaticFunction>("StaticFunction",
        sol::base_classes, sol::bases<sdkgenny::Function, sdkgenny::Object>()
    );

    sdkgenny.push();
    luagenny::open_array(l);

    sdkgenny.push();
    luagenny::open_parameter(l);

    sdkgenny.push();
    luagenny::open_constant(l);

    sdkgenny.push(l);
    return 1;
} // namespace luagenny
}

int luaopen_luagenny(lua_State* l) {
    return luagenny::open(l);
}