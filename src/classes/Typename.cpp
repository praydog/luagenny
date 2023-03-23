extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
#include "ClassMacros.hpp"
#include "Typename.hpp"

namespace luagenny {
int open_typename(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<sdkgenny::Typename>("Typename",
        sol::base_classes, sol::bases<sdkgenny::Object>(),
        MULTIFUNCTION(sdkgenny::Typename, simple_typename_generation, bool)
    );

    return 0;
}
}