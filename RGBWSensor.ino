//
// Boilerplate for configurable web server (probably RESTful) running on ESP8266.
//
// The server always includes the Wifi configuration module. You can enable
// other modules with the preprocessor defines. With the default defines the server
// will allow serving of web pages and other documents, and of uploading those.
//

#include <Arduino.h>
#include "iotsa.h"
#include "iotsaWifi.h"

// CHANGE: Add application includes and declarations here

#undef WITH_USER   // Enable username/password authentication for changing configurations
#undef WITH_NTP    // Use network time protocol to synchronize the clock.
#define WITH_OTA    // Enable Over The Air updates from ArduinoIDE. Needs at least 1MB flash.
#undef WITH_FILES  // Enable static files webserver
#undef WITH_FILESUPLOAD  // Enable upload of static files for webserver
#undef WITH_FILESBACKUP  // Enable backup of all files including config files and webserver files
#define WITH_BATTERY // Enable power-saving support

IotsaApplication application("Iotsa RGBWSensor Server");
IotsaWifiMod wifiMod(application);

#define authProvider NULL

#ifdef WITH_OTA
#include "iotsaOta.h"
IotsaOtaMod otaMod(application, authProvider);
#endif

#ifdef WITH_BATTERY
#define PIN_DISABLE_SLEEP 0 // Define for pin on which low signal disables sleep
#include "iotsaBattery.h"
IotsaBatteryMod batteryMod(application, authProvider);
// Pressing the disable-sleep button will also wake from sleep or hibernation
#include "iotsaInput.h"
Button buttonWake(PIN_DISABLE_SLEEP, true, false, true);
Input *inputs[] = {
  &buttonWake
};
IotsaInputMod inputMod(application, inputs, 1);
#endif

#include "iotsaRGBWSensor.h"
IotsaRGBWSensorMod rgbwMod(application, authProvider);

//
// Keep track of button presses, so we can switch mode or reboot with quick presses.
//
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
    // A button change that was quick enough for a tap
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
    // Either the first change, or too late. Reset.
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

