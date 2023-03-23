extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
#include "ClassMacros.hpp"
#include "Namespace.hpp"

namespace luagenny {
int open_namespace(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<sdkgenny::Namespace>("Namespace",
        sol::base_classes, sol::bases<GENNY_TYPENAME_BASES>(),
        "type", &sdkgenny::Namespace::type,
        "generic_type", &sdkgenny::Namespace::generic_type,
        "struct", &sdkgenny::Namespace::struct_,
        "enum", &sdkgenny::Namespace::enum_,
        "enum_class", &sdkgenny::Namespace::enum_class,
        "namespace", &sdkgenny::Namespace::namespace_
    );

    return 0;
}
}