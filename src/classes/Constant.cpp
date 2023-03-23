extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
#include "ClassMacros.hpp"
#include "Constant.hpp"

namespace luagenny {
int open_constant(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<sdkgenny::Constant>("Constant",
        sol::base_classes, sol::bases<sdkgenny::Object>(),
        MULTIFUNCTION(sdkgenny::Constant, type, sdkgenny::Type*),
        PARAMFUNCTION(sdkgenny::Constant, type, std::string),
        MULTIFUNCTION(sdkgenny::Constant, value, std::string),
        "string", &sdkgenny::Constant::string
    );

    return 0;
}
}