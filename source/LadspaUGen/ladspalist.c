#include <dlfcn.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "ladspa.h"
#include "utils.h"

static void describePluginLibrary(const char * fn, void * handle, LADSPA_Descriptor_Function descFunc) {
    const LADSPA_Descriptor * desc;
    long i, j;

    //printf("%s:\n", fn);
    for (i = 0; (desc = descFunc(i)) != NULL; i++) {
        printf("# %lu %s\n", desc->UniqueID, desc->Name);
//        printf("# %lu %s -- %s\n", desc->UniqueID, desc->Name, desc->Maker);
//        printf("# %lu %s  <%s>\n", desc->UniqueID, desc->Name, desc->Label);
        
        for(j = 0; j < desc->PortCount; j++) {
            LADSPA_PortRangeHintDescriptor hint = desc->PortRangeHints[j].HintDescriptor;
            
            if(LADSPA_IS_PORT_INPUT(desc->PortDescriptors[j])) {
                printf(">");
            } else if(LADSPA_IS_PORT_OUTPUT(desc->PortDescriptors[j])) {
                printf("<");
            }

            if(LADSPA_IS_PORT_CONTROL(desc->PortDescriptors[j]))
                printf(" k:");
            else if(LADSPA_IS_PORT_AUDIO(desc->PortDescriptors[j]))
                printf(" a:");
            
            printf(" %s",desc->PortNames[j]);
            
            if(LADSPA_IS_HINT_BOUNDED_BELOW(hint) || LADSPA_IS_HINT_BOUNDED_ABOVE(hint)) {
                if(LADSPA_IS_HINT_BOUNDED_BELOW(hint))
                    printf(" (%g", desc->PortRangeHints[j].LowerBound);
                else
                    printf(" (...");
                printf(" to ");
                if(LADSPA_IS_HINT_BOUNDED_ABOVE(hint))
                    printf("%g)", desc->PortRangeHints[j].UpperBound);
                else
                    printf("...)");
            }
            printf("\n");
        }
        printf("\n");
    }
    dlclose(handle);
}

int main(const int iArgc, const char ** ppcArgv) {
    LADSPAPluginSearch(describePluginLibrary);
}

