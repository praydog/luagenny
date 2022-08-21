extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "Constant.hpp"

namespace luagenny {
int open_constant(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<genny::Constant>("Constant",
        sol::base_classes, sol::bases<genny::Object>(),
        MULTIFUNCTION(genny::Constant, type, genny::Type*),
        PARAMFUNCTION(genny::Constant, type, std::string),
        MULTIFUNCTION(genny::Constant, value, std::string),
        "string", &genny::Constant::string
    );

    return 0;
}
}