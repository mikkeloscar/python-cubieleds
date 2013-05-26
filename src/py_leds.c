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

#include <Python.h>

#include "leds.h"

static struct leds_t l;
int module_setup;

#define SETUP_FAILED    1
#define SETUP_OK        0

static int init_module(void)
{
    module_setup = 0;

    if (leds_init(&l) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to setup module!");
        return SETUP_FAILED;
    } else {
        module_setup = 1;
        return SETUP_OK;
    }
}

/* python function on(color) */
static PyObject *
py_on(PyObject *self, PyObject *args)
{
    const char *color;

    if (!PyArg_ParseTuple(args, "s", &color)) {
        return NULL;
    }

    if (!module_setup && (init_module() != SETUP_OK)) {
        return NULL;
    }

    if (leds_on(&l, color) < 0) {
        Py_RETURN_FALSE;
    }

    Py_RETURN_TRUE;
}

/* python function off(color) */
static PyObject *
py_off(PyObject *self, PyObject *args)
{
    const char *color;

    if (!PyArg_ParseTuple(args, "s", &color)) {
        return NULL;
    }

    if (!module_setup && (init_module() != SETUP_OK)) {
        return NULL;
    }

    if (leds_off(&l, color) < 0) {
        Py_RETURN_FALSE;
    }

    Py_RETURN_TRUE;
}

/* python function status(color) */
static PyObject *
py_status(PyObject *self, PyObject *args)
{
    const char *color;
    int status;

    if (!PyArg_ParseTuple(args, "s", &color)) {
        return NULL;
    }

    if (!module_setup && (init_module() != SETUP_OK)) {
        return NULL;
    }
    
    status = leds_status(&l, color);

    if (status == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get status from led");
        return NULL;
    }

    PyObject *res = Py_BuildValue("i", status);

    return res;
}

/* python function trigger(color, trigger) */
static PyObject *
py_trigger(PyObject *self, PyObject *args)
{
    const char *color, *trigger;
    int status;

    if (!PyArg_ParseTuple(args, "ss", &color, &trigger)) {
        return NULL;
    }

    if (!module_setup && (init_module() != SETUP_OK)) {
        return NULL;
    }
    
    status = leds_trigger(&l, color, trigger);

    if (status == -1) {
        PyErr_Warn(NULL, "The led does not exist");
        Py_RETURN_FALSE;
    }

    if (status == -2) {
        PyErr_Warn(NULL, "Invalid trigger");
        Py_RETURN_FALSE;
    }

    Py_RETURN_TRUE;
}

/* python function trigger_status(color) */
static PyObject *
py_trigger_status(PyObject *self, PyObject *args)
{
    const char *color;
    int status;

    if (!PyArg_ParseTuple(args, "s", &color)) {
        return NULL;
    }

    if (!module_setup && (init_module() != SETUP_OK)) {
        return NULL;
    }
    
    char trigger[1024];
    status = leds_trigger_status(&l, color, trigger);

    if (status == -1) {
        PyErr_Warn(NULL, "The led does not exist");
        Py_RETURN_NONE;
    }

    if (status == -2) {
        PyErr_Warn(NULL, "Unable to read status");
        Py_RETURN_NONE;
    }

    PyObject *res = Py_BuildValue("s", trigger);

    return res;
}

static char module_docstring[] =
    "This module provides an interface to control the LEDs on the Cubieboard.";
static char on_docstring[] = "Turn on LED.";
static char off_docstring[] = "Turn off LED.";
static char status_docstring[] = 
    "Check status of LED. Return 1 for on and 0 for off.";
static char trigger_docstring[] = "Set LED trigger.";
static char trigger_status_docstring[] = "Read current trigger.";

static PyMethodDef module_methods[] = {
    {"on", py_on, METH_VARARGS, on_docstring},
    {"off", py_off, METH_VARARGS, off_docstring},
    {"status", py_status, METH_VARARGS, status_docstring},
    {"trigger", py_trigger, METH_VARARGS, trigger_docstring},
    {"trigger_status", py_trigger_status, METH_VARARGS, trigger_status_docstring},
    {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION > 2
static struct PyModuleDef cubieledsmodule = {
    PyModuleDef_HEAD_INIT,
    "Cubie.leds",
    module_docstring,
    -1,
    module_methods
};
#endif

#if PY_MAJOR_VERSION > 2
PyMODINIT_FUNC PyInit_leds(void)
#else
PyMODINIT_FUNC initleds(void)
#endif
{
    PyObject *module = NULL;
#if PY_MAJOR_VERSION > 2
    if ((module = PyModule_Create(&cubieledsmodule)) == NULL)
        return NULL;
#else
    module = Py_InitModule3("Cubie.leds", module_methods, module_docstring);
    if (module == NULL)
        return;
#endif

#if PY_MAJOR_VERSION > 2
    return module;
#else
    return;
#endif
}
