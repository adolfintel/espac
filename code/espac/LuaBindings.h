#include<LuaWrapper.h>

LuaWrapper lua; //LUA interpreter

static int lua_getTemperature(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)temperature);
  return 1;
}
static int lua_getHumidity(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)humidity);
  return 1;
}
static int lua_getPressure(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)pressure);
  return 1;
}
static int lua_getApparentTemperature(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)apparentTemperature);
  return 1;
}
static int lua_getYear(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)year);
  return 1;
}
static int lua_getMonth(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)month);
  return 1;
}
static int lua_getDay(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)day);
  return 1;
}
static int lua_getHour(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)hour);
  return 1;
}
static int lua_getMinute(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)minute);
  return 1;
}
static int lua_getDayOfWeek(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)dayOfWeek);
  return 1;
}
static int lua_turnOn(lua_State *lua){
  if(!ac.getPower()){
    ac.on();
    mustResend=true;
  }
}
static int lua_turnOff(lua_State *lua){
  if(ac.getPower()){
    ac.off();
    powerfulStartedAt=0;
    mustResend=true;
  }
}
static int lua_isOn(lua_State *lua){
  lua_pushboolean(lua,ac.getPower());
  return 1;
}
static int lua_enablePowerful(lua_State *lua){
  if(!isPowerful()){
    powerfulStartedAt=millis();
    mustResend=true;
  }
}
static int lua_disablePowerful(lua_State *lua){
  if(isPowerful()){
    powerfulStartedAt=0;
    mustResend=true;
  }
}
static int lua_isPowerful(lua_State *lua){
  lua_pushboolean(lua,isPowerful());
  return 1;
}
static int lua_enableSwing(lua_State *lua){
  if(!ac.getSwingVertical()){
    ac.setSwingVertical(true);
    mustResend=true;
  }
}
static int lua_disableSwing(lua_State *lua){
  if(ac.getSwingVertical()){
    ac.setSwingVertical(false);
    mustResend=true;
  }
}
static int lua_isSwingEnabled(lua_State *lua){
  lua_pushboolean(lua,ac.getSwingVertical());
  return 1;
}
static int lua_enableQuiet(lua_State *lua){
  if(!ac.getQuiet()){
    ac.setQuiet(true);
    mustResend=true;
  }
}
static int lua_disableQuiet(lua_State *lua){
  if(ac.getQuiet()){
    ac.setQuiet(false);
    mustResend=true;
  }
}
static int lua_isQuiet(lua_State *lua){
  lua_pushboolean(lua,ac.getQuiet());
  return 1;
}
static int lua_enableComfort(lua_State *lua){
  if(!ac.getComfort()){
    ac.setComfort(true);
    mustResend=true;
  }
}
static int lua_disableComfort(lua_State *lua){
  if(ac.getComfort()){
    ac.setComfort(false);
    mustResend=true;
  }
}
static int lua_isComfort(lua_State *lua){
  lua_pushboolean(lua,ac.getComfort());
  return 1;
}
static int lua_coolMode(lua_State *lua){
  if(ac.getMode()!=kDaikinCool){
    ac.setMode(kDaikinCool);
    ac.setTemp(18);
    mustResend=true;
  }
}
static int lua_heatMode(lua_State *lua){
  if(ac.getMode()!=kDaikinHeat){
    ac.setMode(kDaikinHeat);
    ac.setTemp(28);
    mustResend=true;
  }
}
static int lua_fanMode(lua_State *lua){
  if(ac.getMode()!=kDaikinFan){
    ac.setMode(kDaikinFan);
    mustResend=true;
  }
}
static int lua_dryMode(lua_State *lua){
  if(ac.getMode()!=kDaikinDry){
    ac.setMode(kDaikinDry);
    mustResend=true;
  }
}
static int lua_isCoolMode(lua_State *lua){
  lua_pushboolean(lua,ac.getMode()==kDaikinCool);
  return 1;
}
static int lua_isHeatMode(lua_State *lua){
  lua_pushboolean(lua,ac.getMode()==kDaikinHeat);
  return 1;
}
static int lua_isFanMode(lua_State *lua){
  lua_pushboolean(lua,ac.getMode()==kDaikinFan);
  return 1;
}
static int lua_isDryMode(lua_State *lua){
  lua_pushboolean(lua,ac.getMode()==kDaikinDry);
  return 1;
}
static int lua_setFanSpeed(lua_State *lua){
  int speed = luaL_checkinteger(lua, 1);
  speed=speed<kDaikinFanMin?kDaikinFanMin:speed>kDaikinFanMax?kDaikinFanMax:speed;
  if(ac.getFan()!=speed){
    ac.setFan(speed);
    mustResend=true;
  }
}
static int lua_getFanSpeed(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)ac.getFan());
  return 1;
}
static int lua_setACTemp(lua_State *lua){
  int temp = luaL_checkinteger(lua, 1);
  temp=temp<18?18:temp>28?28:temp;
  if(ac.getTemp()!=temp){
    ac.setTemp(temp);
    mustResend=true;
  }
}
static int lua_getACTemp(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)ac.getTemp());
  return 1;
}
static int lua_setOnTimer(lua_State *lua){
  int h = luaL_checkinteger(lua, 1);
  int m = luaL_checkinteger(lua, 2);
  h=h<0?0:h>23?23:h;
  m=m<0?0:m>59?59:m;
  uint16_t currentTimer=ac.getOnTime();
  if(currentTimer/60!=h||currentTimer%60!=m){
    ac.enableOnTimer(h*60+m);
    mustResend=true;
  }
}
static int lua_getOnTimerH(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)(ac.getOnTime()/60));
  return 1;
}
static int lua_getOnTimerM(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)(ac.getOnTime() %60));
  return 1;
}
static int lua_setOffTimer(lua_State *lua){
  int h = luaL_checkinteger(lua, 1);
  int m = luaL_checkinteger(lua, 2);
  h=h<0?0:h>23?23:h;
  m=m<0?0:m>59?59:m;
  uint16_t currentTimer=ac.getOffTime();
  if(currentTimer/60!=h||currentTimer%60!=m){
    ac.enableOffTimer(h*60+m);
    mustResend=true;
  }
}
static int lua_getOffTimerH(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)(ac.getOffTime()/60));
  return 1;
}
static int lua_getOffTimerM(lua_State *lua){
  lua_pushnumber(lua,(lua_Number)(ac.getOffTime()%60));
  return 1;
}
static int lua_disableOnTimer(lua_State *lua){
  if(ac.getOnTimerEnabled()){
    ac.disableOnTimer();
    mustResend=true;
  }
}
static int lua_isOnTimerEnabled(lua_State *lua){
  lua_pushboolean(lua,ac.getOnTimerEnabled());
  return 1;
}
static int lua_disableOffTimer(lua_State *lua){
  if(ac.getOffTimerEnabled()){
    ac.disableOffTimer();
    mustResend=true;
  }
}
static int lua_isOffTimerEnabled(lua_State *lua){
  lua_pushboolean(lua,ac.getOffTimerEnabled());
  return 1;
}
static int lua_forceSend(lua_State *lua){
  mustResend=true;
}
static int lua_reset(lua_State *lua){
  acReset();
  mustResend=true;
}
static int lua_blink(lua_State *lua){
  int duration = luaL_checkinteger(lua, 1);
  blink(duration);
}

