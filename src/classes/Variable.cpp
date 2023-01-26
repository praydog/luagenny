extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "Variable.hpp"

namespace luagenny {
int open_variable(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<genny::Variable>("Variable",
        sol::base_classes, sol::bases<genny::Object>(),
        "type", [](sol::this_state s, genny::Variable& f, sol::object param) -> sol::object {
            if (param.is<sol::lua_nil_t>()) {
                return sol::make_object(s, f.type());
            }

            if (param.is<std::string>()) {
                return sol::make_object(s, f.type(param.as<std::string>()));
            }

            return sol::make_object(s, f.type(param.as<genny::Type*>()));
        },
        MULTIFUNCTION(genny::Variable, offset, int),
        "append", &genny::Variable::append,
        "end", &genny::Variable::end,

        MULTIFUNCTION(genny::Variable, bit_size, size_t),
        MULTIFUNCTION(genny::Variable, bit_offset, size_t),
        "is_bitfield", &genny::Variable::is_bitfield,
        "bit_append", &genny::Variable::bit_append
    );


    return 0;
}
}