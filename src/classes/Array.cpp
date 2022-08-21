extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "Array.hpp"

namespace luagenny {
int open_array(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<genny::Array>("Array",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>(),
        MULTIFUNCTION(genny::Array, of, genny::Type*),
        MULTIFUNCTION(genny::Array, count, int)
    );

    return 0;
}
}