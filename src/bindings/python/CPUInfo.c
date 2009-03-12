#include "CPUInfo.h"
#include <stdio.h>

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
    int i;
    self->cip = cpuinfo_new();

    if(self->cip == NULL){
	PyErr_SetString(PyExc_RuntimeError, "couldn't create new cpuinfo descriptor.");
	return -1;
    }

    const cpuinfo_cache_t *ccp = cpuinfo_get_caches(self->cip);
    self->caches = PyTuple_New(ccp->count);

    if (ccp) {
	for (i = 0; i < ccp->count; i++) {
	    const cpuinfo_cache_descriptor_t *ccdp = &ccp->descriptors[i];

	    PyObject *type;
	    if (ccdp->level == 0 && ccdp->type == CPUINFO_CACHE_TYPE_TRACE)
		type = PyString_FromString("Instruction trace cache");
	    else
		type = PyString_FromFormat("L%d %s cache", ccdp->level, cpuinfo_string_of_cache_type(ccdp->type));
	    PyObject *cache = PyTuple_Pack(2, type, PyInt_FromLong(ccdp->size*1024));
	    PyTuple_SetItem(self->caches, i, cache);
	}
    }

    self->features = PyDict_New();    
    PyObject *genFeats = PyDict_New();
    PyObject *archFeats = PyDict_New();
    PyDict_SetItemString(self->features, "general", genFeats);
    PyDict_SetItemString(self->features, "architecture", archFeats);

    cpuinfo_feature_t feature;

    for (feature = cpuinfo_feature_common; feature != cpuinfo_feature_architecture_max; feature++) {
    	if(feature == cpuinfo_feature_common_max)
	    feature = cpuinfo_feature_architecture;
	const char *name = cpuinfo_string_of_feature(feature);
	const char *detail = cpuinfo_string_of_feature_detail(feature);
	if (name && detail)
	{
    	    if(feature < cpuinfo_feature_common_max)
		PyDict_SetItemString(genFeats, name, PyString_FromString(detail));
	    else
		PyDict_SetItemString(archFeats, name, PyString_FromString(detail));
	}
	else
	    fprintf(stdout, "  %-10s No description for feature %08x\n", "<error>", feature);
    }

    return 0;
}

static PyObject *
CPUInfo_getVendorName(CPUInfoObject *self)
{
    cpuinfo_vendor_t vendorID = cpuinfo_get_vendor(self->cip);;

    return PyString_FromFormat("%s", cpuinfo_string_of_vendor(vendorID));
}

static PyObject *
CPUInfo_getModel(CPUInfoObject *self)
{
    return PyString_FromFormat("%s", cpuinfo_get_model(self->cip));
}

static PyObject *
CPUInfo_getFrequency(CPUInfoObject *self)
{
    return PyInt_FromLong(cpuinfo_get_frequency(self->cip));
}

static PyObject *
CPUInfo_getSocket(CPUInfoObject *self)
{
    return PyInt_FromLong(cpuinfo_get_socket(self->cip));
}

static PyObject *
CPUInfo_getCores(CPUInfoObject *self)
{
    return PyInt_FromLong(cpuinfo_get_cores(self->cip));
}

static PyObject *
CPUInfo_getThreads(CPUInfoObject *self)
{
    return PyInt_FromLong(cpuinfo_get_threads(self->cip));
}

static PyObject *
CPUInfo_getCacheTypes(CPUInfoObject *self)
{
    return self->caches;
}

static PyObject *
CPUInfo_getFeatures(CPUInfoObject *self)
{
    return self->features;
}

static PyGetSetDef CPUInfo_getset[] = {
    {"caches|", (getter)CPUInfo_getCacheTypes, NULL,
	"Processor caches", NULL},
    {"cores", (getter)CPUInfo_getCores, NULL,
	"Processor caches", NULL},
    {"frequency", (getter)CPUInfo_getFrequency, NULL,
	"Processor frequency", NULL},	
    {"features", (getter)CPUInfo_getFeatures, NULL,
	"general and architecture specific features", NULL},
    {"model", (getter)CPUInfo_getModel, NULL,
	"Processor model", NULL},
    {"socket", (getter)CPUInfo_getSocket, NULL,
	"Processor socket", NULL},	
    {"threads", (getter)CPUInfo_getThreads, NULL,
	"Processor threads", NULL},
    {"vendorname", (getter)CPUInfo_getVendorName, NULL,
	"Processor model", NULL},
    {NULL, NULL, NULL, NULL, NULL}	/* Sentinel */
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

    return (PyObject *)self;
}

static void
CPUInfo_dealloc(CPUInfoObject *self)
{
    if(self->cip != NULL)
	cpuinfo_destroy(self->cip);
    self->ob_type->tp_free((PyObject*)self);
    Py_DecRef(self->caches);
    Py_DecRef(self->features);
}

PyTypeObject CPUInfo_Type = {
    PyObject_HEAD_INIT(NULL)
    0,						/*ob_size*/
    "CPUInfo.CPUInfo",				/*tp_name*/
    sizeof(CPUInfoObject),			/*tp_basicsize*/
    0,						/*tp_itemsize*/
    (destructor)CPUInfo_dealloc,		/*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,	/*tp_flags*/
    CPUInfo_init__doc__,         		/*tp_doc*/
    0,						/*tp_traverse*/
    0,						/*tp_clear*/
    0,						/*tp_richcompare*/
    0,						/*tp_weaklistoffset*/
    0,						/*tp_iter*/
    0,						/*tp_iternext*/
    0,						/*tp_methods*/
    0,						/*tp_members*/
    CPUInfo_getset,				/*tp_getset*/
    0,						/*tp_base*/
    0,						/*tp_dict*/
    0,						/*tp_descr_get*/
    0,						/*tp_descr_set*/
    0,						/*tp_dictoffset*/
    (initproc)CPUInfo_init,			/*tp_init*/
    PyType_GenericAlloc,			/*tp_alloc*/
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

