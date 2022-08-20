#include <spdlog/spdlog.h>

#include "Lua.hpp"

namespace sdkgennylua {
int open(lua_State *L) {
    spdlog::info("Right now this does nothing.");
    return 0;
} // namespace sdkgennylua
}