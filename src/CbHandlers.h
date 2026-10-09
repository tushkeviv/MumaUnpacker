#pragma once

#include "plugin.h"

//#define PLUG_OPENTAB_BUTTON 7

void cbMenuEntry(CBTYPE cbType, void* callbackInfo);
void cbSystemBreakpoint(CBTYPE cbType, void* callbackInfo);
void cbBreakpoint(CBTYPE cbType, void* callbackInfo);
void cbInitDebug(CBTYPE cbType, void* callbackInfo);
void cdLoadDll(CBTYPE cbType, void* callbackInfo);
void cdStopDebug(CBTYPE cbType, void* callbackInfo);