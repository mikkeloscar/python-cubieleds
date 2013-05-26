#!/usr/bin/env python

# LED example:
# 
# Monitor journald for ssh connects and disconnects.
# If any ssh connections are active turn on led, else turn it off.

import sys
import signal, time
import subprocess
import re
import Cubie.leds as leds

LEDS = ['blue', 'green']

class SSHMonitor:

    REGEX_OPEN = "sshd(.*)session opened"
    REGEX_CLOSE = "sshd(.*)session closed"

    def __init__(self, led):
        self.led = led
        # save default led state
        self.default = leds.status(self.led)
        leds.off(self.led)
        self.opened = re.compile(SSHMonitor.REGEX_OPEN)
        self.closed = re.compile(SSHMonitor.REGEX_CLOSE)
        
        if self.num_conn():
            leds.on(self.led)

    def run(self):
        p = subprocess.Popen('journalctl -fn0', shell=True,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT) 
        while True:
            line = p.stdout.readline().rstrip('\n')
            if self.opened.search(line):
                leds.on(self.led)
            elif self.closed.search(line):
                if self.num_conn() == 0:
                    leds.off(self.led)

    def num_conn(self):
        p = subprocess.Popen('netstat -tn | grep ":2020" | wc -l', shell=True,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT)
        i = int(p.stdout.readline().rstrip('\n'))
        return i

    def reset(self):
        if self.default:
            leds.on(self.led)
        else:
            leds.off(self.led)


def main():
    if len(sys.argv) > 1:
        color = sys.argv[1]
        if color in LEDS:
            monitor = SSHMonitor(color)
            monitor.run()
        else:
            print "Please provide the LED color as first argument"


if __name__ == "__main__":
    main()
