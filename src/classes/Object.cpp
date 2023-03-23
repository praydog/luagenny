extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <type_traits>
#include <string_view>

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
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
    using Type = T;

    static bool is_a(sdkgenny::Object& o, std::string_view name) {
        return name == Name.value && o.is_a<T>();
    }

    static T* as(sdkgenny::Object& o, std::string_view name) {
        if (name == Name.value) {
            return dynamic_cast<T*>(&o);
        }
        return nullptr;
    }

    static T* as_standalone(sdkgenny::Object& o) {
        return dynamic_cast<T*>(&o);
    }


    static T* find(sdkgenny::Object& o, std::string_view name, std::string_view target_name) {
        if (name == Name.value) {
            return o.find<T>(target_name);
        }
        return nullptr;
    }

    static T* find_in_owners(sdkgenny::Object& o, std::string_view name, std::string_view target_name, bool include_self) {
        if (name == Name.value) {
            return o.find_in_owners<T>(target_name, include_self);
        }
        return nullptr;
    }

    static bool has_any(sdkgenny::Object& o, std::string_view name) {
        return name == Name.value && o.has_any<T>();
    }

    static bool has_any_in_children(sdkgenny::Object& o, std::string_view name) {
        return name == Name.value && o.has_any_in_children<T>();
    }

    static T* owner(sdkgenny::Object& o, std::string_view name) {
        if (name == Name.value) {
            return o.owner<T>();
        }
        return nullptr;
    }
    
    static T* topmost_owner(sdkgenny::Object& o, std::string_view name) {
        if (name == Name.value) {
            return o.topmost_owner<T>();
        }
        return nullptr;
    }

    static std::optional<std::vector<T*>> get_all(sdkgenny::Object& o, std::string_view name) {
        if (name == Name.value) {
            return o.get_all<T>();
        }

        return std::nullopt;
    }

    static consteval std::string_view name() {
        return Name.value;
    }

    static constexpr auto is_a_standalone = &sdkgenny::Object::is_a<T>;
    static constexpr auto find_standalone = &sdkgenny::Object::find<T>;
    static constexpr auto find_in_owners_standalone = &sdkgenny::Object::find_in_owners<T>;
    static constexpr auto has_any_standalone = &sdkgenny::Object::has_any<T>;
    static constexpr auto has_any_in_children_standalone = &sdkgenny::Object::has_any_in_children<T>;
    static constexpr auto owner_standalone = (T* (sdkgenny::Object::*)())&sdkgenny::Object::owner<T>;
    static constexpr auto topmost_owner_standalone = (T* (sdkgenny::Object::*)())&sdkgenny::Object::topmost_owner<T>;
    static constexpr auto get_all_standalone = &sdkgenny::Object::get_all<T>;
};

template <typename ...Args>
struct Bindings {

};

