extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
#include "ClassMacros.hpp"
#include "Function.hpp"

namespace luagenny {
int open_function(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<sdkgenny::Function>("Function",
        sol::base_classes, sol::bases<sdkgenny::Object>(),
        MULTIFUNCTION(sdkgenny::Function, returns, sdkgenny::Type*),
        MULTIFUNCTION(sdkgenny::Function, procedure, std::string),
        "dependencies", &sdkgenny::Function::dependencies,
        PARAMFUNCTION(sdkgenny::Function, depends_on, sdkgenny::Type*),
        MULTIFUNCTION(sdkgenny::Function, defined, bool)
    );

    return 0;
}
}