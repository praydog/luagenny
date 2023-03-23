extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <sol/sol.hpp>

#include "sdkgenny.hpp"
#include "ClassMacros.hpp"
#include "Struct.hpp"

namespace luagenny {
int open_sdk(lua_State* l) {
    sol::table sdkgenny = sol::stack::pop<sol::table>(l);

    sdkgenny.new_usertype<sdkgenny::Sdk>("Sdk",
        "global_ns", &sdkgenny::Sdk::global_ns,
        "preamble", &sdkgenny::Sdk::preamble,
        "postamble", &sdkgenny::Sdk::postamble,
        "include", &sdkgenny::Sdk::include,
        "include_local", &sdkgenny::Sdk::include_local,
        "generate", [](sdkgenny::Sdk& sdk, std::string p) {
            return sdk.generate(p);
        },
        MULTIFUNCTION(sdkgenny::Sdk, header_extension, std::string),
        MULTIFUNCTION(sdkgenny::Sdk, source_extension, std::string),
        MULTIFUNCTION(sdkgenny::Sdk, generate_namespaces, bool)
    );

    return 0;
}
}