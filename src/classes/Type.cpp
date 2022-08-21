extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "Type.hpp"

namespace luagenny {
int open_type(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<genny::Type>("Type",
        sol::base_classes, sol::bases<GENNY_TYPENAME_BASES>(),
        MULTIFUNCTION(genny::Type, size, int),
        "ref", &genny::Type::ref,
        "ptr", &genny::Type::ptr,
        "array", &genny::Type::array_
    );

    return 0;
}
}