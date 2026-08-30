#ifndef _IOTSARGBWSENSORMOD_H_
#define _IOTSARGBWSENSORMOD_H_
#include "iotsa.h"
#include "iotsaApi.h"

class IotsaRGBWSensorMod : public IotsaModule {
public:
  using IotsaModule::IotsaModule;
  void setup() override;
  void lateSetup() override;
  void loop() override;
  String info() override;
  void configLoad() override;
  void configSave() override;
protected:
  bool getHandler(const char *path, JsonObject& reply) override;
  bool putHandler(const char *path, const JsonVariant& request, JsonObject& reply) override;
  void webHandler() override;
  void _measure();
  void _setInterval();
  bool error;
  int raw_r;
  int raw_g;
  int raw_b;
  int raw_w;
  float r;
  float g;
  float b;
  float w;
  float cct;
  float lux;
  uint32_t integrationInterval = 320;
  uint32_t oldIntegrationInterval = 320;
  uint32_t nextReadingAvailable = 0;
};

#endif
