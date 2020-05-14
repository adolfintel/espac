#include<ESPAsyncWebServer.h>

AsyncWebServer ws(80); //web server

void initWebUI(){
  ws.serveStatic("/",SPIFFS,"/webui").setDefaultFile("index.html");
  ws.on("/getScript",HTTP_GET,[](AsyncWebServerRequest *request) {
      AsyncWebServerResponse *response;
      if(SPIFFS.exists(SCRIPT_FILENAME)){
        response=request->beginResponse(SPIFFS,SCRIPT_FILENAME,"text/plain");
      }else{
        response=request->beginResponseStream("text/plain");
      }
      request->send(response);
  });
  ws.on("/newScript",HTTP_GET,[](AsyncWebServerRequest *request){
    SPIFFS.open(NEWSCRIPT_FILENAME,"w").close();
    request->send(200,"text/plain","1");
  });
  ws.on("/appendScript",HTTP_POST,[](AsyncWebServerRequest *request) {
    if(request->hasParam("script",true)){
      File s=SPIFFS.open(NEWSCRIPT_FILENAME,"a");
      s.print(request->getParam("script",true)->value());
      s.flush();
      s.close();
    }
    request->send(200,"text/plain","1");
  });
  ws.on("/commitScript",HTTP_GET,[](AsyncWebServerRequest *request){
    if(SPIFFS.exists(NEWSCRIPT_FILENAME)){
      if(SPIFFS.exists(SCRIPT_FILENAME)) SPIFFS.remove(SCRIPT_FILENAME);
      SPIFFS.rename(NEWSCRIPT_FILENAME,SCRIPT_FILENAME);
      reloadScriptASAP=true; //because of an SPIFFS bug, we can't reload the script immediately here, we'll do it in the next loop
    }
    request->send(200,"text/plain","1");
  });
  ws.on("/status",HTTP_GET,[](AsyncWebServerRequest *request) {
    AsyncResponseStream *response=request->beginResponseStream("text/plain");
    response->printf("%s\n%f\n%f\n%f\n%f\n",manualOnly?"m":"a",temperature,humidity,pressure,apparentTemperature);
    request->send(response);
  });
  ws.on("/remote",HTTP_GET,[](AsyncWebServerRequest *request) {
    AsyncResponseStream *response=request->beginResponseStream("text/plain");
    response->printf("%s\n",ac.toString().c_str());
    request->send(response);
  });
  ws.on("/run",HTTP_POST,[](AsyncWebServerRequest *request) {
    if(request->hasParam("script",true)){
      if(tempScript.length()==0){
        tempScript=request->getParam("script",true)->value();
        request->send(200,"text/plain","1");
      }else{
        request->send(200,"text/plain","0");
      }
    } else request->send(200,"text/plain","1");
  });
  ws.on("/manual",HTTP_GET,[](AsyncWebServerRequest *request) {
    manualOnly=true;
    File s=SPIFFS.open(MODE_FILENAME,"w");
    s.println("m");
    s.flush();
    s.close();
    request->send(200,"text/plain","1");
  });
  ws.on("/auto",HTTP_GET,[](AsyncWebServerRequest *request) {
    manualOnly=false;
    acReset();
    File s=SPIFFS.open(MODE_FILENAME,"w");
    s.println("a");
    s.flush();
    s.close();
    request->send(200,"text/plain","1");
  });
  ws.on("/setRTC",HTTP_GET,[](AsyncWebServerRequest *request) {
    rtc.setClockMode(false);
    if(request->hasParam("year")) rtc.setYear(atoi(request->getParam("year")->value().c_str()));
    if(request->hasParam("month")) rtc.setMonth(atoi(request->getParam("month")->value().c_str()));
    if(request->hasParam("day")) rtc.setDate(atoi(request->getParam("day")->value().c_str()));
    if(request->hasParam("hh")) rtc.setHour(atoi(request->getParam("hh")->value().c_str()));
    if(request->hasParam("mm")) rtc.setMinute(atoi(request->getParam("mm")->value().c_str()));
    if(request->hasParam("ss")) rtc.setSecond(atoi(request->getParam("ss")->value().c_str()));
    if(request->hasParam("dow")) rtc.setDoW(atoi(request->getParam("dow")->value().c_str()));
    request->send(200,"text/plain","1");
  });
  ws.on("/net",HTTP_GET,[](AsyncWebServerRequest *request) {
    if(SPIFFS.exists(WIFI_FILENAME)){
      AsyncWebServerResponse *response=request->beginResponse(SPIFFS,WIFI_FILENAME,"text/plain");
      request->send(response);
    }else{
      AsyncResponseStream *response=request->beginResponseStream("text/plain");
      response->printf("%s\n%s\n%s\n%s\n",wifiModeAP?"a":"c",ssid.c_str(),wifipsk.c_str(),hostName.c_str());
    }
  });    
  ws.on("/setNet",HTTP_GET,[](AsyncWebServerRequest *request) {
    if(request->hasParam("wmode")){
      String m=request->getParam("wmode")->value().c_str(); m.trim();
      wifiModeAP=m.compareTo("c")!=0;
    }
    if(request->hasParam("ssid")){ssid=request->getParam("ssid")->value().c_str(); ssid.trim();}
    if(request->hasParam("psk")){wifipsk=request->getParam("psk")->value().c_str(); wifipsk.trim();}
    if(request->hasParam("host")){hostName=request->getParam("host")->value().c_str(); hostName.trim();}
    restartNetASAP=true;
    File f=SPIFFS.open(WIFI_FILENAME,"w");
    f.print(wifiModeAP?"a":"c"); f.print("\n");
    f.print(ssid); f.print("\n");
    f.print(wifipsk); f.print("\n");
    f.print(hostName); f.print("\n");
    f.flush();
    f.close();
    request->send(200,"text/plain","1");
  });
  ws.begin();  
}
