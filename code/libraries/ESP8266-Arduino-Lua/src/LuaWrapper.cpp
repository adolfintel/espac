#include "LuaWrapper.h"

LuaWrapper::LuaWrapper() {
  _state = luaL_newstate();
}

String LuaWrapper::Lua_dostring(const String *script) {
  String result;
  if (luaL_dostring(_state, script->c_str())) {
    result = "# lua error:\n" + String(lua_tostring(_state, -1));
    lua_pop(_state, 1);
  }
  return result;
}

void LuaWrapper::Lua_register(const String name, const lua_CFunction function) {
  lua_register(_state, name.c_str(), function);
}
