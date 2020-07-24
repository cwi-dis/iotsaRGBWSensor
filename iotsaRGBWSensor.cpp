#include "iotsa.h"
#include "iotsaRGBWSensor.h"
#include "iotsaConfigFile.h"
#include "Wire.h"
#include "veml6040.h"

VEML6040 sensor;
#ifdef IOTSA_WITH_WEB
void
IotsaRGBWSensorMod::handler() {
  String message = "<html><head><title>RGBW sensor module</title></head><body><h1>RGBW sensor module</h1>";
  _measure();
  if (error) {
    message += "<p><em>Error: VEML6040 sensor not detected</em></p>";
  }
  message += "<p>RGB intensities: R=" + String(r) + ", G=" + String(g) + ", B=" + String(b) + "<br>";
  message += "White intensity: W=" + String(w) + "<br>";
  message += "Color temperature: CCT=" + String(cct) + "<br>";
  message += "Ambient light level: " + String(lux) + "lux</p>";
  server->send(200, "text/html", message);
}

String IotsaRGBWSensorMod::info() {
  String message = "<p>Built with RGBW sensor module. See <a href=\"/rgbw\">/rgbw</a>, or <a href=\"/api/rgbw\">/api/rgbw</a> for REST API.</p>";
  return message;
}
#endif // IOTSA_WITH_WEB

void IotsaRGBWSensorMod::setup() {
  Wire.begin();
  if(sensor.begin()) {
    sensor.setConfiguration(VEML6040_IT_320MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE);
    error = false;
  } else {
    error = true;
  }
}

#ifdef IOTSA_WITH_API
bool IotsaRGBWSensorMod::getHandler(const char *path, JsonObject& reply) {
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
  return true;
}
#endif // IOTSA_WITH_API

void IotsaRGBWSensorMod::serverSetup() {
#ifdef IOTSA_WITH_WEB
  server->on("/rgbw", std::bind(&IotsaRGBWSensorMod::handler, this));
#endif
#ifdef IOTSA_WITH_API
  api.setup("/api/rgbw", true, false);
  name = "rgbw";
#endif
}

void IotsaRGBWSensorMod::_measure() {
  if (error) return;
  r = sensor.getRed();
  g = sensor.getGreen();
  b = sensor.getBlue();
  w = sensor.getWhite();
  cct = sensor.getCCT();
  lux = sensor.getAmbientLight();
  IFDEBUG IotsaSerial.printf("r=%d g=%d b=%d w=%d cct=%d lux=%f\n", r, g, b, w, cct, lux);
}

void IotsaRGBWSensorMod::loop() {
}
