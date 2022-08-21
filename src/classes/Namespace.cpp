extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "Genny.hpp"
#include "ClassMacros.hpp"
#include "Namespace.hpp"

namespace luagenny {
int open_namespace(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<genny::Namespace>("Namespace",
        sol::base_classes, sol::bases<GENNY_TYPENAME_BASES>(),
        "type", &genny::Namespace::type,
        "generic_type", &genny::Namespace::generic_type,
        "struct", &genny::Namespace::struct_,
        "enum", &genny::Namespace::enum_,
        "enum_class", &genny::Namespace::enum_class,
        "namespace", &genny::Namespace::namespace_
    );

    return 0;
}
}