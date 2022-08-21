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
int open_struct(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<genny::Struct>("Struct",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>(),
        MULTIFUNCTION(genny::Struct, size, int),
        "variable", &genny::Struct::variable,
        "constant", &genny::Struct::constant,
        "struct", &genny::Struct::struct_,
        "class", &genny::Struct::class_,
        "enum", &genny::Struct::enum_,
        "enum_class", &genny::Struct::enum_class,
        "function", &genny::Struct::function,
        "virtual_function", &genny::Struct::virtual_function,
        "static_function", &genny::Struct::static_function,
        "bitfield", &genny::Struct::bitfield
    );

    return 0;
}
}