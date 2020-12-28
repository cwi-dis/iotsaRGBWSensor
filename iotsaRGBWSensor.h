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
  bool putHandler(const char *path, const JsonVariant& request, JsonObject& reply);  void handler();
  void _measure();
  void _setInterval();
  bool error;
  float r;
  float g;
  float b;
  float w;
  float cct;
  float lux;
  uint32_t integrationInterval;
  uint32_t nextReadingAvailable;
};

#endif
