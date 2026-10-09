#include "BreakpointsHandlers.h"
#include "plugin.h"

void SetBreakPoints()
{
    for (int i = 0; i < functionsCount; i++)
    {
        if (functions[i].is_enabled && !functions[i].current_state) {
            char cmd[128];
            sprintf(cmd, "bp %s:%s", functions[i].libname, functions[i].name);
            DbgCmdExec(cmd);
        }
    }
}

void SetBreakOnFunction(int num)
{
    char cmd[128];
    sprintf(cmd, "bp %s:%s", functions[num].libname, functions[num].name);
    DbgCmdExec(cmd);
    functions[num].is_enabled = true;
}

void UnSetBreakOnFunction(int num)
{
    char cmd[128];
    sprintf(cmd, "bpc %s:%s", functions[num].libname, functions[num].name);
    functions[num].is_enabled = false;
    functions[num].current_state = false;
    DbgCmdExec(cmd);
}

void DeleteAllBreaks()
{
    for (int i = 0; i < functionsCount; i++)
    {
        char cmd[128];
        sprintf(cmd, "bpc %s:%s", functions[i].libname, functions[i].name);
        DbgCmdExec(cmd);
        functions[i].current_state = false;
    }
}

void SetAllBreaks()
{
    for (int i = 0; i < functionsCount; i++)
    {
        char cmd[128];
        sprintf(cmd, "bp %s:%s", functions[i].libname, functions[i].name);
        DbgCmdExec(cmd);
        functions[i].current_state = true;
    }
}