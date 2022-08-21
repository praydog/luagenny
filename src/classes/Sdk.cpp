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
int open_sdk(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<genny::Sdk>("Sdk",
        "global_ns", &genny::Sdk::global_ns,
        "preamble", &genny::Sdk::preamble,
        "postamble", &genny::Sdk::postamble,
        "include", &genny::Sdk::include,
        "include_local", &genny::Sdk::include_local,
        "generate", [](genny::Sdk& sdk, std::string p) {
            return sdk.generate(p);
        },
        MULTIFUNCTION(genny::Sdk, header_extension, std::string),
        MULTIFUNCTION(genny::Sdk, source_extension, std::string),
        MULTIFUNCTION(genny::Sdk, generate_namespaces, bool)
    );

    return 0;
}
}