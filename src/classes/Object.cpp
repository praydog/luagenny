extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <type_traits>
#include <string_view>

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "Object.hpp"

namespace luagenny {
template<size_t N>
struct String {
    constexpr String(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    char value[N];
};

template<typename T, String Name>
struct TypeDescriptor {
    static bool is_a(genny::Object& o, std::string_view name) {
        return name == Name.value && o.is_a<T>();
    }

    static T* as(genny::Object& o, std::string_view name) {
        if (name == Name.value) {
            return dynamic_cast<T*>(&o);
        }
        return nullptr;
    }

    static T* find(genny::Object& o, std::string_view name, std::string_view target_name) {
        if (name == Name.value) {
            return o.find<T>(target_name);
        }
        return nullptr;
    }

    static T* find_in_owners(genny::Object& o, std::string_view name, std::string_view target_name, bool include_self) {
        if (name == Name.value) {
            return o.find_in_owners<T>(target_name, include_self);
        }
        return nullptr;
    }

    static bool has_any(genny::Object& o, std::string_view name) {
        return name == Name.value && o.has_any<T>();
    }

    static bool has_any_in_children(genny::Object& o, std::string_view name) {
        return name == Name.value && o.has_any_in_children<T>();
    }

    static T* owner(genny::Object& o, std::string_view name) {
        if (name == Name.value) {
            return o.owner<T>();
        }
        return nullptr;
    }
    
    static T* topmost_owner(genny::Object& o, std::string_view name) {
        if (name == Name.value) {
            return o.topmost_owner<T>();
        }
        return nullptr;
    }

    static std::optional<std::vector<T*>> get_all(genny::Object& o, std::string_view name) {
        if (name == Name.value) {
            return o.get_all<T>();
        }

        return std::nullopt;
    }
};

template <typename ...Args>
struct Bindings {

};

template <typename ...Args> // args are TypeDescriptors
void create_bindings(sol::table sdkgenny) {
    sdkgenny.new_usertype<genny::Object>("Object",
        "metadata", [](sol::this_state s, genny::Object& o) -> std::vector<std::string>& {
            return o.metadata();
        },
        MULTIFUNCTION(genny::Object, name, std::string),
        "is_a", [] (sol::this_state s, genny::Object& o, std::string name) -> bool {
            return (Args::is_a(o, name) || ... || false);
        },
        "as", [] (sol::this_state s, genny::Object& o, std::string name) -> sol::object {
            sol::object result = sol::make_object(s, sol::nil);

            (((
                (result = sol::make_object(s, Args::as(o, name))) && !result.is<sol::nil_t>()))
                || ...
                || (result = sol::make_object(s, sol::nil))
            );

            return result;
        },
        "find", [] (sol::this_state s, genny::Object& o, std::string name, std::string target_name) -> sol::object {
            sol::object result = sol::make_object(s, sol::nil);

            (((
                (result = sol::make_object(s, Args::find(o, name, target_name))) && !result.is<sol::nil_t>()))
                || ...
                || (result = sol::make_object(s, sol::nil))
            );

            return result;
        },
        "find_in_owners", [] (sol::this_state s, genny::Object& o, std::string name, std::string target_name, bool include_self) -> sol::object {
            sol::object result = sol::make_object(s, sol::nil);

            (((
                (result = sol::make_object(s, Args::find_in_owners(o, name, target_name, include_self))) && !result.is<sol::nil_t>()))
                || ...
                || (result = sol::make_object(s, sol::nil))
            );

            return result;
        },
        "has_any", [] (sol::this_state s, genny::Object& o, std::string name) -> bool {
            return (Args::has_any(o, name) || ... || false);
        },
        "has_any_in_children", [] (sol::this_state s, genny::Object& o, std::string name) -> bool {
            return (Args::has_any_in_children(o, name) || ... || false);
        },
        "owner", [] (sol::this_state s, genny::Object& o, std::string name) -> sol::object {
            sol::object result = sol::make_object(s, sol::nil);

            (((
                (result = sol::make_object(s, Args::owner(o, name))) && !result.is<sol::nil_t>()))
                || ...
                || (result = sol::make_object(s, sol::nil))
            );

            return result;
        },
        "topmost_owner", [] (sol::this_state s, genny::Object& o, std::string name) -> sol::object {
            sol::object result = sol::make_object(s, sol::nil);

            (((
                (result = sol::make_object(s, Args::topmost_owner(o, name))) && !result.is<sol::nil_t>()))
                || ...
                || (result = sol::make_object(s, sol::nil))
            );

            return result;
        },
        "get_all", [] (sol::this_state s, genny::Object& o, std::string name) -> sol::object {
            sol::object result = sol::make_object(s, sol::nil);

            (((
                (result = sol::make_object(s, Args::get_all(o, name))) && !result.is<sol::nil_t>()))
                || ...
                || (result = sol::make_object(s, sol::nil))
            );

            return result;
        }
    );
}

int open_object(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    create_bindings<
        TypeDescriptor<genny::Typename, "typename">,
        TypeDescriptor<genny::Type, "type">,
        TypeDescriptor<genny::GenericType, "generic_type">,
        TypeDescriptor<genny::Struct, "struct">,
        TypeDescriptor<genny::Class, "class">,
        TypeDescriptor<genny::Enum, "enum">,
        TypeDescriptor<genny::EnumClass, "enum_class">,
        TypeDescriptor<genny::Namespace, "namespace">,
        TypeDescriptor<genny::Reference, "reference">,
        TypeDescriptor<genny::Pointer, "pointer">,
        TypeDescriptor<genny::Variable, "variable">,
        TypeDescriptor<genny::Function, "function">,
        TypeDescriptor<genny::VirtualFunction, "virtual_function">,
        TypeDescriptor<genny::StaticFunction, "static_function">,
        TypeDescriptor<genny::Array, "array">,
        TypeDescriptor<genny::Parameter, "parameter">,
        TypeDescriptor<genny::Constant, "constant">>(sdkgenny);

    return 0;
}
}