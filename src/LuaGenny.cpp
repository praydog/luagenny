extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <sol/sol.hpp>

#include <Genny.hpp>

#include "LuaGenny.hpp"

namespace sdkgennylua {
#define GENNY_OBJECT_GEN(luaname, cppname) \
    "is_" #luaname, [](genny::Object& o) { return o.is_a<cppname>(); }, \
    "as_" #luaname, [](genny::Object& o) -> cppname* { return o.is_a<cppname>() ? dynamic_cast<cppname*>(&o) : nullptr; }, \
    "has_any_" #luaname, [](genny::Object& o) { return o.has_any<cppname>(); }, \
    "has_any_" #luaname "_in_children", [](genny::Object& o) { return o.has_any_in_children<cppname>(); }, \
    #luaname "_owners", [](genny::Object& o) { return o.owners<cppname>(); }, \
    "get_all_" #luaname, [](genny::Object& o) { return o.get_all<cppname>(); }, \
    "is_child_of_" #luaname, [](genny::Object& o, cppname* obj) { return o.is_child_of<cppname>(obj); }, \
    "find_" #luaname, [](genny::Object& o, std::string name) { return o.find<cppname>(name); }, \
    "find_" #luaname "_in_owners", [](genny::Object& o, std::string name, bool include_self) { return o.find_in_owners<cppname>(name, include_self); }
    

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

#define PARAMFUNCTION(parent_type, name, t) \
    #name, [](sol::this_state s, parent_type& f, sol::object param) -> sol::object { \
        return sol::make_object(s, f.name(param.as<t>())); \
    }

