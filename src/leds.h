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

#ifndef LEDS_H
#define LEDS_H

#include <limits.h>

#define LEDS_ROOT "/sys/class/leds"
#define LEDS_TRIGGERS 11

typedef char filepath_t[PATH_MAX];

struct leds_t {
    const char *triggers[LEDS_TRIGGERS];
    filepath_t b_dev;
    filepath_t t_dev;
};

int leds_init(struct leds_t *l, const char *color);
int leds_on(struct leds_t *l);
int leds_off(struct leds_t *l);
int leds_status(struct leds_t *l);
int leds_trigger(struct leds_t *l, const char *trigger);
int leds_trigger_status(struct leds_t *l);

#endif
