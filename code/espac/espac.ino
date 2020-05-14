#include "IRremoteFlags.h"
#include<ir_Daikin.h>
#include "SPIFFSFlags.h"
#include<FS.h>
#include<ESP8266WiFi.h>
#include "Sensors.h"

#define IR_LED D5 //pin to which the IR led is connected
#define STATUS_LED D0 //pin to which the status led is connected

#define PERIOD 60000 //how often to update sensors and run script
#define WATCHDOG_TIMEOUT 5000 //how long to wait before resetting the board if the script freezes

IRDaikinESP ac(IR_LED);  //virutal ARC433 remote

bool mustResend=false;
#define POWERFUL_DURATION 1200000 //powerful mode lasts 20 minutes
unsigned long powerfulStartedAt=0;
bool isPowerful(){
  return powerfulStartedAt>0&&((millis()-powerfulStartedAt)<=POWERFUL_DURATION);
}
void acReset(){
  ac.off();
  ac.setMode(kDaikinCool);
  ac.setTemp(18);
  ac.setFan(1);
  ac.setQuiet(false);
  ac.setComfort(true);
  ac.setPowerful(false);
  ac.disableOnTimer();
  ac.disableOffTimer();
  powerfulStartedAt=0;
}
void blink(int duration){
  //this function can be called from user scripts so we can't use delay here because it resets the watchdog
  unsigned long t1=millis()+duration/2, t2=t1+duration/2;
  if(duration>0&&duration<=2000){
    digitalWrite(STATUS_LED,HIGH);
    while(millis()<t1);
    digitalWrite(STATUS_LED,LOW);
    while(millis()<t2);
  }
}

String const SCRIPT_FILENAME="/script.lua", WIFI_FILENAME="/wifi.txt", NEWSCRIPT_FILENAME="/script.new", MODE_FILENAME="/mode.txt";
String script="",tempScript="";
bool reloadScriptASAP=false, restartNetASAP=true;
bool manualOnly=false;
String const default_ssid="ESPAC_AP", default_wifipsk="ESP", default_hostName="ESPAC";
String ssid=default_ssid, wifipsk=default_wifipsk, hostName=default_hostName;
bool wifiModeAP=true;

#include "LuaBindings.h"
#include "WebUI.h"

void setup() {
    pinMode(STATUS_LED,OUTPUT);
    digitalWrite(STATUS_LED,HIGH);
    Serial.begin(9600);
    initSensors();
    ac.begin();
    acReset();
    SPIFFS.begin();
    WiFi.disconnect(true);
    File s;
    uint8_t r=ESP.getResetInfoPtr()->reason;
    if(r!=REASON_EXT_SYS_RST&&r!=REASON_SOFT_RESTART){
      //if we just recovered from a watchdog reset, don't load the script and inform the user with some blinking
      for(int i=0;i<30;i++) blink(100);
    }else{
      //otherwise, just load the script
      if(SPIFFS.exists(SCRIPT_FILENAME)){
        reloadScriptASAP=true;
      }
    }
    if(SPIFFS.exists(WIFI_FILENAME)){
      File s=SPIFFS.open(WIFI_FILENAME,"r");
      String m=s.readStringUntil('\n'); m.trim();
      wifiModeAP=m.compareTo("c")!=0;
      ssid=s.readStringUntil('\n'); ssid.trim();
      wifipsk=s.readStringUntil('\n'); wifipsk.trim();
      hostName=s.readStringUntil('\n'); hostName.trim();
      s.close();
    }
    if(SPIFFS.exists(MODE_FILENAME)){
      s=SPIFFS.open(MODE_FILENAME,"r");
      String m=s.readStringUntil('\n'); m.trim();
      manualOnly=m.compareTo("m")==0;
      s.close();
    }
    initWebUI();
    initLuaBindings();
    ESP.wdtEnable(WATCHDOG_TIMEOUT);
    digitalWrite(STATUS_LED,LOW);
}

#define WIFI_TIMEOUT 300000 //how long to wait before switching to AP mode if we've never been connected to a network
bool wifiConnected=false; unsigned long disconnectedSince=0, lastNetReset=0;
unsigned long ts=0;
void loop() {
  if(restartNetASAP){
    ESP.wdtFeed();
    restartNetASAP=false;
    WiFi.disconnect(true);
    yield();
    if(!wifiModeAP){
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, wifipsk);
      WiFi.setAutoReconnect(true);
    }else{
      WiFi.softAP(ssid,wifipsk);  
    }
    WiFi.hostname(hostName);
    wifiConnected=false;
    disconnectedSince=millis();
    lastNetReset=disconnectedSince;
  }
  if(reloadScriptASAP){
    ESP.wdtFeed();
    reloadScriptASAP=false;
    script="";
    yield();
    File s=SPIFFS.open(SCRIPT_FILENAME,"r");
    script=s.readStringUntil(0);
    s.close();
  }
  bool forced=tempScript.length()>0;
  if(millis()-ts>=PERIOD||ts==0||forced){
    ESP.wdtFeed();
    if(!forced){
      updateSensors();
      ESP.wdtFeed();
    }
    String ret="";
    if(forced){
      ret=lua.Lua_dostring(&tempScript);
      tempScript="";
    }else if(!manualOnly&&ts!=0) ret=lua.Lua_dostring(&script);
    if(ret.length()!=0) Serial.println(ret);
    ESP.wdtFeed();
    if(mustResend){
      ac.setPowerful(isPowerful());
      ac.setCurrentTime(hour*60+minute);
      ac.send();
      mustResend=false;
    }
    if(!forced) ts=millis();
  }
  ESP.wdtFeed();
  if(!wifiModeAP){
    if(wifiConnected){
      if(WiFi.status()!=WL_CONNECTED){
        wifiConnected=false;
        disconnectedSince=millis();
      }
    }else{
      if(WiFi.status()==WL_CONNECTED){
        wifiConnected=true;
      }
      if((millis()-lastNetReset)-(disconnectedSince-lastNetReset)>WIFI_TIMEOUT){
        wifiModeAP=true;
        ssid=default_ssid;
        //wifipsk=default_wifipsk; //disabled for security
        hostName=default_hostName;
        restartNetASAP=true;
      }
    }
  }
  yield(); //this also feeds the wdt, so no need to feed it again here
}
