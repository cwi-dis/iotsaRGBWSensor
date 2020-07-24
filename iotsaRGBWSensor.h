#ifndef _IOTSARGBWSENSOR_H_
#define _IOTSARGBWSENSOR_H_
#include "iotsa.h"
#include "iotsaApi.h"

#ifdef IOTSA_WITH_API
#define IotsaRGBWSensorModBaseMod IotsaApiMod
#else
#define IotsaRGBWSensorModBaseMod IotsaMod
#endif

class IotsaRGBWSensorMod : public IotsaRGBWSensorModBaseMod {
public:
  using IotsaRGBWSensorModBaseMod::IotsaRGBWSensorModBaseMod;
  void setup();
  void serverSetup();
  void loop();
  String info();
protected:
  bool getHandler(const char *path, JsonObject& reply);
  void handler();
  void _measure();
  bool error;
  uint16_t r;
  uint16_t g;
  uint16_t b;
  uint16_t w;
  uint16_t cct;
  float lux;
};

#endif
