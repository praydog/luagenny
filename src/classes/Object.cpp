extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "Object.hpp"

namespace luagenny {
int open_object(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

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

    return 0;
}
}