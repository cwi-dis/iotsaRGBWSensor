#include "iotsa.h"
#include "iotsaRGBWSensor.h"
#include "iotsaConfigFile.h"

#ifdef IOTSA_WITH_WEB
void
IotsaRGBWSensorMod::handler() {
  bool anyChanged = false;
  if( server->hasArg("argument")) {
    if (needsAuthentication()) return;
    argument = server->arg("argument");
    anyChanged = true;
  }
  if (anyChanged) configSave();

  String message = "<html><head><title>Boilerplate module</title></head><body><h1>Boilerplate module</h1>";
  message += "<form method='get'>Argument: <input name='argument' value='";
  message += htmlEncode(argument);
  message += "'><br><input type='submit'></form>";
  server->send(200, "text/html", message);
}

String IotsaRGBWSensorMod::info() {
  String message = "<p>Built with RGBW sensor module. See <a href=\"/rgbw\">/rgbw</a>, or <a href=\"/api/rgbw\">/api/rgbw</a> for REST API.</p>";
  return message;
}
#endif // IOTSA_WITH_WEB

void IotsaRGBWSensorMod::setup() {
  configLoad();
}

#ifdef IOTSA_WITH_API
bool IotsaRGBWSensorMod::getHandler(const char *path, JsonObject& reply) {
  reply["argument"] = argument;
  return true;
}

bool IotsaRGBWSensorMod::putHandler(const char *path, const JsonVariant& request, JsonObject& reply) {
  bool anyChanged = false;
  JsonObject reqObj = request.as<JsonObject>();
  if (reqObj.containsKey("argument")) {
    argument = reqObj["argument"].as<String>();
    anyChanged = true;
  }
  if (anyChanged) configSave();
  return anyChanged;
}
#endif // IOTSA_WITH_API

void IotsaRGBWSensorMod::serverSetup() {
#ifdef IOTSA_WITH_WEB
  server->on("/rgbw", std::bind(&IotsaRGBWSensorMod::handler, this));
#endif
#ifdef IOTSA_WITH_API
  api.setup("/api/rgbw", true, true);
  name = "rgbw";
#endif
}

void IotsaRGBWSensorMod::configLoad() {
  IotsaConfigFileLoad cf("/config/rgbw.cfg");
  cf.get("argument", argument, "");
 
}

void IotsaRGBWSensorMod::configSave() {
  IotsaConfigFileSave cf("/config/rgbw.cfg");
  cf.put("argument", argument);
}

void IotsaRGBWSensorMod::loop() {
}
