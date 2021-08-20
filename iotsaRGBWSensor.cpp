#include "iotsa.h"
#include "iotsaRGBWSensor.h"
#include "iotsaConfigFile.h"
#include "Wire.h"
#include "veml6040.h"

VEML6040 sensor;
#ifdef IOTSA_WITH_WEB
void
IotsaRGBWSensorMod::handler() {
  iotsaConfig.extendCurrentMode();
  String message = "<html><head><title>RGBW sensor module</title></head><body><h1>RGBW sensor module</h1>";
  _measure();
  if (error) {
    message += "<p><em>Error: VEML6040 sensor not detected</em></p>";
  }
  message += "<p>RGB intensities: R=" + String(r) + ", G=" + String(g) + ", B=" + String(b) + "<br>";
  uint32_t hexColor = ((int)(r*255) << 16) | ((int)(g*255) << 8) | ((int)(b*255));
  message += "Color: <svg width='40' height='40'><rect width='40' height='40' style='fill:#" + String(hexColor, HEX) + ";stroke-width:2;stroke:rgb(0,0,0)' /></svg>";
  float maxRGB = max(r, max(g, b));
  if (maxRGB == 0) maxRGB = 1;
  hexColor = ((int)(r/maxRGB*255) << 16) | ((int)(g/maxRGB*255) << 8) | ((int)(b/maxRGB*255));
  message += "Hue: <svg width='40' height='40'><rect width='40' height='40' style='fill:#" + String(hexColor, HEX) + ";stroke-width:2;stroke:rgb(0,0,0)' /></svg><br>";

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
    integrationInterval = 320;
    _setInterval();
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
  nextReadingAvailable = millis() + integrationInterval;
}

#ifdef IOTSA_WITH_API
bool IotsaRGBWSensorMod::getHandler(const char *path, JsonObject& reply) {
  iotsaConfig.extendCurrentMode();
  _measure();
  if (error) {
    reply["error"] = "no sensor";
    return true;
  }
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
    _setInterval();
  }
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
  r = (float)sensor.getRed()/65535.0;
  g = (float)sensor.getGreen()/65535.0;
  b = (float)sensor.getBlue()/65535.0;
  w = (float)sensor.getWhite()/65535.0;
  cct = (float)sensor.getCCT();
  lux = sensor.getAmbientLight();
  nextReadingAvailable = millis() + integrationInterval;
  IFDEBUG IotsaSerial.printf("r=%f g=%f b=%f w=%f cct=%f lux=%f\n", r, g, b, w, cct, lux);
}

void IotsaRGBWSensorMod::loop() {
}
