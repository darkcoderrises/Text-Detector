#ifndef ADABOOST_PYTHON_H
#define ADABOOST_PYTHON_H

#include <Python.h>
#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;

class AdaboostPython {
    public:
        string moduleName = "run_adaboost", loadFunction="load", predFunc="predict";
        PyObject *pName, *adaboostModule;

        AdaboostPython() {
            Py_Initialize();

            PyRun_SimpleString("import sys");
            PyRun_SimpleString("sys.path.append(\".\")");
            PyRun_SimpleString("from adaboost import DTC");

            pName = PyUnicode_FromString(moduleName.c_str());
            adaboostModule = PyImport_Import(pName);
        }

        void load() {
            PyObject *loadFunc = PyObject_GetAttrString(adaboostModule, 
                    loadFunction.c_str());

            PyObject *args = PyTuple_New(0);
            PyObject *result = PyObject_CallObject(loadFunc, args);
        }

        int predict(double* hist) {
            PyObject *predictFunc = PyObject_GetAttrString(adaboostModule, 
                    predFunc.c_str());

            PyObject *predictArgs = PyTuple_New(1);
            PyObject *list = PyList_New(24*24);

            for (int i=0; i<24*24; i++) {
                PyObject *listValue = PyLong_FromLong((int) hist[i]);
                PyList_SetItem(list, i, listValue);
            }
            PyTuple_SetItem(predictArgs, 0, list);

            PyObject *predictRes = PyObject_CallObject(predictFunc, predictArgs);
            return (int) PyLong_AsLong(predictRes); 
        }

        ~AdaboostPython() {
            Py_Finalize();
        }
};

#endif
