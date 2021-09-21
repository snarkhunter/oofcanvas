/* -*- C -*- */

#include <string.h>
#include <stdlib.h>
/* Definitions for Windows/Unix exporting */
#if defined(__WIN32__)
#   if defined(_MSC_VER)
#	define SWIGEXPORT(a) __declspec(dllexport) a
#   else
#	if defined(__BORLANDC__)
#	    define SWIGEXPORT(a) a _export 
#	else
#	    define SWIGEXPORT(a) a 
#	endif
#   endif
#else
#   define SWIGEXPORT(a) a 
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "Python.h"
extern void OCSWIG_MakePtr(char *, const void *, const char *);
extern void OCSWIG_RegisterMapping(const char *, const char *, void *(*)(void *));
extern const char *OCSWIG_GetPtr(const char *, void **, const char *);
extern const char *OCSWIG_GetPtrObj(PyObject *, void **, const char *);
extern void OCSWIG_addvarlink(PyObject *, const char *, PyObject *(*)(void), int (*)(PyObject *));
extern PyObject *OCSWIG_newvarlink(void);
#ifdef __cplusplus
}
#endif
