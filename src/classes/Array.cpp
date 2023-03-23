extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
#include "ClassMacros.hpp"
#include "Array.hpp"

namespace luagenny {
int open_array(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<sdkgenny::Array>("Array",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>(),
        MULTIFUNCTION(sdkgenny::Array, of, sdkgenny::Type*),
        MULTIFUNCTION(sdkgenny::Array, count, int)
    );

    return 0;
}
}