void initLuaBindings(){
    lua.Lua_register("getTemperature", (const lua_CFunction) &lua_getTemperature);
    lua.Lua_register("getHumidity", (const lua_CFunction) &lua_getHumidity);
    lua.Lua_register("getPressure", (const lua_CFunction) &lua_getPressure);
    lua.Lua_register("getApparentTemperature", (const lua_CFunction) &lua_getApparentTemperature);
    lua.Lua_register("getYear", (const lua_CFunction) &lua_getYear);
    lua.Lua_register("getMonth", (const lua_CFunction) &lua_getMonth);
    lua.Lua_register("getDay", (const lua_CFunction) &lua_getDay);
    lua.Lua_register("getHour", (const lua_CFunction) &lua_getHour);
    lua.Lua_register("getMinute", (const lua_CFunction) &lua_getMinute);
    lua.Lua_register("getDayOfWeek", (const lua_CFunction) &lua_getDayOfWeek);
    lua.Lua_register("turnOn", (const lua_CFunction) &lua_turnOn);
    lua.Lua_register("turnOff", (const lua_CFunction) &lua_turnOff);
    lua.Lua_register("isOn", (const lua_CFunction) &lua_isOn);
    lua.Lua_register("enablePowerful", (const lua_CFunction) &lua_enablePowerful);
    lua.Lua_register("disablePowerful", (const lua_CFunction) &lua_disablePowerful);
    lua.Lua_register("isPowerful", (const lua_CFunction) &lua_isPowerful);
    lua.Lua_register("enableSwing", (const lua_CFunction) &lua_enableSwing);
    lua.Lua_register("disableSwing", (const lua_CFunction) &lua_disableSwing);
    lua.Lua_register("isSwingEnabled", (const lua_CFunction) &lua_isSwingEnabled);
    lua.Lua_register("enableQuiet", (const lua_CFunction) &lua_enableQuiet);
    lua.Lua_register("disableQuiet", (const lua_CFunction) &lua_disableQuiet);
    lua.Lua_register("isQuiet", (const lua_CFunction) &lua_isQuiet);
    lua.Lua_register("enableComfort", (const lua_CFunction) &lua_enableComfort);
    lua.Lua_register("disableComfort", (const lua_CFunction) &lua_disableComfort);
    lua.Lua_register("isComfort", (const lua_CFunction) &lua_isComfort);
    lua.Lua_register("coolMode", (const lua_CFunction) &lua_coolMode);
    lua.Lua_register("heatMode", (const lua_CFunction) &lua_heatMode);
    lua.Lua_register("fanMode", (const lua_CFunction) &lua_fanMode);
    lua.Lua_register("dryMode", (const lua_CFunction) &lua_dryMode);
    lua.Lua_register("isCoolMode", (const lua_CFunction) &lua_isCoolMode);
    lua.Lua_register("isHeatMode", (const lua_CFunction) &lua_isHeatMode);
    lua.Lua_register("isFanMode", (const lua_CFunction) &lua_isFanMode);
    lua.Lua_register("isDryMode", (const lua_CFunction) &lua_isDryMode);
    lua.Lua_register("setFanSpeed", (const lua_CFunction) &lua_setFanSpeed);
    lua.Lua_register("getFanSpeed", (const lua_CFunction) &lua_getFanSpeed);
    lua.Lua_register("setACTemp", (const lua_CFunction) &lua_setACTemp);
    lua.Lua_register("getACTemp", (const lua_CFunction) &lua_getACTemp);
    lua.Lua_register("setOnTimer", (const lua_CFunction) &lua_setOnTimer);
    lua.Lua_register("getOnTimerH", (const lua_CFunction) &lua_getOnTimerH);
    lua.Lua_register("getOnTimerM", (const lua_CFunction) &lua_getOnTimerM);
    lua.Lua_register("disableOnTimer", (const lua_CFunction) &lua_disableOnTimer);
    lua.Lua_register("isOnTimerEnabled", (const lua_CFunction) &lua_isOnTimerEnabled);
    lua.Lua_register("setOffTimer", (const lua_CFunction) &lua_setOnTimer);
    lua.Lua_register("getOffTimerH", (const lua_CFunction) &lua_getOffTimerH);
    lua.Lua_register("getOffTimerM", (const lua_CFunction) &lua_getOffTimerM);
    lua.Lua_register("disableOffTimer", (const lua_CFunction) &lua_disableOffTimer);
    lua.Lua_register("isOffTimerEnabled", (const lua_CFunction) &lua_isOffTimerEnabled);
    lua.Lua_register("forceSend", (const lua_CFunction) &lua_forceSend);
    lua.Lua_register("reset", (const lua_CFunction) &lua_reset);
    lua.Lua_register("blink", (const lua_CFunction) &lua_blink);
}
