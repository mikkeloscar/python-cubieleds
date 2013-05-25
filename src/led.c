/**
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) Mikkel Oscar Lyderik, 2013
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>

#include "leds.h"

int main()
{
    struct leds_t blue, green;

    if (leds_init(&blue, "blue") < 0)
        return 1;
    if (leds_init(&green, "green") < 0)
        return 1;

    
    for (int i=0; i < 20; i++) {
        if (i % 2 == 0) {
            leds_on(&blue);
            leds_off(&green);
        } else {
            leds_off(&blue);
            leds_on(&green);
        }
        usleep(1000*100);
    }

    leds_off(&blue);
    leds_off(&green);

    leds_trigger(&blue, "usb-online");

    return 0;
}
