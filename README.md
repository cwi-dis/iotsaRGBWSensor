# iotsaRGBWSensor

Light sensor that detects RGB, White and ambient light using a VEML6040 color sensor.

Based on [iotsa](https://github.com/cwi-dis/iotsa).

The sensor uses a cheap AliExpress VEML6040 board (you may have to de-solder the white LED if
you want to take emitted/ambient light readings, as opposed to measuring the color of objects).

The VEML6040 is described in <https://www.vishay.com/docs/84276/veml6040.pdf> with additional
design information available at <https://www.vishay.com/docs/84331/designingveml6040.pdf>.

The interface library used is <https://github.com/thewknd/VEML6040>