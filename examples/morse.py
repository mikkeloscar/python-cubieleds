#!/usr/bin/env python

# LED example:
# 
# Translate string to morsecode and morse it using the LED.

import Cubie.leds as leds
import time

LEDS = ['blue', 'green']

class Morse:

    MORSE_TABLE = {
            "a": "*-",
            "b": "-***",
            "c": "-*-*",
            "d": "-**",
            "e": "*",
            "f": "**-*",
            "g": "--*",
            "h": "****",
            "i": "**",
            "j": "*---",
            "k": "-*-",
            "l": "*-**",
            "m": "--",
            "n": "-*",
            "o": "---",
            "p": "*--*",
            "q": "--*-",
            "r": "*-*",
            "s": "***",
            "t": "-",
            "u": "**-",
            "v": "***-",
            "w": "*--",
            "x": "-**-",
            "y": "-*--",
            "z": "--*",
            "1": "*----",
            "2": "**---",
            "3": "***--",
            "4": "****-",
            "5": "*****",
            "6": "-****",
            "7": "--***",
            "8": "---**",
            "9": "----*",
            "0": "-----",
            " ": "/",
            }

    def __init__(self, led):
        self.led = led
        leds.off(self.led)
        self.string = ""
        self.default = leds.status(self.led)

    def dash(self):
        leds.on(self.led)
        time.sleep(700.0 / 1000.0)
        leds.off(self.led)
        time.sleep(100.0 / 1000.0)

    def dot(self):
        leds.on(self.led)
        time.sleep(200.0 / 1000.0)
        leds.off(self.led)
        time.sleep(100.0 / 1000.0)

    def morse(self, string):
        for c in string:
            self.morse_char(c)
            self.string = self.string + "/"
        print "Morsed the string: %s - %s" % (self.string, string)

    def to_morse(self, char):
        c = char.lower()
        if c in Morse.MORSE_TABLE:
            return Morse.MORSE_TABLE[c]
        else:
            print "Char, %s is not morsable." % c
            return None

    def morse_char(self, char):
        leds.off(self.led)
        morse_char = self.to_morse(char)

        if morse_char:
            for c in morse_char:
                if c == "*":
                    self.dot()
                elif c == "-":
                    self.dash()
                elif c == "/":
                    c = ""
                    time.sleep(100.0 / 1000.0)
                self.string = self.string + c
            time.sleep(100.0 / 1000.0)

    def reset(self):
        if self.default:
            leds.on(self.led)
        else:
            leds.off(self.led)


if __name__ == "__main__":
    morse = Morse("green")
    morse.morse("SOS")
    morse.reset()
