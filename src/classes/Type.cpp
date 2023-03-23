extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
#include "ClassMacros.hpp"
#include "Type.hpp"

namespace luagenny {
int open_type(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<sdkgenny::Type>("Type",
        sol::base_classes, sol::bases<GENNY_TYPENAME_BASES>(),
        MULTIFUNCTION(sdkgenny::Type, size, int),
        "ref", &sdkgenny::Type::ref,
        "ptr", &sdkgenny::Type::ptr,
        "array", &sdkgenny::Type::array_
    );

    return 0;
}
}