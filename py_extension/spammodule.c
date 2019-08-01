#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string>
#include <iostream>
#include "searchresponse.h"

static PyObject *SpamError;

static PyObject *
spam_search(PyObject *self, PyObject *args)
{
    const char *arg;

    if (!PyArg_ParseTuple(args, "s", &arg))
        return NULL;

    qsearch::SearchResponse response(arg);
    std::list<std::string> urls = response.get_urls();

    PyObject *ret_urls = PyList_New(0);
    for (auto iter = urls.begin(); iter != urls.end(); ++iter) {
        PyObject *str = Py_BuildValue("s", (*iter).c_str());
        PyList_Append(ret_urls, str);
    }

    return ret_urls;
}

static PyMethodDef SpamMethods[] = {
    {"search",  spam_search, METH_VARARGS,
     "Search web pages where the expression is found"},

    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",   /* name of module */
    "This module should be used in web applications", /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    SpamMethods
};

PyMODINIT_FUNC
PyInit_qsearch(void)
{
    PyObject *m;

    m = PyModule_Create(&spammodule);
    if (m == NULL)
        return NULL;

    SpamError = PyErr_NewException("spam.error", NULL, NULL);
    Py_INCREF(SpamError);
    PyModule_AddObject(m, "error", SpamError);
    return m;
}
