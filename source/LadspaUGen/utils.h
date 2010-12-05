#ifdef __cplusplus
extern "C" {
#endif

/* utils.h

   Free software by Richard W.E. Furse. Do with as you will. No
   warranty. */

#ifndef LADSPA_SDK_LOAD_PLUGIN_LIB
#define LADSPA_SDK_LOAD_PLUGIN_LIB

/*****************************************************************************/

#include "ladspa.h"

/* Functions in search.c: */

/* Callback function for use with LADSPAPluginSearch(). The callback
   function passes the filename (full path), a plugin handle (dlopen()
   style) and a LADSPA_DescriptorFunction (from which
   LADSPA_Descriptors can be acquired). */
typedef void LADSPAPluginSearchCallbackFunction
(const char * pcFullFilename, 
 void * pvPluginHandle,
 LADSPA_Descriptor_Function fDescriptorFunction);

/* Search through the $(LADSPA_PATH) (or a default path) for any
   LADSPA plugin libraries. Each plugin library is tested using
   dlopen() and dlsym(,"ladspa_descriptor"). After loading each
   library, the callback function is called to process it. This
   function leaves items passed to the callback function open. */
void LADSPAPluginSearch(LADSPAPluginSearchCallbackFunction fCallbackFunction);

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
