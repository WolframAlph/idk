#include <Python.h>
#include "patch.h"

static PyObject *map;
static int running = 0;

static PyObject *watch(PyObject *module, PyObject *args) {
    PyObject *ob, *cb;
    if (!PyArg_ParseTuple(args, "OO", &ob, &cb)) {
        return NULL;
    }
    PyObject *id = PyLong_FromVoidPtr(ob);
    if (id == NULL) {
        return NULL;
    }
    if (PyDict_SetItem(map, id, cb) < 0) {
        Py_DECREF(id);
        return NULL;
    }
    Py_DECREF(id);
    Py_RETURN_NONE;
};

static void idk_free(void *_) {
    Py_XDECREF(map);
}

static PyMethodDef idkmethods[] = {
    {"watch", watch, METH_VARARGS, NULL},
    {NULL, NULL}
};

static PyModuleDef idkmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "idk",
    .m_methods = idkmethods,
    .m_free = idk_free,
    .m_doc = NULL,
};


void _patched_Py_Dealloc(PyObject* op) {
    destructor dealloc = Py_TYPE(op)->tp_dealloc;
    (*dealloc)(op);

    if (!Py_IsInitialized() || PyErr_Occurred())
        return;

    if (!running) {
        running = 1;
        PyObject *id = PyLong_FromVoidPtr(op);
        PyObject *res = NULL;

        if (id == NULL) {
            goto fail;
        }

        PyObject *cb = PyDict_GetItemWithError(map, id);

        if (cb == NULL) {
            if (PyErr_Occurred()) {
                goto fail;
            }
            goto success;
        }

        Py_INCREF(cb);
        res = PyObject_CallOneArg(cb, id);
        Py_DECREF(cb);

        if (PyDict_DelItem(map, id) < 0) {
            goto fail; 
        }

        if (res == NULL) {
            goto fail;
        }

        goto success;

fail:
        PyErr_WriteUnraisable(NULL);
success:
        Py_XDECREF(id);
        Py_XDECREF(res);
        running = 0;
    }
}

PyMODINIT_FUNC PyInit_idk(void) {
    redirect(_Py_Dealloc, _patched_Py_Dealloc);
    if ((map = PyDict_New()) == NULL) return NULL;
    return PyModule_Create(&idkmodule);
}
