extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
#include "ClassMacros.hpp"
#include "VirtualFunction.hpp"

namespace luagenny {
int open_virtual_function(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<sdkgenny::VirtualFunction>("VirtualFunction",
        sol::base_classes, sol::bases<sdkgenny::Function, sdkgenny::Object>(),
        MULTIFUNCTION(sdkgenny::VirtualFunction, vtable_index, int)
    );

    return 0;
}
}