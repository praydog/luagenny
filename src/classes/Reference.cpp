extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
#include "ClassMacros.hpp"
#include "Struct.hpp"

namespace luagenny {
int open_reference(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<sdkgenny::Reference>("Reference",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>(),
        MULTIFUNCTION(sdkgenny::Reference, to, sdkgenny::Type*)
    );

    return 0;
}
}