int open(lua_State* l) {
    sol::state_view lua{l};

    auto sdkgenny = lua.create_table();
    auto sdk = sdkgenny.new_usertype<genny::Sdk>("Sdk",
        "global_ns", &genny::Sdk::global_ns,
        "preamble", &genny::Sdk::preamble,
        "postamble", &genny::Sdk::postamble,
        "include", &genny::Sdk::include,
        "include_local", &genny::Sdk::include_local,
        "generate", [](genny::Sdk& sdk, std::string p) {
            return sdk.generate(p);
        },
        MULTIFUNCTION(genny::Sdk, header_extension, std::string),
        MULTIFUNCTION(genny::Sdk, source_extension, std::string),
        MULTIFUNCTION(genny::Sdk, generate_namespaces, bool)
    );

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
        GENNY_OBJECT_GEN(parameter, genny::Parameter),
        GENNY_OBJECT_GEN(constant, genny::Constant)
    );

    #define GENNY_TYPENAME_BASES genny::Typename, genny::Object
    sdkgenny.new_usertype<genny::Typename>("Typename",
        sol::base_classes, sol::bases<genny::Object>(),
        MULTIFUNCTION(genny::Typename, simple_typename_generation, bool)
    );

    #define GENNY_TYPE_BASES genny::Type, genny::Typename, genny::Object
    sdkgenny.new_usertype<genny::Type>("Type",
        sol::base_classes, sol::bases<GENNY_TYPENAME_BASES>(),
        MULTIFUNCTION(genny::Type, size, int),
        "ref", &genny::Type::ref,
        "ptr", &genny::Type::ptr,
        "array", &genny::Type::array_
    );

    sdkgenny.new_usertype<genny::GenericType>("GenericType",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>()
    );

    sdkgenny.new_usertype<genny::Struct>("Struct",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>(),
        MULTIFUNCTION(genny::Struct, size, int),
        "variable", &genny::Struct::variable,
        "constant", &genny::Struct::constant,
        "struct", &genny::Struct::struct_,
        "class", &genny::Struct::class_,
        "enum", &genny::Struct::enum_,
        "enum_class", &genny::Struct::enum_class,
        "function", &genny::Struct::function,
        "virtual_function", &genny::Struct::virtual_function,
        "static_function", &genny::Struct::static_function,
        "bitfield", &genny::Struct::bitfield
    );

    sdkgenny.new_usertype<genny::Class>("Class",
        sol::base_classes, sol::bases<genny::Struct, GENNY_TYPE_BASES>()
    );

    sdkgenny.new_usertype<genny::Enum>("Enum",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>(),
        "value", [] (genny::Enum& e, std::string name, uint64_t value) {
            e.value(name, value);
        },
        "values", [] (genny::Enum& e) -> std::vector<std::tuple<std::string, uint64_t>> {
            return e.values();
        },
        MULTIFUNCTION(genny::Enum, type, genny::Type*)
    );

    sdkgenny.new_usertype<genny::EnumClass>("EnumClass",
        sol::base_classes, sol::bases<genny::Enum, GENNY_TYPE_BASES>()
    );

    sdkgenny.new_usertype<genny::Namespace>("Namespace",
        sol::base_classes, sol::bases<GENNY_TYPENAME_BASES>(),
        "type", &genny::Namespace::type,
        "generic_type", &genny::Namespace::generic_type,
        "struct", &genny::Namespace::struct_,
        "enum", &genny::Namespace::enum_,
        "enum_class", &genny::Namespace::enum_class,
        "namespace", &genny::Namespace::namespace_
    );

    #define GENNY_REFERENCE_BASES genny::Reference, GENNY_TYPE_BASES
    sdkgenny.new_usertype<genny::Reference>("Reference",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>(),
        MULTIFUNCTION(genny::Reference, to, genny::Type*)
    );

    sdkgenny.new_usertype<genny::Pointer>("Pointer",
        sol::base_classes, sol::bases<GENNY_REFERENCE_BASES>()
    );

    sdkgenny.new_usertype<genny::Variable>("Variable",
        sol::base_classes, sol::bases<genny::Object>(),
        MULTIFUNCTION(genny::Variable, type, genny::Type*),
        PARAMFUNCTION(genny::Variable, type, std::string),
        MULTIFUNCTION(genny::Variable, offset, int),
        "append", &genny::Variable::append,
        "end", &genny::Variable::end,

        MULTIFUNCTION(genny::Variable, bit_size, size_t),
        MULTIFUNCTION(genny::Variable, bit_offset, size_t),
        "is_bitfield", &genny::Variable::is_bitfield,
        "bit_append", &genny::Variable::bit_append
    );

    sdkgenny.new_usertype<genny::Function>("Function",
        sol::base_classes, sol::bases<genny::Object>(),
        MULTIFUNCTION(genny::Function, returns, genny::Type*),
        MULTIFUNCTION(genny::Function, procedure, std::string),
        "dependencies", &genny::Function::dependencies,
        PARAMFUNCTION(genny::Function, depends_on, genny::Type*),
        MULTIFUNCTION(genny::Function, defined, bool)
    );

    sdkgenny.new_usertype<genny::VirtualFunction>("VirtualFunction",
        sol::base_classes, sol::bases<genny::Function, genny::Object>(),
        MULTIFUNCTION(genny::VirtualFunction, vtable_index, int)
    );

    sdkgenny.new_usertype<genny::StaticFunction>("StaticFunction",
        sol::base_classes, sol::bases<genny::Function, genny::Object>()
    );

    sdkgenny.new_usertype<genny::Array>("Array",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>(),
        MULTIFUNCTION(genny::Array, of, genny::Type*),
        MULTIFUNCTION(genny::Array, count, int)
    );

    sdkgenny.new_usertype<genny::Parameter>("Parameter",
        sol::base_classes, sol::bases<genny::Object>(),
        MULTIFUNCTION(genny::Parameter, type, genny::Type*)
    );

    sdkgenny.new_usertype<genny::Constant>("Constant",
        sol::base_classes, sol::bases<genny::Object>(),
        MULTIFUNCTION(genny::Constant, type, genny::Type*),
        PARAMFUNCTION(genny::Constant, type, std::string),
        MULTIFUNCTION(genny::Constant, value, std::string),
        "string", &genny::Constant::string
    );

    sdkgenny.push(l);
    return 1;
} // namespace sdkgennylua
}