//
// RGBW colour / ambient-light sensor (VEML6040) exposed over WiFi (web + REST)
// and BLE. Battery-aware: the wake button on GPIO0 disables deep sleep and
// doubles as the config-mode tap gesture.
//

#include <Arduino.h>
#include "iotsa.h"
#include "iotsaWifi.h"
#include "iotsaOta.h"
#include "iotsaBattery.h"
#include "iotsaInput.h"
#include "iotsaBLEServer.h"
#include "iotsaRGBWSensorMod.h"

// Low signal on this pin disables deep sleep; it is also the config-mode
// tap button (see button0Pressed() below).
#define PIN_DISABLE_SLEEP 0

IotsaApplication application("Iotsa RGBWSensor Server");

IotsaWifiMod wifiMod(application);
IotsaOtaMod otaMod(application);
IotsaBatteryMod batteryMod(application);

Button buttonWake(PIN_DISABLE_SLEEP, true, false, true);
Input *inputs[] = {
  &buttonWake
};
IotsaInputMod inputMod(application, inputs, 1);

IotsaRGBWSensorMod rgbwMod(application);

IotsaBLEServerMod bleserverMod(application);

//
// Keep track of button presses, so we can switch mode or reboot with quick presses.
//
bool button0Pressed() {
  const int TAP_COUNT_MODE_CHANGE=3;
  const int TAP_COUNT_REBOOT=6;
  const uint32_t TAP_DURATION=1000;

  IFDEBUG IotsaSerial.println("button0 pressed");
  iotsaController.extendCurrentMode();
  static uint32_t lastButtonTapMillis = 0;
  static int buttonTapCount = 0;
  uint32_t now = millis();
  if (lastButtonTapMillis > 0 && now < lastButtonTapMillis + TAP_DURATION) {
    // A button change that was quick enough for a tap
    lastButtonTapMillis = now;
    buttonTapCount++;
    if (buttonTapCount == TAP_COUNT_MODE_CHANGE) {
      IFDEBUG IotsaSerial.println("tap mode change");
      iotsaController.allowRequestedConfigurationMode();
    }
    if (buttonTapCount == TAP_COUNT_REBOOT) {
      IFDEBUG IotsaSerial.println("tap mode reboot");
      iotsaController.requestReboot(1000);
    }
  } else {
    // Either the first change, or too late. Reset.
    lastButtonTapMillis = millis();
    buttonTapCount = 0;
  }
  return true;
}

void setup(void){
  application.setup();
  application.lateSetup();
  batteryMod.setPinDisableSleep(PIN_DISABLE_SLEEP);
  buttonWake.setCallback(button0Pressed);
}

void loop(void){
  application.loop();
}
