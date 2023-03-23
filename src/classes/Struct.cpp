extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
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

    static T* find_in_parents(sdkgenny::Struct& s, std::string_view name, std::string_view target_name, bool include_self) {
        if (name == Name.value) {
            return s.find_in_parents<T>(target_name);
        }
        return nullptr;
    }

    static consteval std::string_view name() {
        return Name.value;
    }

    static constexpr auto find_in_parents_standalone = &sdkgenny::Struct::find_in_parents<T>;
};

template <typename ...Args> // args are TypeDescriptors
void create_bindings(sol::table sdkgenny) {
    auto object = sdkgenny.new_usertype<sdkgenny::Struct>("Struct",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>(),
        MULTIFUNCTION(sdkgenny::Struct, size, int),
        "parent", &sdkgenny::Struct::parent,
        "parents", &sdkgenny::Struct::parents,
        "variable", &sdkgenny::Struct::variable,
        "constant", &sdkgenny::Struct::constant,
        "struct", &sdkgenny::Struct::struct_,
        "class", &sdkgenny::Struct::class_,
        "enum", &sdkgenny::Struct::enum_,
        "enum_class", &sdkgenny::Struct::enum_class,
        "function", &sdkgenny::Struct::function,
        "virtual_function", &sdkgenny::Struct::virtual_function,
        "static_function", &sdkgenny::Struct::static_function,
        "bitfield", &sdkgenny::Struct::bitfield,
        "find_in_parents", [] (sol::this_state s, sdkgenny::Struct& st, const char* name, const char* target_name) -> sol::object {
            using ft = std::function<sol::object(sol::this_state s, sdkgenny::Struct&, std::string_view)>;
            static std::unordered_map<std::string, ft> find_in_parents_functions = []() {
                std::unordered_map<std::string, ft> find_in_parents_functions{};
                (find_in_parents_functions.emplace(Args::name().data(), [] (sol::this_state s, sdkgenny::Struct& st, std::string_view target_name) -> sol::object {
                    return sol::make_object(s, st.find_in_parents<typename Args::Type>(target_name));
                }), ...);

                return find_in_parents_functions;
            }();

            if (auto it = find_in_parents_functions.find(name); it != find_in_parents_functions.end()) {
                return it->second(s, st, target_name);
            }

            return sol::make_object(s, sol::lua_nil);
        }
    );

    (object.set(std::string("find_") + Args::name().data() + "_in_parents", Args::find_in_parents_standalone), ...);
}

int open_struct(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    create_bindings<
        STypeDescriptor<sdkgenny::Typename, "typename">,
        STypeDescriptor<sdkgenny::Type, "type">,
        STypeDescriptor<sdkgenny::GenericType, "generic_type">,
        STypeDescriptor<sdkgenny::Struct, "struct">,
        STypeDescriptor<sdkgenny::Class, "class">,
        STypeDescriptor<sdkgenny::Enum, "enum">,
        STypeDescriptor<sdkgenny::EnumClass, "enum_class">,
        STypeDescriptor<sdkgenny::Namespace, "namespace">,
        STypeDescriptor<sdkgenny::Reference, "reference">,
        STypeDescriptor<sdkgenny::Pointer, "pointer">,
        STypeDescriptor<sdkgenny::Variable, "variable">,
        STypeDescriptor<sdkgenny::Function, "function">,
        STypeDescriptor<sdkgenny::VirtualFunction, "virtual_function">,
        STypeDescriptor<sdkgenny::StaticFunction, "static_function">,
        STypeDescriptor<sdkgenny::Array, "array">,
        STypeDescriptor<sdkgenny::Parameter, "parameter">,
        STypeDescriptor<sdkgenny::Constant, "constant">>(sdkgenny);

    return 0;
}
}