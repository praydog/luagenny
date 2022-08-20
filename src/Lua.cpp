#include <spdlog/spdlog.h>
#include <sol/sol.hpp>

#include <Genny.hpp>

#include "Lua.hpp"

namespace sdkgennylua {
#define MULTIFUNCTION(parent_type, name, possible_type) \
    #name, [](sol::this_state s, parent_type& f, sol::object param) -> sol::object { \
        if (param.is<sol::nil_t>()) { \
            return sol::make_object(s, f.name()); \
        } \
        return sol::make_object(s, f.name(param.as<possible_type>())); \
    }

#define FUNCTION(parent_type, name) \
    #name, [](sol::this_state s, parent_type& f) -> sol::object { \
        return sol::make_object(s, f.name()); \
    }

#define FUNCTION2(parent_type, name) \
    #name, [](sol::this_state s, parent_type& f) -> sol::object { \
        return sol::make_object(s, f.name()); \
    }

#define PARAMFUNCTION(parent_type, name, t) \
    #name, [](sol::this_state s, parent_type& f, sol::object param) -> sol::object { \
        return sol::make_object(s, f.name(param.as<t>())); \
    }

int open(lua_State* l) {
    sol::state_view lua{l};

    auto sdkgenny = lua.create_table();
    auto sdk = sdkgenny.new_usertype<genny::Sdk>("Sdk",
        "global_ns", &genny::Sdk::global_ns
    );

    #define GENNY_OBJECT_GEN(luaname, cppname) \
        "is_" #luaname, [](genny::Object& o) { return o.is_a<cppname>(); }, \
        "as_" #luaname, [](genny::Object& o) -> cppname* { return o.is_a<cppname>() ? dynamic_cast<cppname*>(&o) : nullptr; }


    sdkgenny.new_usertype<genny::Object>("Object",
        "metadata", [](sol::this_state s, genny::Object& o) -> std::vector<std::string>& {
            return o.metadata();
        },
        MULTIFUNCTION(genny::Object, name, std::string),
        GENNY_OBJECT_GEN(typename, genny::Typename),
        GENNY_OBJECT_GEN(type, genny::Type),
        GENNY_OBJECT_GEN(generic_type, genny::GenericType),
        GENNY_OBJECT_GEN(struct, genny::Struct),
        GENNY_OBJECT_GEN(class, genny::Class),
        GENNY_OBJECT_GEN(enum, genny::Enum),
        GENNY_OBJECT_GEN(enum_class, genny::EnumClass),
        GENNY_OBJECT_GEN(namespace, genny::Namespace),
        GENNY_OBJECT_GEN(reference, genny::Reference),
        GENNY_OBJECT_GEN(pointer, genny::Pointer),
        GENNY_OBJECT_GEN(variable, genny::Variable),
        GENNY_OBJECT_GEN(function, genny::Function),
        GENNY_OBJECT_GEN(virtual_function, genny::VirtualFunction),
        GENNY_OBJECT_GEN(static_function, genny::StaticFunction),
        GENNY_OBJECT_GEN(array, genny::Array),
        GENNY_OBJECT_GEN(parameter, genny::Parameter)
    );

    sdkgenny.new_usertype<genny::Typename>("Typename",
        sol::base_classes, sol::bases<genny::Object>()
    );

    sdkgenny.new_usertype<genny::Type>("Type",
        sol::base_classes, sol::bases<genny::Typename>(),
        MULTIFUNCTION(genny::Type, size, int)
    );

    sdkgenny.new_usertype<genny::GenericType>("GenericType",
        sol::base_classes, sol::bases<genny::Type>()
    );

    sdkgenny.new_usertype<genny::Struct>("Struct",
        sol::base_classes, sol::bases<genny::Type>(),
        MULTIFUNCTION(genny::Struct, size, int)
    );

    sdkgenny.new_usertype<genny::Class>("Class",
        sol::base_classes, sol::bases<genny::Struct>()
    );

    sdkgenny.new_usertype<genny::Enum>("Enum",
        sol::base_classes, sol::bases<genny::Type>()
    );

    sdkgenny.new_usertype<genny::EnumClass>("EnumClass",
        sol::base_classes, sol::bases<genny::Enum>()
    );

    sdkgenny.new_usertype<genny::Namespace>("Namespace",
        sol::base_classes, sol::bases<genny::Typename>(),
        "type", &genny::Namespace::type,
        "generic_type", &genny::Namespace::generic_type,
        "struct", &genny::Namespace::struct_,
        "enum", &genny::Namespace::enum_,
        "enum_class", &genny::Namespace::enum_class,
        "namespace", &genny::Namespace::namespace_
    );

    sdkgenny.new_usertype<genny::Reference>("Reference",
        sol::base_classes, sol::bases<genny::Type>()
    );

    sdkgenny.new_usertype<genny::Pointer>("Pointer",
        sol::base_classes, sol::bases<genny::Reference>()
    );

    sdkgenny.new_usertype<genny::Variable>("Variable",
        sol::base_classes, sol::bases<genny::Object>()
    );

    sdkgenny.new_usertype<genny::Function>("Function",
        sol::base_classes, sol::bases<genny::Object>(),
        MULTIFUNCTION(genny::Function, returns, genny::Type*),
        MULTIFUNCTION(genny::Function, procedure, std::string),
        FUNCTION(genny::Function, dependencies),
        PARAMFUNCTION(genny::Function, depends_on, genny::Type*),
        MULTIFUNCTION(genny::Function, defined, bool)
    );

    sdkgenny.new_usertype<genny::VirtualFunction>("VirtualFunction",
        sol::base_classes, sol::bases<genny::Function>(),
        MULTIFUNCTION(genny::VirtualFunction, vtable_index, int)
    );

    sdkgenny.new_usertype<genny::StaticFunction>("StaticFunction",
        sol::base_classes, sol::bases<genny::Function>()
    );

    sdkgenny.new_usertype<genny::Array>("Array",
        sol::base_classes, sol::bases<genny::Type>(),
        MULTIFUNCTION(genny::Array, of, genny::Type*),
        MULTIFUNCTION(genny::Array, count, int)
    );

    sdkgenny.new_usertype<genny::Parameter>("Parameter",
        sol::base_classes, sol::bases<genny::Object>(),
        MULTIFUNCTION(genny::Parameter, type, genny::Type*)
    );

    return 0;
} // namespace sdkgennylua
}