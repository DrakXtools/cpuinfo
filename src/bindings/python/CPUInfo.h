#ifndef PY_CPUINFO_H
#define PY_CPUINFO_H 1

#include <Python.h>
#include <structmember.h>
#include <cpuinfo.h>
#include <stdio.h>

typedef struct
{
    PyObject_HEAD
    PyObject *procInc;
    cpuinfo_t *cip;
} CPUInfoObject;

extern PyTypeObject CPUInfo_Type;

#endif /* PY_CPUINFO_H */
