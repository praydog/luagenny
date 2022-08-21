extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "Struct.hpp"

namespace luagenny {
int open_reference(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<genny::Reference>("Reference",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>(),
        MULTIFUNCTION(genny::Reference, to, genny::Type*)
    );

    return 0;
}
}