extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "Struct.hpp"

namespace luagenny {
template<size_t N>
struct String {
    constexpr String(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    char value[N];
};

template<typename T, String Name>
struct STypeDescriptor {
    using Type = T;

    static T* find_in_parents(genny::Struct& s, std::string_view name, std::string_view target_name, bool include_self) {
        if (name == Name.value) {
            return s.find_in_parents<T>(target_name);
        }
        return nullptr;
    }

    static consteval std::string_view name() {
        return Name.value;
    }

    static constexpr auto find_in_parents_standalone = &genny::Struct::find_in_parents<T>;
};

template <typename ...Args> // args are TypeDescriptors
void create_bindings(sol::table sdkgenny) {
    auto object = sdkgenny.new_usertype<genny::Struct>("Struct",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>(),
        MULTIFUNCTION(genny::Struct, size, int),
        "parent", &genny::Struct::parent,
        "parents", &genny::Struct::parents,
        "variable", &genny::Struct::variable,
        "constant", &genny::Struct::constant,
        "struct", &genny::Struct::struct_,
        "class", &genny::Struct::class_,
        "enum", &genny::Struct::enum_,
        "enum_class", &genny::Struct::enum_class,
        "function", &genny::Struct::function,
        "virtual_function", &genny::Struct::virtual_function,
        "static_function", &genny::Struct::static_function,
        "bitfield", &genny::Struct::bitfield,
        "find_in_parents", [] (sol::this_state s, genny::Struct& st, const char* name, const char* target_name) -> sol::object {
            using ft = std::function<sol::object(sol::this_state s, genny::Struct&, std::string_view)>;
            static std::unordered_map<std::string, ft> find_in_parents_functions = []() {
                std::unordered_map<std::string, ft> find_in_parents_functions{};
                (find_in_parents_functions.emplace(Args::name().data(), [] (sol::this_state s, genny::Struct& st, std::string_view target_name) -> sol::object {
                    return sol::make_object(s, st.find_in_parents<Args::Type>(target_name));
                }), ...);

                return find_in_parents_functions;
            }();

            if (auto it = find_in_parents_functions.find(name); it != find_in_parents_functions.end()) {
                return it->second(s, st, target_name);
            }

            return sol::make_object(s, sol::nil);
        }
    );

    (object.set(std::string("find_") + Args::name().data() + "_in_parents", Args::find_in_parents_standalone), ...);
}

int open_struct(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    create_bindings<
        STypeDescriptor<genny::Typename, "typename">,
        STypeDescriptor<genny::Type, "type">,
        STypeDescriptor<genny::GenericType, "generic_type">,
        STypeDescriptor<genny::Struct, "struct">,
        STypeDescriptor<genny::Class, "class">,
        STypeDescriptor<genny::Enum, "enum">,
        STypeDescriptor<genny::EnumClass, "enum_class">,
        STypeDescriptor<genny::Namespace, "namespace">,
        STypeDescriptor<genny::Reference, "reference">,
        STypeDescriptor<genny::Pointer, "pointer">,
        STypeDescriptor<genny::Variable, "variable">,
        STypeDescriptor<genny::Function, "function">,
        STypeDescriptor<genny::VirtualFunction, "virtual_function">,
        STypeDescriptor<genny::StaticFunction, "static_function">,
        STypeDescriptor<genny::Array, "array">,
        STypeDescriptor<genny::Parameter, "parameter">,
        STypeDescriptor<genny::Constant, "constant">>(sdkgenny);

    return 0;
}
}