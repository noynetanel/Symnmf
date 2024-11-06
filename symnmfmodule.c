#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h>
#include <stdio.h>
#include "symnmf.h"

/* Declare the shared global variables */
extern int n;
extern int d;
extern int k;
extern double** D;
extern double** W;

/* Declare the freeMatrix function */
void freeMatrix(double **matrix);

/* Helper function to convert a Python list to a 2D C array */
static double** transformPyObjectToDoubleMatrix(PyObject* pyArray, int numRows, int numCols) {
    int i,j,r;
    double** doubleArray = malloc(numRows * sizeof(double*));
    if (doubleArray == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Memory allocation failed");
        return NULL;
    }

    for (i = 0; i < numRows; ++i) {
        doubleArray[i] = malloc(numCols * sizeof(double));
        if (doubleArray[i] == NULL) {
            PyErr_SetString(PyExc_MemoryError, "Memory allocation failed");
            for (j = 0; j < i; ++j) {
                free(doubleArray[j]);
            }
            free(doubleArray);
            return NULL;
        }

        PyObject* row = PyList_GetItem(pyArray, i);
        if (!PyList_Check(row) || PyList_Size(row) != numCols) {
            PyErr_SetString(PyExc_TypeError, "Input is not a 2D list of appropriate size");
            for ( j = 0; j <= i; ++j) {
                free(doubleArray[j]);
            }
            free(doubleArray);
            return NULL;
        }

        for (j = 0; j < numCols; ++j) {
            PyObject* item = PyList_GetItem(row, j);
            if (!PyFloat_Check(item)) {
                PyErr_SetString(PyExc_TypeError, "Element is not a float");
                for (r = 0; r <= i; ++r) {
                    free(doubleArray[r]);
                }
                free(doubleArray);
                return NULL;
            }
            doubleArray[i][j] = PyFloat_AsDouble(item);
        }
    }
    return doubleArray;
}

/* Helper function to convert a 2D C array to a Python list */
static PyObject* transformDoubleMatrixToPyObjectList(double** doubleArray, int numRows, int numCols) {
    PyObject* pyArray = PyList_New(numRows);
    if (!pyArray) {
        return NULL;
    }

    for (int i = 0; i < numRows; ++i) {
        PyObject* row = PyList_New(numCols);
        if (!row) {
            Py_DECREF(pyArray);
            return NULL;
        }

        for (int j = 0; j < numCols; ++j) {
            PyObject* value = PyFloat_FromDouble(doubleArray[i][j]);
            if (!value) {
                Py_DECREF(pyArray);
                Py_DECREF(row);
                return NULL;
            }
            PyList_SET_ITEM(row, j, value);
        }

        PyList_SET_ITEM(pyArray, i, row);
    }
    return pyArray;
}

/* Python API for symC */
static PyObject* sym(PyObject *self, PyObject *args) {
    PyObject* X_array;
    if (!PyArg_ParseTuple(args, "O", &X_array)) {
        return NULL;
    }

    n = PyList_Size(X_array);
    d = PyList_Size(PyList_GetItem(X_array, 0));
    double** X = transformPyObjectToDoubleMatrix(X_array, n, d);
    if (X == NULL) return NULL;

    double** A = symC(X);
    PyObject* py_result = transformDoubleMatrixToPyObjectList(A, n, n);

    /* Free memory */
    freeMatrix(X);
    freeMatrix(A);
    return py_result;
}

/* Python API for ddgC */
static PyObject* ddg(PyObject *self, PyObject *args) {
    PyObject* X_array;
    if (!PyArg_ParseTuple(args, "O", &X_array)) {
        return NULL;
    }

    n = PyList_Size(X_array);
    d = PyList_Size(PyList_GetItem(X_array, 0));
    double** X = transformPyObjectToDoubleMatrix(X_array, n, d);
    if (X == NULL) return NULL;

    D = ddgC(X);
    PyObject* py_result = transformDoubleMatrixToPyObjectList(D, n, n);

    /* Free memory */
    freeMatrix(X);
    freeMatrix(D);
    return py_result;
}

/* Python API for normC */
static PyObject* norm(PyObject *self, PyObject *args) {
    PyObject* X_array;
    if (!PyArg_ParseTuple(args, "O", &X_array)) {
        return NULL;
    }

    n = PyList_Size(X_array);
    d = PyList_Size(PyList_GetItem(X_array, 0));
    double** X = transformPyObjectToDoubleMatrix(X_array, n, d);
    if (X == NULL) return NULL;

    W = normC(X);
    PyObject* py_result = transformDoubleMatrixToPyObjectList(W, n, n);

    /* Free memory */
    freeMatrix(X);
    freeMatrix(D);
    freeMatrix(W);
    return py_result;
}
/* Python API for symnmfC */
static PyObject* symnmf(PyObject *self, PyObject *args) {
    PyObject *H_array, *W_array;
    if (!PyArg_ParseTuple(args, "iOO", &k, &W_array, &H_array)) {
        return NULL;
    }
    /*n = PyList_Size(H_array);*/
    double** W = transformPyObjectToDoubleMatrix(W_array, n, n);
    double** H = transformPyObjectToDoubleMatrix(H_array, n, k);

    if (W == NULL || H == NULL){
        return NULL;
    } 

    double** result = symnmfC(H);
    PyObject* py_result = transformDoubleMatrixToPyObjectList(result, n, k);
    /* Free memory */
    freeMatrix(W);
    freeMatrix(H);
    freeMatrix(result);
    return py_result;
}

static PyMethodDef symnmfMethods[] = {
    {"sym", (PyCFunction)sym, METH_VARARGS, PyDoc_STR("Generate similarity matrix A from X")},
    {"norm", (PyCFunction)norm, METH_VARARGS, PyDoc_STR("Compute the normalized similarity matrix W")},
    {"ddg", (PyCFunction)ddg, METH_VARARGS, PyDoc_STR("Compute the diagonal degree matrix D")},
    {"symnmf", (PyCFunction)symnmf, METH_VARARGS, PyDoc_STR("Perform Symmetric Nonnegative Matrix Factorization")},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmfmodule", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,  /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    symnmfMethods /* the PyMethodDef array containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_symnmfmodule(void) {
    return PyModule_Create(&symnmfmodule);
}
