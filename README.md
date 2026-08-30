# iotsaRGBWSensor

![build-platformio](https://github.com/cwi-dis/iotsaRGBWSensor/workflows/build-platformio/badge.svg)
![build-arduino](https://github.com/cwi-dis/iotsaRGBWSensor/workflows/build-arduino/badge.svg)

Light sensor that detects RGB, White and ambient light using a VEML6040 color sensor.

Based on [iotsa](https://github.com/cwi-dis/iotsa).

The sensor uses a cheap AliExpress VEML6040 board (you may have to de-solder the white LED if
you want to take emitted/ambient light readings, as opposed to measuring the color of objects).

The VEML6040 is described in <https://www.vishay.com/docs/84276/veml6040.pdf> with additional
design information available at <https://www.vishay.com/docs/84331/designingveml6040.pdf>.

The interface library used is <https://github.com/thewknd/VEML6040>

## Hardware construction

Any esp32 board will do. I have used a lolin 32 lite, because it has a LiPo charger on the board. Here is the breadboard layout:

![breadboard layout](extras/breadboard.jpg)

Connections to be made:

- GND to VEML GND, pushbutton
- 3V to VEML VCC
- GPIO19 to VEML SDA
- GPIO23 to VEML SCL
- GPIO0 to pushbutton

After the breadboard work I've transferred the components to a solderboard. It's not a work of art, I wanted the sensor on the bottom of the board for easier measurement, but that led to routing problems. But it works:

![solderboard top](extras/top.jpg)

![solderboard bottom](extras/bottom.jpg)

See the note above about desoldering the bright white LED on the VEML.

### 3d-printed case

Here are pictures of a 3d-printed case for the board, plus a LiPo battery:

![case top](extras/case_top.jpg)

![case bottom](extras/case_bottom.jpg)

The `extras` folder contains the `.stl` files to print the box and the lid, and it also has the `colorimeter.f3d` Autodesk Fusion file in case you want to make modifications.


## Software installation

- Build using PlatformIO (`pio run`), or the Arduino IDE / `arduino-cli`.
  - The `build_opt.h` file passes `-DIOTSA_WITH_BLE` to the Arduino build (PlatformIO
    gets it from `platformio.ini`). Don't remove it.
  - In the Arduino IDE, set **Tools → Partition Scheme → Minimal SPIFFS** — the
    BLE firmware does not fit the default partition.
- Flash to the board over USB.
- Board will create a WiFi hotspot `config-xxxxxxx`.
- Connect to that WiFi, browse to <http://192.168.4.1>, configure hostname (lets say `colorsensor` for this example), WiFi SSID and Password. You may have to reset the board to switch to configuration mode, it will tell you.
- Reset the board, it will connect to your wifi with hostname `colorsensor.local`.
- Browse to battery config. Set sleep mode to _Deep Sleep_, wake time to _60000_.

## Usage

Boot the board. If you have set the battery config above the board will go to deep sleep after 60 seconds of inactivity (saving battery). Pressing the button will wake it again, or keep it awake for another 60 seconds. Accessing the web or REST interface will also keep it awake for another 60 seconds.

Point the sensor at whatever light (or lit surface) you want to measure.

Read the results with a browser to <http://colorsensor.local/rgbw> in human-readable form, or <http://colorsensor.local/api/rgbw> as JSON (REST interface).

You can reflash the board over the air, see [iotsa](https://github.com/cwi-dis/iotsa) for details. Tapping the button 3 times will enable OTA mode or config mode (after being requested over the net), tapping it 6 times will force a reboot.

## Python tool

The `extras/python/` directory contains the `colorMeter` Python tool for retrieving, storing, and graphing readings from the device.

### Setup

(If instructions in this repo's parent directory cover venv setup, those take precedence over the steps below.)

From the repo root:

```sh
python3 -m venv .venv
source .venv/bin/activate
pip install -e ../iotsa/extras/python/
pip install -e extras/python/
```

Or, if you have the `requirements_dev.txt`:

```sh
python3 -m venv .venv
source .venv/bin/activate
pip install -r extras/python/requirements_dev.txt
pip install -e extras/python/
```

### Usage

Retrieve current data from a device and print it:

```sh
colorMeter -d colorsensor.local
```

Save readings to a CSV file (repeat to append more readings over time):

```sh
colorMeter -d colorsensor.local -o readings.csv
```

Read back a CSV file and plot the CCT (color temperature) reading on the CIE 1931 chromaticity diagram:

```sh
colorMeter -i readings.csv -g
```

This device is also used by [lissabon](https://github.com/cwi-dis/lissabon)'s `lissabonCalibrate` package to calibrate RGBW ledstrips (it talks to the device's `rgbw` API directly rather than through this tool). `colorMeter` is deliberately kept simpler and free of any lissabon-specific logic, so it stays useful as a general-purpose way to read this sensor — that was already the intent when it was split off in 2021.