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

### Usage

The module is pretty simpel. This is how it works:

```python
>>> import Cubie.leds

# turn on the blue LED
>>> Cubie.leds.on("blue")
True
# check status of blue LED
>>> Cubie.leds.status("blue")
1
# Turn off blue LED
>>> Cubie.leds.off("blue")
True
# check status again
>>> Cubie.leds.status("blue")
0

# It is also possible to add a trigger, eg. the mmc0 trigger which will make
# the LED blink on SD card activity
>>> Cubie.leds.trigger("green", "mmc0")
True
>>> Cubie.leds.trigger_status("green")
'mmc0'
```
