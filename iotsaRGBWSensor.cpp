#include "iotsa.h"
#include "iotsaRGBWSensor.h"
#include "iotsaConfigFile.h"
#include "Wire.h"
#include "veml6040.h"

VEML6040 sensor;

#ifndef IOTSA_VEML_SDA
#define IOTSA_VEML_SDA 19
#endif
#ifndef IOTSA_VEML_SCL
#define IOTSA_VEML_SCL 23
#endif

void IotsaRGBWSensorMod::configLoad() {
  IotsaConfigFileLoad cf("/config/rgbwsensor.cfg");
  cf.get("integrationInterval", integrationInterval, 320);
  _setInterval();
}

void IotsaRGBWSensorMod::configSave() {
  IotsaConfigFileSave cf("/config/rgbwsensor.cfg");
  cf.put("integrationInterval", integrationInterval);
  _setInterval();
}

#ifdef IOTSA_WITH_WEB
void
IotsaRGBWSensorMod::handler() {
  iotsaConfig.extendCurrentMode();
  bool anyChanged = false;
  if( server->hasArg("integrationInterval")) {
    if (needsAuthentication()) return;
    String sInterval = server->arg("integrationInterval");
    integrationInterval = sInterval.toInt();
    anyChanged = true;
  }  
  if (anyChanged) configSave();
  String message = "<html><head><title>RGBW sensor module</title></head><body><h1>RGBW sensor module</h1>";
  _measure();
  if (error) {
    message += "<p><em>Error: VEML6040 sensor not detected</em></p>";
  }
  message += "<h2>Sensor Configuration</h2>";
  message += "<form method='get'>Integration interval (ms): <input name='integrationInterval' value='";
  message += String(integrationInterval);
  message += "'><input type='submit' value='Configure'></form>";
  message += "<h2>Sensor Reading</h2>";
  message += "<form method='get'><input type='submit' value='Refresh'></form>";

  if (r > 0.99 || g > 0.99 || b > 0.99) {
    message += "<p><em>Note:</em> Color channel over-exposed, decrease interval for better color results.</p>";
  } else
  if (r < 0.33 && g < 0.33 && b < 0.33) {
    message += "<p><em>Note:</em> Increasing interval may give better color results.</p>";
  } else
  if (w > 0.99) {
    message += "<p><em>Note:</em> W channel over-exposed. May mean lots of IR light.</p>";
  }

  message += "<p>RGBW counts: R=" + String(raw_r) + ", G=" + String(raw_g) + ", B=" + String(raw_b) + ", W=" + String(raw_w) + "<br>";
  message += "RGB intensities: R=" + String(r) + ", G=" + String(g) + ", B=" + String(b) + "<br>";
  uint32_t hexColor = ((int)(r*255) << 16) | ((int)(g*255) << 8) | ((int)(b*255));
  
  message += "Color 0x" + String(hexColor, 16)+ ": <svg width='40' height='40'><rect width='40' height='40' style='fill:#" + String(hexColor, HEX) + ";stroke-width:2;stroke:rgb(0,0,0)' /></svg>";
  float maxRGB = max(r, max(g, b));
  if (maxRGB == 0) maxRGB = 1;
  hexColor = ((int)(r/maxRGB*255) << 16) | ((int)(g/maxRGB*255) << 8) | ((int)(b/maxRGB*255));
  message += "Hue 0x" + String(hexColor, 16) + ": <svg width='40' height='40'><rect width='40' height='40' style='fill:#" + String(hexColor, HEX) + ";stroke-width:2;stroke:rgb(0,0,0)' /></svg><br>";

  message += "White intensity: W=" + String(w) + "<br>";
  message += "Color temperature: CCT=" + String(cct) + "<br>";
  message += "Ambient light level: " + String(lux) + "lux<br>";
  message += "(Integration interval: "+ String(integrationInterval) + "ms)";
  server->send(200, "text/html", message);
}

String IotsaRGBWSensorMod::info() {
  iotsaConfig.extendCurrentMode();
  String message = "<p>Built with RGBW sensor module. See <a href=\"/rgbw\">/rgbw</a>, or <a href=\"/api/rgbw\">/api/rgbw</a> for REST API.</p>";
  return message;
}
#endif // IOTSA_WITH_WEB

void IotsaRGBWSensorMod::setup() {
  Wire.begin(IOTSA_VEML_SDA, IOTSA_VEML_SCL);
  if(sensor.begin()) {
    configLoad();
    error = false;
    IFDEBUG IotsaSerial.println("VEML6040 sensor attached");
  } else {
    error = true;
    IFDEBUG IotsaSerial.println("VEML6040 sensor not found");
  }
}

void IotsaRGBWSensorMod::_setInterval() {
  uint8_t conf;
  if (integrationInterval <= 40) {
    integrationInterval = 40;
    conf = VEML6040_IT_40MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE;
  } else
  if (integrationInterval <= 80) {
    integrationInterval = 80;
    conf = VEML6040_IT_80MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE;
  } else
  if (integrationInterval <= 160) {
    integrationInterval = 160;
    conf = VEML6040_IT_160MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE;
  } else
  if (integrationInterval <= 320) {
    integrationInterval = 320;
    conf = VEML6040_IT_320MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE;
  } else
  if (integrationInterval <= 640) {
    integrationInterval = 640;
    conf = VEML6040_IT_640MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE;
  } else {
    integrationInterval = 1280;
    conf = VEML6040_IT_1280MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE;
  }
  sensor.setConfiguration(conf);
  nextReadingAvailable = millis() + oldIntegrationInterval + integrationInterval + 1;
  oldIntegrationInterval = integrationInterval;
}

#ifdef IOTSA_WITH_API
bool IotsaRGBWSensorMod::getHandler(const char *path, JsonObject& reply) {
  iotsaConfig.extendCurrentMode();
  _measure();
  if (error) {
    reply["error"] = "no sensor";
    return true;
  }
  reply["raw_r"] = raw_r;
  reply["raw_g"] = raw_g;
  reply["raw_b"] = raw_b;
  reply["raw_w"] = raw_w;
  reply["r"] = r;
  reply["g"] = g;
  reply["b"] = b;
  reply["w"] = w;
  reply["cct"] = cct;
  reply["lux"] = lux;
  reply["integrationInterval"] = integrationInterval;
  return true;
}

bool IotsaRGBWSensorMod::putHandler(const char *path, const JsonVariant& request, JsonObject& reply) {
  iotsaConfig.extendCurrentMode();
  bool anyChanged = false;
  JsonObject reqObj = request.as<JsonObject>();
  if (reqObj.containsKey("integrationInterval")) {
    integrationInterval = reqObj["integrationInterval"];
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

void IotsaRGBWSensorMod::_measure() {
  if (error) return;
  uint32_t now = millis();
  if (now < nextReadingAvailable) delay(nextReadingAvailable-now);
  raw_r = sensor.getRed();
  raw_g = sensor.getGreen();
  raw_b = sensor.getBlue();
  raw_w = sensor.getWhite();
  r = (float)raw_r/raw_w;
  g = (float)raw_g/raw_w;
  b = (float)raw_b/raw_w;
  w = (float)raw_w/65536.0;
  cct = (float)sensor.getCCT();
  lux = sensor.getAmbientLight();
  nextReadingAvailable = millis() + integrationInterval;
  IFDEBUG IotsaSerial.printf("raw_r=%d raw_g=%d raw_b=%d raw_w=%d r=%f g=%f b=%f w=%f cct=%f lux=%f\n", raw_r, raw_g, raw_b, raw_w, r, g, b, w, cct, lux);
}

void IotsaRGBWSensorMod::loop() {
}
