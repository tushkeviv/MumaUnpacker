#pragma once

#include "pluginmain.h"
#include "MumaUnpackerWidget.h"

struct FunctionInfo {
    const char* name;
    const char* libname;
    bool is_enabled;
    bool current_state;
};

extern FunctionInfo functions[];
extern const int functionsCount;
extern bool is_auto_unpack_button_pressed;
extern bool is_open_file_button_pressed;
extern char mainModule_name[];
extern MumaUnpackerWidget* g_unpackerTab;
extern bool is_sys_break_reached;
extern bool is_suitable_break_reached;
extern duint addr;
extern duint old_cip;

//functions
bool pluginInit(PLUG_INITSTRUCT* initStruct);
void pluginStop();
void pluginSetup();
