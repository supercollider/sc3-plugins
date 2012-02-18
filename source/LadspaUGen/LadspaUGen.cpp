//SuperCollider is under GNU GPL version 3, http://supercollider.sourceforge.net/
//these extensions released under the same license

/*
 *  LadspaUGen.c
 *
 *  Run LADSPA plugins in SuperCollider
 *  by Jonatan Liljedahl 2010
 *
 */

// compile with
// g++ -shared -fPIC search.c LadspaUGen.c -I /home/lijon/Coding/SuperCollider-svn/common/Headers/plugin_interface/ -I /home/lijon/Coding/SuperCollider-svn/common/Headers/common/ -I /home/lijon/Coding/SuperCollider-svn/common/Headers/server -lm -lscsynth -ldl -o LadspaUGen.so

#include "SC_PlugIn.h"
#include "ladspa.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

static InterfaceTable *ft; 
static int plugins_index;
static int plugins_size;

struct LADSPA : public Unit 
{
    LADSPA_Handle handle;
    const LADSPA_Descriptor *desc;
    int requested_channels;
    int plugin_channels;
};

extern "C" {  
    static const LADSPA_Descriptor **plugins;
    void LADSPA_next(LADSPA *unit, int inNumSamples);
    void LADSPA_Ctor(LADSPA *unit);
    void LADSPA_Dtor(LADSPA *unit);
}

static int desc_cmp(const void *m1, const void *m2) {
    const LADSPA_Descriptor *a = *(LADSPA_Descriptor**) m1;
    const LADSPA_Descriptor *b = *(LADSPA_Descriptor**) m2;
    return (a->UniqueID - b->UniqueID);
}

void LADSPA_Ctor(LADSPA *unit) {
    LADSPA_Descriptor key, *keyp = &key;
    const LADSPA_Descriptor **found;

    unit->desc = NULL;
    unit->handle = NULL;
    unit->requested_channels = (int) IN0(0);
    if(unit->requested_channels < 1) {
        Print("LADSPA: Must request more than 0 channels\n");
        unit->mDone = true;
        SETCALC(ClearUnitOutputs);
        return;
    }

    key.UniqueID = (unsigned long) IN0(1);
    found = (const LADSPA_Descriptor**) bsearch(&keyp, &plugins[0], plugins_index, sizeof(LADSPA_Descriptor*), desc_cmp);

    if(!found) {
        Print("LADSPA: ERROR, plugin %lu not found!\n",key.UniqueID);
        unit->mDone = true;
        SETCALC(ClearUnitOutputs);
        return;
    } else {
        unit->desc = *found;
        Print("LADSPA: Found plugin %lu (%s)\n",unit->desc->UniqueID,unit->desc->Name);
    }

    unit->handle = unit->desc->instantiate(unit->desc,SAMPLERATE);

    int in_index = 2, out_index = 0;
    for(unsigned long i = 0; i < unit->desc->PortCount; i++) {
        if(LADSPA_IS_PORT_INPUT(unit->desc->PortDescriptors[i])) {
            Print("IN %d: %s ",in_index,unit->desc->PortNames[i]);
            unit->desc->connect_port(unit->handle,i,IN(in_index++));
        } else if(LADSPA_IS_PORT_OUTPUT(unit->desc->PortDescriptors[i])) {
            Print("OUT %d: %s ",out_index,unit->desc->PortNames[i]);
            if(out_index<unit->requested_channels && LADSPA_IS_PORT_AUDIO(unit->desc->PortDescriptors[i]))
                unit->desc->connect_port(unit->handle,i,OUT(out_index++));
            else
                Print("SKIPPED ");
        }
        if(LADSPA_IS_PORT_CONTROL(unit->desc->PortDescriptors[i]))
            Print("[control]\n");
        else if(LADSPA_IS_PORT_AUDIO(unit->desc->PortDescriptors[i]))
            Print("[audio]\n");
    }

    unit->plugin_channels = out_index;

//    printf("output channels: ladspa: %d ugen: %d\n",unit->plugin_channels,unit->requested_channels);

    if(unit->desc->activate)
        unit->desc->activate(unit->handle);

    SETCALC(LADSPA_next);
}

void LADSPA_Dtor(LADSPA *unit)
{
    if(!unit->desc) return;
    if(unit->desc->deactivate)
        unit->desc->deactivate(unit->handle);
    unit->desc->cleanup(unit->handle);
}

void LADSPA_next( LADSPA *unit, int inNumSamples ) {
    if(!unit->desc) return;
    unit->desc->run(unit->handle, inNumSamples);
    int i = unit->plugin_channels;
    while(i<unit->requested_channels) {
        ZClear(inNumSamples, ZOUT(i));
        i++;
    }
}

static void getPluginLibrary(const char *fn, void *dlHandle, LADSPA_Descriptor_Function descFunc) {
    const LADSPA_Descriptor * desc;

    for(long i = 0; (desc = descFunc(i)) != NULL; i++) {
        if(plugins_index >= plugins_size) {
            plugins_size += 64;
            plugins = (const LADSPA_Descriptor**) realloc((void*)plugins, sizeof(LADSPA_Descriptor*) * plugins_size);
        }
        plugins[plugins_index] = desc;
        plugins_index++;
    }
}

PluginLoad(LadspaUGen)
{
    ft = inTable;

    plugins_index = 0;
    plugins_size = 64;
    plugins = (const LADSPA_Descriptor**) malloc(sizeof(LADSPA_Descriptor*)*plugins_size);
    LADSPAPluginSearch(getPluginLibrary);

    qsort(&plugins[0], plugins_index, sizeof(LADSPA_Descriptor*), desc_cmp);

//    for(int i=0;i<plugins_index;i++) {
//        const LADSPA_Descriptor *p = plugins[i];
//        printf("%03d: %lu (%s)\n",i,p->UniqueID,p->Label);
//    }
    Print("Found %d LADSPA plugins\n",plugins_index);

//    DefineDtorUnit(LADSPA);
    DefineDtorCantAliasUnit(LADSPA);
}

