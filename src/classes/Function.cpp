extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "Function.hpp"

namespace luagenny {
int open_function(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<genny::Function>("Function",
        sol::base_classes, sol::bases<genny::Object>(),
        MULTIFUNCTION(genny::Function, returns, genny::Type*),
        MULTIFUNCTION(genny::Function, procedure, std::string),
        "dependencies", &genny::Function::dependencies,
        PARAMFUNCTION(genny::Function, depends_on, genny::Type*),
        MULTIFUNCTION(genny::Function, defined, bool)
    );

    return 0;
}
}