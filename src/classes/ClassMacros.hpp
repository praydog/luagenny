#pragma once

#include <sol/sol.hpp>

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
        if (param.is<sol::lua_nil_t>()) { \
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

#define GENNY_TYPENAME_BASES genny::Typename, genny::Object
#define GENNY_TYPE_BASES genny::Type, genny::Typename, genny::Object
#define GENNY_REFERENCE_BASES genny::Reference, GENNY_TYPE_BASES