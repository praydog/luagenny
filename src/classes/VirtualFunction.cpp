extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "VirtualFunction.hpp"

namespace luagenny {
int open_virtual_function(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<genny::VirtualFunction>("VirtualFunction",
        sol::base_classes, sol::bases<genny::Function, genny::Object>(),
        MULTIFUNCTION(genny::VirtualFunction, vtable_index, int)
    );

    return 0;
}
}