# Cubieboard LED python module

Control the green and blue LED on the Cubieboard with python.
Tested with python 2.7+ and 3+

### Enable LED support on the Cubieboard

In order to use this module you need a Cubieboard (obviously), running some
sort of linux. I have tested on Archlinux arm, but anything that run on the
Cubieboard should work.

Then you need to activate the LEDs as [described here](http://linux-sunxi.org/Cubieboard/Programming/StatusLEDs).
I also had to load the module:

    leds_sunxi

### Installation

You can install using pip

    $ pip install cubieleds

If you are running Archlinux, PKGBUILDs for python2 and 3 are available
[here](https://github.com/mikkeloscar/packages).

### Usage

The module is pretty simpel. This is how it works:

```python
>>> import cubie.leds

# turn on the blue LED
>>> cubie.leds.on("blue")
True
# check status of blue LED
>>> cubie.leds.status("blue")
1
# Turn off blue LED
>>> cubie.leds.off("blue")
True
# check status again
>>> cubie.leds.status("blue")
0

# It is also possible to add a trigger, eg. the mmc0 trigger which will make
# the LED blink on SD card activity
>>> cubie.leds.trigger("green", "mmc0")
True
>>> cubie.leds.trigger_status("green")
'mmc0'
```
