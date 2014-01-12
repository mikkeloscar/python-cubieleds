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
#include <dirent.h>

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

static int in_trigger_array(const char *elem, struct leds_t *l)
{
    int i;

    for (i = 0; i < l->num_triggers; i++) {
        if (strcmp(elem, l->triggers[i]) == 0)
            return 1;
    }

    return 0;
}

static int led_init(struct led_t *led, char *device)
{
    // make a copy of device to tokenize
    char *tmp = malloc(strlen(device)+1);
    if (tmp == NULL)
        err(EXIT_FAILURE, "Out of memory");

    strcpy(tmp, device);

    // get color from device name
    char *color = strtok(tmp, ":");

    led->color = malloc(strlen(color));
    if (led->color == NULL)
        err(EXIT_FAILURE, "Out of memory");

    strcpy(led->color, color);

    snprintf(led->b_dev, PATH_MAX, LEDS_ROOT "/%s/brightness", device);
    snprintf(led->t_dev, PATH_MAX, LEDS_ROOT "/%s/trigger", device);

    return 0;
}

static struct led_t* get_led(struct leds_t *l, const char *color)
{
    int i;

    for (i = 0; i < l->num_leds; i++) {
        if (strcmp(l->leds[i].color, color) == 0)
            return &(l->leds[i]);
    }

    return NULL;
}

int leds_init(struct leds_t *l)
{
    // list all devices and make a led_t struct from each of them
    DIR *d;
    int i = 0;
    struct dirent *dir;

    // initialize number of LEDs to 0
    l->num_triggers = 0;
    l->num_leds = 0;

    d = opendir(LEDS_ROOT);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                i++;
                if (i == 1)
                    l->leds = malloc(sizeof(struct led_t));
                else
                    l->leds = realloc(l->leds, sizeof(struct led_t) * i);

                struct led_t led;
                led_init(&led, dir->d_name);
                l->leds[i-1] = led;
                l->num_leds++;
            }
        }
        closedir(d);
    }

    // add triggers of the first LED and assume it's the same for all other
    // LEDs in /sys/class/leds/
    if (l->num_leds > 0) {
        int j, k;
        char *triggers, *token;
        triggers = malloc(sizeof(char)*1024);
        int rc = get_trigger(l->leds[0].t_dev, triggers);

        if (rc < 0)
            return -1;

        token = strsep(&triggers, " ");
        j = 0;
        while (token && token[0] != '\n') {
            if (j == 0)
                l->triggers = malloc(sizeof(char *) * 1);
            else
                l->triggers = realloc(l->triggers, sizeof(char *) * (j+1));

            if (token[0] == '[') {
                int len = strlen(token) - 1;
                l->triggers[j] = malloc(sizeof(char) * len);
                for (k = 0; k < len; k++) {
                    if (k == len-1)
                        l->triggers[j][k] = '\0';
                    else
                        l->triggers[j][k] = token[k+1];
                }
            } else {
                l->triggers[j] = malloc(sizeof(char) * (strlen(token) + 1));
                strcpy(l->triggers[j], token);
            }

            l->num_triggers++;
            token = strsep(&triggers, " ");
            j++;
        }
        free(triggers);
    }

    return 0;
}

int leds_status(struct leds_t *l, const char *color)
{
    struct led_t *led;
    led = get_led(l, color);
    if (led == NULL) {
        warn("%s LED not found", color);
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
    led = get_led(l, color);
    if (led == NULL) {
        warn("%s LED not found", color);
        return -1;
    }

    if (leds_status(l, color))
        return 1;

    return set(led->b_dev, 1);
}

int leds_off(struct leds_t *l, const char *color)
{
    struct led_t *led;
    led = get_led(l, color);
    if (led == NULL) {
        warn("%s LED not found", color);
        return -1;
    }

    if (!leds_status(l, color))
        return 0;

    return set(led->b_dev, 0);
}

int leds_trigger(struct leds_t *l, const char *color, const char *trigger)
{
    struct led_t *led;
    led = get_led(l, color);
    if (led == NULL) {
        warn("%s LED not found", color);
        return -1;
    }

    if (in_trigger_array(trigger, l))
        return set_trigger(led->t_dev, trigger);

    warn("incorrect trigger type");
    return -2;
}

int leds_trigger_status(struct leds_t *l, const char *color, char *status)
{
    struct led_t *led;
    led = get_led(l, color);
    if (led == NULL) {
        warn("%s LED not found", color);
        return -1;
    }

    char value[1024];
    int rc = get_trigger(led->t_dev, value);

    if (rc < 0)
        return -1;

    int i = 0;
    while (1) {
        if (value[i] == '[') {
            int j = 0;
            while (1) {
                i++;
                if (value[i] == ']') {
                    status[j] = '\0';
                    break;
                } else
                    status[j] = value[i];
                j++;
            }
            break;
        }
        i++;
    }

    return 0;
}
