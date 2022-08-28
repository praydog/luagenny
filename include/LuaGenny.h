#ifndef LUAGENNY_H
#define LUAGENNY_H

struct lua_State;

#ifdef __cplusplus
extern "C" {
#endif
#ifdef LUAGENNY_EXPORTS
__declspec(dllexport) int luaopen_luagenny(lua_State* l);
#else
int luaopen_luagenny(lua_State* l);
#endif
#ifdef __cplusplus
}
#endif

#endif // LUAGENNY_H
