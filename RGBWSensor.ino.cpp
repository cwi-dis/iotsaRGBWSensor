# 1 "/var/folders/8p/xg7xpzbx2tnf5xfzzvl925lr0000gn/T/tmpxdhbtaa6"
#include <Arduino.h>
# 1 "/Users/jack/src/dis-git/iotsaRGBWSensor/RGBWSensor.ino"
# 9 "/Users/jack/src/dis-git/iotsaRGBWSensor/RGBWSensor.ino"
#include <Arduino.h>
#include "iotsa.h"
#include "iotsaWifi.h"



#undef WITH_USER
#undef WITH_NTP
#define WITH_OTA 
#undef WITH_FILES
#undef WITH_FILESUPLOAD
#undef WITH_FILESBACKUP
#define WITH_BATTERY 

IotsaApplication application("Iotsa RGBWSensor Server");
IotsaWifiMod wifiMod(application);

#define authProvider NULL

#ifdef WITH_OTA
#include "iotsaOta.h"
IotsaOtaMod otaMod(application, authProvider);
#endif

#ifdef WITH_BATTERY
#define PIN_DISABLE_SLEEP 0
#include "iotsaBattery.h"
IotsaBatteryMod batteryMod(application, authProvider);

#include "iotsaInput.h"
Button buttonWake(PIN_DISABLE_SLEEP, true, false, true);
Input *inputs[] = {
  &buttonWake
};
IotsaInputMod inputMod(application, inputs, 1);
#endif

#include "iotsaRGBWSensorMod.h"
IotsaRGBWSensorMod rgbwMod(application, authProvider);

#include "iotsaBLEServer.h"
#ifdef IOTSA_WITH_BLE
IotsaBLEServerMod bleserverMod(application);
#endif
bool button0Pressed();
void setup(void);
void loop(void);
#line 56 "/Users/jack/src/dis-git/iotsaRGBWSensor/RGBWSensor.ino"
bool button0Pressed() {
  const int TAP_COUNT_MODE_CHANGE=3;
  const int TAP_COUNT_REBOOT=6;
  const uint32_t TAP_DURATION=1000;

  IFDEBUG IotsaSerial.println("button0 pressed");
  iotsaConfig.extendCurrentMode();
  static uint32_t lastButtonTapMillis = 0;
  static int buttonTapCount = 0;
  uint32_t now = millis();
  if (lastButtonTapMillis > 0 && now < lastButtonTapMillis + TAP_DURATION) {

    lastButtonTapMillis = now;
    buttonTapCount++;
    if (buttonTapCount == TAP_COUNT_MODE_CHANGE) {
      IFDEBUG IotsaSerial.println("tap mode change");
      iotsaConfig.allowRequestedConfigurationMode();
    }
    if (buttonTapCount == TAP_COUNT_REBOOT) {
      IFDEBUG IotsaSerial.println("tap mode reboot");
      iotsaConfig.requestReboot(1000);
    }
  } else {

    lastButtonTapMillis = millis();
    buttonTapCount = 0;
  }
  return true;
}

void setup(void){
  application.setup();
  application.serverSetup();
#ifdef PIN_DISABLE_SLEEP
  batteryMod.setPinDisableSleep(PIN_DISABLE_SLEEP);
#endif
buttonWake.setCallback(button0Pressed);
}

void loop(void){
  application.loop();
}