extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "Enum.hpp"

namespace luagenny {
int open_enum(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<genny::Enum>("Enum",
        sol::base_classes, sol::bases<GENNY_TYPE_BASES>(),
        "value", [] (genny::Enum& e, std::string name, uint64_t value) {
            e.value(name, value);
        },
        "values", [] (genny::Enum& e) -> std::vector<std::tuple<std::string, uint64_t>> {
            return e.values();
        },
        MULTIFUNCTION(genny::Enum, type, genny::Type*)
    );

    return 0;
}
}