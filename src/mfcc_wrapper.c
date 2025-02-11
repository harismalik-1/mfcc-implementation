#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "mfcc.h"

static PyObject* compute_mfcc_wrapper(PyObject* self, PyObject* args) {
    PyObject* input_list;
    int sample_rate, n_fft, n_mels, n_mfcc;
    float fmin, fmax, preemphasis;
    
    if (!PyArg_ParseTuple(args, "Oiiiifff", &input_list, &sample_rate, 
                         &n_fft, &n_mels, &n_mfcc, &fmin, &fmax, &preemphasis))
        return NULL;
    
    // Convert Python list to C array
    int signal_length = PyList_Size(input_list);
    float* signal = (float*)malloc(signal_length * sizeof(float));
    for (int i = 0; i < signal_length; i++) {
        PyObject* item = PyList_GetItem(input_list, i);
        signal[i] = (float)PyFloat_AsDouble(item);
    }
    
    // Configure MFCC
    MFCCConfig config = {
        .preemphasis = 0.97f,
        .frame_length = n_fft,
        .frame_step = n_fft / 4,  // 75% overlap
        .sample_rate = sample_rate,
        .n_mels = n_mels,
        .n_mfcc = n_mfcc,
        .fmin = 0.0f,
        .fmax = sample_rate / 2.0f,
        .n_fft = n_fft,
        .mel_scale_type = MEL_SCALE_HTK,  // Use HTK by default
        .normalize_mel = true  // Enable Slaney normalization
    };
    
    // Compute MFCCs
    float* mfcc_features = compute_mfcc(signal, signal_length, &config);
    
    // Convert to Python list
    PyObject* result = PyList_New(n_mfcc);
    for (int i = 0; i < n_mfcc; i++) {
        PyList_SetItem(result, i, PyFloat_FromDouble(mfcc_features[i]));
    }
    
    // Clean up
    free(signal);
    free(mfcc_features);
    
    return result;
}

static PyMethodDef MFCCMethods[] = {
    {"compute_mfcc", compute_mfcc_wrapper, METH_VARARGS, "Compute MFCC features"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef mfccmodule = {
    PyModuleDef_HEAD_INIT,
    "mfcc",
    NULL,
    -1,
    MFCCMethods
};

PyMODINIT_FUNC PyInit_mfcc(void) {
    return PyModule_Create(&mfccmodule);
} 