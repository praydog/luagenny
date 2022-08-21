extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "Parameter.hpp"

namespace luagenny {
int open_parameter(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<genny::Parameter>("Parameter",
        sol::base_classes, sol::bases<genny::Object>(),
        MULTIFUNCTION(genny::Parameter, type, genny::Type*)
    );

    return 0;
}
}