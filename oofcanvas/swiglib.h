/* -*- C++ -*- */

#include <string>
#include <stdlib.h>
#include "Python.h"

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


namespace OOFCanvas {

#ifdef __cplusplus
extern "C" {
#endif
void OCSWIG_MakePtr(char *, const void *, const char *);
void OCSWIG_RegisterMappingStr(const std::string&, const std::string&, void *(*)(void *));
void OCSWIG_RegisterMapping(const char *, const char *, void *(*)(void *));
const char *OCSWIG_GetPtr(const char *, void **, const char *);
const char *OCSWIG_GetPtrObj(PyObject *, void **, const char *);
void OCSWIG_addvarlink(PyObject *, const char *, PyObject *(*)(void), int (*)(PyObject *));
PyObject *OCSWIG_newvarlink(void);

  // bool registerSWIGsubclass(const std::string &basecl,
  // 			    const std::string &subcl,
  // 			    void *(*pcnv)(void*));
#ifdef __cplusplus
}
#endif

}; // namespace OOFCanvas