template <typename ...Args> // args are TypeDescriptors
void create_bindings(sol::table sdkgenny) {
    auto object = sdkgenny.new_usertype<sdkgenny::Object>("Object",
        "metadata", [](sol::this_state s, sdkgenny::Object& o) -> std::vector<std::string>& {
            return o.metadata();
        },
        MULTIFUNCTION(sdkgenny::Object, name, std::string),
        "is_a", [] (sol::this_state s, sdkgenny::Object& o, const char* name) -> bool {
            return (Args::is_a(o, name) || ... || false);
        },
        "as", [] (sol::this_state s, sdkgenny::Object& o, const char* name) -> sol::object {
            /*sol::object result = sol::make_object(s, sol::lua_nil);

            (((
                (result = sol::make_object(s, Args::as(o, name))) && !result.is<sol::lua_nil_t>()))
                || ...
                || (result = sol::make_object(s, sol::lua_nil))
            );

            return result;*/

            using ft = std::function<sol::object(sol::this_state s, sdkgenny::Object&)>;
            static std::unordered_map<std::string, ft> find_functions = []() {
                std::unordered_map<std::string, ft> find_functions{};
                (find_functions.emplace(Args::name().data(), [] (sol::this_state s, sdkgenny::Object& o) -> sol::object {
                    return sol::make_object(s, dynamic_cast<typename Args::Type*>(&o));
                }), ...);

                return find_functions;
            }();

            if (auto it = find_functions.find(name); it != find_functions.end()) {
                return it->second(s, o);
            }

            return sol::make_object(s, sol::lua_nil);
        },
        "find", [] (sol::this_state s, sdkgenny::Object& o, const char* name, const char* target_name) -> sol::object {
            /*sol::object result = sol::make_object(s, sol::lua_nil);

            (((
                (result = sol::make_object(s, Args::find(o, name, target_name))) && !result.is<sol::lua_nil_t>()))
                || ...
                || (result = sol::make_object(s, sol::lua_nil))
            );

            return result;*/

            using ft = std::function<sol::object(sol::this_state s, sdkgenny::Object&, std::string_view)>;
            static std::unordered_map<std::string, ft> find_functions = []() {
                std::unordered_map<std::string, ft> find_functions{};
                (find_functions.emplace(Args::name().data(), [] (sol::this_state s, sdkgenny::Object& o, std::string_view target_name) -> sol::object {
                    return sol::make_object(s, o.find<typename Args::Type>(target_name));
                }), ...);

                return find_functions;
            }();

            if (auto it = find_functions.find(name); it != find_functions.end()) {
                return it->second(s, o, target_name);
            }

            return sol::make_object(s, sol::lua_nil);
        },
        "find_in_owners", [] (sol::this_state s, sdkgenny::Object& o, const char* name, const char* target_name, bool include_self) -> sol::object {
            /*sol::object result = sol::make_object(s, sol::lua_nil);

            (((
                (result = sol::make_object(s, Args::find_in_owners(o, name, target_name, include_self))) && !result.is<sol::lua_nil_t>()))
                || ...
                || (result = sol::make_object(s, sol::lua_nil))
            );

            return result;*/

            using ft = std::function<sol::object(sol::this_state s, sdkgenny::Object&, std::string_view, bool)>;
            static std::unordered_map<std::string, ft> find_in_owners_functions = []() {
                std::unordered_map<std::string, ft> find_in_owners_functions{};
                (find_in_owners_functions.emplace(Args::name().data(), [] (sol::this_state s, sdkgenny::Object& o, std::string_view target_name, bool include_self) -> sol::object {
                    return sol::make_object(s, o.find_in_owners<typename Args::Type>(target_name, include_self));
                }), ...);

                return find_in_owners_functions;
            }();

            if (auto it = find_in_owners_functions.find(name); it != find_in_owners_functions.end()) {
                return it->second(s, o, target_name, include_self);
            }

            return sol::make_object(s, sol::lua_nil);
        },
        "has_any", [] (sol::this_state s, sdkgenny::Object& o, const char* name) -> bool {
            return (Args::has_any(o, name) || ... || false); // this is faster than using the map
        },
        "has_any_in_children", [] (sol::this_state s, sdkgenny::Object& o, const char* name) -> bool {
            return (Args::has_any_in_children(o, name) || ... || false);
        },
        "owner", [] (sol::this_state s, sdkgenny::Object& o, const char* name) -> sol::object {
            /*sol::object result = sol::make_object(s, sol::lua_nil);

            (((
                (result = sol::make_object(s, Args::owner(o, name))) && !result.is<sol::lua_nil_t>()))
                || ...
                || (result = sol::make_object(s, sol::lua_nil))
            );

            return result;*/

            using ft = std::function<sol::object(sol::this_state s, sdkgenny::Object&)>;
            static std::unordered_map<std::string, ft> find_functions = []() {
                std::unordered_map<std::string, ft> find_functions{};
                (find_functions.emplace(Args::name().data(), [] (sol::this_state s, sdkgenny::Object& o) -> sol::object {
                    return sol::make_object(s, o.owner<typename Args::Type>());
                }), ...);

                return find_functions;
            }();

            if (auto it = find_functions.find(name); it != find_functions.end()) {
                return it->second(s, o);
            }

            return sol::make_object(s, sol::lua_nil);
        },
        "topmost_owner", [] (sol::this_state s, sdkgenny::Object& o, const char* name) -> sol::object {
            /*sol::object result = sol::make_object(s, sol::lua_nil);

            (((
                (result = sol::make_object(s, Args::topmost_owner(o, name))) && !result.is<sol::lua_nil_t>()))
                || ...
                || (result = sol::make_object(s, sol::lua_nil))
            );

            return result;*/

            using ft = std::function<sol::object(sol::this_state s, sdkgenny::Object&)>;
            static std::unordered_map<std::string, ft> find_functions = []() {
                std::unordered_map<std::string, ft> find_functions{};
                (find_functions.emplace(Args::name().data(), [] (sol::this_state s, sdkgenny::Object& o) -> sol::object {
                    return sol::make_object(s, o.topmost_owner<typename Args::Type>());
                }), ...);

                return find_functions;
            }();

            if (auto it = find_functions.find(name); it != find_functions.end()) {
                return it->second(s, o);
            }

            return sol::make_object(s, sol::lua_nil);
        },
        "get_all", [] (sol::this_state s, sdkgenny::Object& o, const char* name) -> sol::object {
            /*sol::object result = sol::make_object(s, sol::lua_nil);

            (((
                (result = sol::make_object(s, Args::get_all(o, name))) && !result.is<sol::lua_nil_t>()))
                || ...
                || (result = sol::make_object(s, sol::lua_nil))
            );

            return result;*/

            using ft = std::function<sol::object(sol::this_state s, sdkgenny::Object&)>;
            static std::unordered_map<std::string, ft> find_functions = []() {
                std::unordered_map<std::string, ft> find_functions{};
                (find_functions.emplace(Args::name().data(), [] (sol::this_state s, sdkgenny::Object& o) -> sol::object {
                    return sol::make_object(s, o.get_all<typename Args::Type>());
                }), ...);

                return find_functions;
            }();

            if (auto it = find_functions.find(name); it != find_functions.end()) {
                return it->second(s, o);
            }

            return sol::make_object(s, sol::lua_nil);
        }
    );

    (object.set(std::string("is_") + Args::name().data(), Args::is_a_standalone), ...);
    (object.set(std::string("as_") + Args::name().data(), Args::as_standalone), ...);
    (object.set(std::string("find_") + Args::name().data(), Args::find_standalone), ...);
    (object.set(std::string("find_") + Args::name().data() + "_in_owners", Args::find_in_owners_standalone), ...);
    (object.set(std::string("has_any_") + Args::name().data(), Args::has_any_standalone), ...);
    (object.set(std::string("has_any_") + Args::name().data() + "_in_children", Args::has_any_in_children_standalone), ...);
    (object.set(std::string("owner_") + Args::name().data(), Args::owner_standalone), ...);
    (object.set(std::string("topmost_owner_") + Args::name().data(), Args::topmost_owner_standalone), ...);
    (object.set(std::string("get_all_") + Args::name().data(), Args::get_all_standalone), ...);
}

int open_object(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    create_bindings<
        TypeDescriptor<sdkgenny::Typename, "typename">,
        TypeDescriptor<sdkgenny::Type, "type">,
        TypeDescriptor<sdkgenny::GenericType, "generic_type">,
        TypeDescriptor<sdkgenny::Struct, "struct">,
        TypeDescriptor<sdkgenny::Class, "class">,
        TypeDescriptor<sdkgenny::Enum, "enum">,
        TypeDescriptor<sdkgenny::EnumClass, "enum_class">,
        TypeDescriptor<sdkgenny::Namespace, "namespace">,
        TypeDescriptor<sdkgenny::Reference, "reference">,
        TypeDescriptor<sdkgenny::Pointer, "pointer">,
        TypeDescriptor<sdkgenny::Variable, "variable">,
        TypeDescriptor<sdkgenny::Function, "function">,
        TypeDescriptor<sdkgenny::VirtualFunction, "virtual_function">,
        TypeDescriptor<sdkgenny::StaticFunction, "static_function">,
        TypeDescriptor<sdkgenny::Array, "array">,
        TypeDescriptor<sdkgenny::Parameter, "parameter">,
        TypeDescriptor<sdkgenny::Constant, "constant">>(sdkgenny);

    return 0;
}
}