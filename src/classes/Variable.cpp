extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
#include "ClassMacros.hpp"
#include "Variable.hpp"

namespace luagenny {
int open_variable(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<sdkgenny::Variable>("Variable",
        sol::base_classes, sol::bases<sdkgenny::Object>(),
        "type", [](sol::this_state s, sdkgenny::Variable& f, sol::object param) -> sol::object {
            if (param.is<sol::lua_nil_t>()) {
                return sol::make_object(s, f.type());
            }

            if (param.is<std::string>()) {
                return sol::make_object(s, f.type(param.as<std::string>()));
            }

            return sol::make_object(s, f.type(param.as<sdkgenny::Type*>()));
        },
        MULTIFUNCTION(sdkgenny::Variable, offset, int),
        "append", &sdkgenny::Variable::append,
        "end", &sdkgenny::Variable::end,

        MULTIFUNCTION(sdkgenny::Variable, bit_size, size_t),
        MULTIFUNCTION(sdkgenny::Variable, bit_offset, size_t),
        "is_bitfield", &sdkgenny::Variable::is_bitfield,
        "bit_append", &sdkgenny::Variable::bit_append
    );


    return 0;
}
}