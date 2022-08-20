#include <iostream>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <sol/sol.hpp>

#include <LuaGenny.hpp>

int main() {
    // Create a Lua state
    sol::state lua{};

    // Add the sdkgenny bindings
    sdkgennylua::open(lua);
    sol::table sdkgenny = sol::stack::pop<sol::table>(lua);

    std::cout << "Hello, World!" << std::endl;

    std::system("pause");
    return 0;
}