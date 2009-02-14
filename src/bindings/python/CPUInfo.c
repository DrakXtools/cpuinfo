#include "CPUInfo.h"

static const char __author__[] =
"The pycpuinfo python module was written by:\n\
\n\
Per Øyvind Karlsen <peroyvind@mandriva.org>\n\
";

PyDoc_STRVAR(CPUInfo_init__doc__,
"CPUInfo() -> CPUInfo object\n\
\n\
Initializes a CPUInfo object holding the cpuinfo descriptor for your CPU.");

static int
CPUInfo_init(CPUInfoObject *self)
{
    self->cip = cpuinfo_new();

    if(self->cip == NULL){
	PyErr_SetString(PyExc_RuntimeError, "couldn't create new cpuinfo descriptor.");
	return -1;
    }
    PyDict_SetItem(self->procInf, PyString_FromString("model", 

    return 0;

}

PyDoc_STRVAR(CPUInfo_getVendorID__doc__,
"getVendorID() -> int\n\
\n\
Get processor vendor ID");

static PyObject*
CPUInfo_getVendorID(CPUInfoObject *self)
{
    return PyInt_FromLong(cpuinfo_get_vendor(self->cip));
}

static PyObject*
CPUInfo_getVendorName(CPUInfoObject *self, PyObject *args)
{
    cpuinfo_vendor_t vendorID = CPUINFO_VENDOR_UNKNOWN;

    if(!PyArg_ParseTuple(args, "|i", &vendorID))
	return NULL;
    if(vendorID == CPUINFO_VENDOR_UNKNOWN)
	vendorID = cpuinfo_get_vendor(self->cip);

    return PyString_FromFormat("%s\n", cpuinfo_string_of_vendor(vendorID));
}

static PyObject*
CPUInfo_getModel(CPUInfoObject *self)
{
    return PyString_FromFormat("%s\n", cpuinfo_get_model(self->cip));
}

static PyObject*
CPUInfo_getFrequency(CPUInfoObject *self)
{
    return PyInt_FromLong(cpuinfo_get_frequency(self->cip));
}

static PyObject*
CPUInfo_getSocket(CPUInfoObject *self)
{
    return PyInt_FromLong(cpuinfo_get_socket(self->cip));
}

static PyObject*
CPUInfo_getCores(CPUInfoObject *self)
{
    return PyInt_FromLong(cpuinfo_get_cores(self->cip));
}

static PyObject*
CPUInfo_getThreads(CPUInfoObject *self)
{
    return PyInt_FromLong(cpuinfo_get_threads(self->cip));
}

static PyObject*
CPUInfo_getCacheTypes(CPUInfoObject *self)
{
    const cpuinfo_cache_t *ccp = cpuinfo_get_caches(self->cip);

    PyObject *ccp_tuple = PyTuple_New(ccp->count);
    int i;
    for (i = 0; i < ccp->count; i++) {
	const cpuinfo_cache_descriptor_t *ccdp = &ccp->descriptors[i];
	PyTuple_SetItem(ccp_tuple, i, PyString_FromString(cpuinfo_string_of_cache_type(ccdp->type)));
    }
    return ccp_tuple;
}

static PyMethodDef CPUInfoObject_methods[] = {
    {"getVendorID", (PyCFunction)CPUInfo_getVendorID, 0,
	CPUInfo_getVendorID__doc__},
    {"getVendorName", (PyCFunction)CPUInfo_getVendorName, METH_VARARGS, NULL},
    {"getModel", (PyCFunction)CPUInfo_getModel, 0, NULL},
    {"getFrequency", (PyCFunction)CPUInfo_getFrequency, 0, NULL},
    {"getSocket", (PyCFunction)CPUInfo_getSocket, 0, NULL},
    {"getCores", (PyCFunction)CPUInfo_getCores, 0, NULL},
    {"getThreads", (PyCFunction)CPUInfo_getThreads, 0, NULL},
    {"getCacheTypes", (PyCFunction)CPUInfo_getCacheTypes, 0, NULL},
    {0, 0, 0, 0}
};

static PyObject *
CPUInfoObject_new(PyTypeObject *type, __attribute__((unused)) PyObject *args, __attribute__((unused)) PyObject *kwargs)
{
    CPUInfoObject *self;
    self = (CPUInfoObject *)type->tp_alloc(type, 0);
    if(self == NULL){
	Py_DECREF(self);
	return NULL;
    }
    self->procInf = PyDict_New();

    return (PyObject *)self;
}

static void
CPUInfo_dealloc(CPUInfoObject *self)
{
    if(self->cip != NULL)
	cpuinfo_destroy(self->cip);
    self->ob_type->tp_free((PyObject*)self);
}

PyTypeObject CPUInfo_Type = {
PyObject_HEAD_INIT(NULL)
	0,						/*ob_size*/
	"CPUInfo.CPUInfo",				/*tp_name*/
	sizeof(CPUInfoObject),				/*tp_basicsize*/
	0,						/*tp_itemsize*/
	(destructor)CPUInfo_dealloc,			/*tp_dealloc*/
	0,						/*tp_print*/
	0,						/*tp_getattr*/
	0,						/*tp_setattr*/
	0,						/*tp_compare*/
	0,						/*tp_repr*/
	0,						/*tp_as_number*/
	0,						/*tp_as_sequence*/
	0,						/*tp_as_mapping*/
	0,						/*tp_hash*/
	0,						/*tp_call*/
	0,						/*tp_str*/
	0,						/*tp_getattro*/
	0,						/*tp_setattro*/
	0,						/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,		/*tp_flags*/
	CPUInfo_init__doc__,         			/*tp_doc*/
	0,						/*tp_traverse*/
	0,						/*tp_clear*/
	0,						/*tp_richcompare*/
	0,						/*tp_weaklistoffset*/
	0,						/*tp_iter*/
	0,						/*tp_iternext*/
	CPUInfoObject_methods,				/*tp_methods*/
	0,						/*tp_members*/
	0,						/*tp_getset*/
	0,						/*tp_base*/
	0,						/*tp_dict*/
	0,						/*tp_descr_get*/
	0,						/*tp_descr_set*/
	0,						/*tp_dictoffset*/
	(initproc)CPUInfo_init,				/*tp_init*/
	PyType_GenericAlloc,				/*tp_alloc*/
	CPUInfoObject_new,				/*tp_new*/
	0,						/*tp_free*/
	0,						/*tp_is_gc*/
	0,						/*tp_bases*/
	0,						/*tp_mro*/
	0,						/*tp_cache*/
	0,						/*tp_subclasses*/
	0,						/*tp_weaklist*/
	0,						/*tp_del*/
	0
    };

static PyMethodDef CPUInfo_methods[] = {
    {0, 0, 0, 0}
};

PyDoc_STRVAR(CPUInfo_module_documentation,
	"The python CPUInfo module provides a interface for the CPUInfo\n\
	library.");

/* declare function before defining it to avoid compile warnings */
PyMODINIT_FUNC initCPUInfo(void);
PyMODINIT_FUNC
initCPUInfo(void)
{
    PyObject *m;
    if (PyType_Ready(&CPUInfo_Type) < 0)
	return;
    m = Py_InitModule3("CPUInfo", CPUInfo_methods,
	    CPUInfo_module_documentation);
    if (m == NULL)
	return;

    Py_INCREF(&CPUInfo_Type);
    PyModule_AddObject(m, "CPUInfo", (PyObject *)&CPUInfo_Type);

    PyModule_AddObject(m, "__author__", PyString_FromString(__author__));

    PyModule_AddStringConstant(m, "__version__", VERSION);
}

