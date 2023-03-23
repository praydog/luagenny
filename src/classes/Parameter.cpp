extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
#include "ClassMacros.hpp"
#include "Parameter.hpp"

namespace luagenny {
int open_parameter(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<sdkgenny::Parameter>("Parameter",
        sol::base_classes, sol::bases<sdkgenny::Object>(),
        MULTIFUNCTION(sdkgenny::Parameter, type, sdkgenny::Type*)
    );

    return 0;
}
}