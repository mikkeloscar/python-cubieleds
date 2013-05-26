/**
 * Copyright (C) 2013  Mikkel Oscar Lyderik
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>

#include "leds.h"

static int get(filepath_t path, long *value)
{
    int fd =  open(path, O_RDONLY);
    if (fd < 0) {
        warn("failed to open to read from LED %s", path);
        return -1;
    }

    char buf[1024], *end = NULL;
    if (read(fd, buf, 1024) < 0)
        err(EXIT_FAILURE, "failed to read %s", path);

    errno = 0;
    *value = strtol(buf, &end, 10);
    if (errno || buf == end) {
        warn("not a number: %s", buf);
        return -1;
    }

    close(fd);
    return 0;
}

static int set(filepath_t path, long value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        warn("failed to open to write to LED %s", path);
        return -1;
    }

    char buf[1024];
    int len = snprintf(buf, 1024, "%ld", value);
    if (write(fd, buf, len) < 0)
        err(EXIT_FAILURE, "failed to set LED");

    close(fd);
    return 0;
}

static int set_trigger(filepath_t path, const char *value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        warn("failed to open to write to LED %s", path);
        return -1;
    }

    if (write(fd, value, strlen(value)) < 0)
        err(EXIT_FAILURE, "failed to set LED");

    close(fd);
    return 0;
}

static int get_trigger(filepath_t path, char *value)
{
    int fd =  open(path, O_RDONLY);
    if (fd < 0) {
        warn("failed to open to read from LED %s", path);
        return -1;
    }

    char buf[1024];
    if (read(fd, buf, 1024) < 0)
        err(EXIT_FAILURE, "failed to read %s", path);

    strncpy(value, buf, 1024);

    close(fd);
    return 0;
}

static int in_trigger_array(const char *elem, const char **array)
{
    int i;

    for (i = 0; i < LEDS_TRIGGERS; i++) {
        if (strcmp(elem, array[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

static int led_init(struct leds_t *l, const char *color)
{
    char *device;
    struct led_t *led;
    if (strcmp(color, "blue") == 0) {
        device = "blue:ph21:led2";
        led = &(l->blue);
    } else if (strcmp(color, "green") == 0) {
        device = "green:ph20:led1";
        led = &(l->green);
    } else {
        return -1;
    }

    l->triggers[0] = "none";
    l->triggers[1] = "battery-charging-or-full";
    l->triggers[2] = "battery-charging";
    l->triggers[3] = "battery-full";
    l->triggers[4] = "battery-charging-blink-full-solid";
    l->triggers[5] = "ac-online";
    l->triggers[6] = "usb-online";
    l->triggers[7] = "mmc0";
    l->triggers[8] = "mmc1";
    l->triggers[9] = "mmc2";
    l->triggers[10] = "rfkill0";

    snprintf(led->b_dev, PATH_MAX, LEDS_ROOT "/%s/brightness", device);
    snprintf(led->t_dev, PATH_MAX, LEDS_ROOT "/%s/trigger", device);
    return 0;
}

int leds_init(struct leds_t *l)
{
    if (led_init(l, "blue") < 0) {
        return -1;
    }
    if (led_init(l, "green") < 0) {
        return -1;
    }
    return 0;
}

int leds_status(struct leds_t *l, const char *color)
{
    struct led_t *led;
    if (strcmp(color, "blue") == 0) {
        led = &(l->blue);
    } else if (strcmp(color, "green") == 0) {
        led = &(l->green);
    } else {
        return -1;
    }

    long value = 0;
    int res = 0;
    int rc = get(led->b_dev, &value);
    if (value > 0) res = 1; 
    return rc ? rc : res;
}

int leds_on(struct leds_t *l, const char *color)
{
    struct led_t *led;
    if (strcmp(color, "blue") == 0) {
        led = &(l->blue);
    } else if (strcmp(color, "green") == 0) {
        led = &(l->green);
    } else {
        return -1;
    }

    if (leds_status(l, color)) {
        return 1;
    }

    return set(led->b_dev, 1);
}

int leds_off(struct leds_t *l, const char *color)
{
    struct led_t *led;
    if (strcmp(color, "blue") == 0) {
        led = &(l->blue);
    } else if (strcmp(color, "green") == 0) {
        led = &(l->green);
    } else {
        return -1;
    }

    if (!leds_status(l, color)) {
        return 0;
    }

    return set(led->b_dev, 0);
}

int leds_trigger(struct leds_t *l, const char *color, const char *trigger)
{
    struct led_t *led;
    if (strcmp(color, "blue") == 0) {
        led = &(l->blue);
    } else if (strcmp(color, "green") == 0) {
        led = &(l->green);
    } else {
        return -1;
    }

    if (in_trigger_array(trigger, l->triggers)) {
        return set_trigger(led->t_dev, trigger);
    }
    
    warn("incorrect trigger type");
    return -2;
}

int leds_trigger_status(struct leds_t *l, const char *color, char *status)
{
    struct led_t *led;
    if (strcmp(color, "blue") == 0) {
        led = &(l->blue);
    } else if (strcmp(color, "green") == 0) {
        led = &(l->green);
    } else {
        return -1;
    }

    char value[1024];
    int rc = get_trigger(led->t_dev, value);

    if (rc < 0) {
        return -1;
    }

    int i = 0;
    while (1) {
        if (value[i] == '[') {
            int j = 0;
            while (1) {
                i++;
                if (value[i] == ']') {
                    status[j] = '\0';
                    break;
                } else {
                    status[j] = value[i];
                }
                j++;
            }
            break;
        }
        i++;
    }

    return 0;
}
