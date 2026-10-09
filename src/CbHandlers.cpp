#include "CbHandlers.h"
#include "BreakpointsHandlers.h"
#include "plugin.h"

void cbMenuEntry(CBTYPE cbType, void* callbackInfo)
{
    PLUG_CB_MENUENTRY* info = (PLUG_CB_MENUENTRY*)callbackInfo;
    //if (info->hEntry == PLUG_OPENTAB_BUTTON)
    //{
    //    if (!g_unpackerTab)
    //    {
    //        g_unpackerTab = new MumaUnpackerWidget();
    //        // Добавляем как вкладку с заголовком "Muma Unpacker"
    //        GuiAddQWidgetTab(g_unpackerTab);
    //        // Сразу показываем эту вкладку
    //        GuiShowQWidgetTab(g_unpackerTab);
    //    }
    //}
}

void cbSystemBreakpoint(CBTYPE cbType, void* callbackInfo)
{
    PLUG_CB_SYSTEMBREAKPOINT* info = (PLUG_CB_SYSTEMBREAKPOINT*)callbackInfo;
    DeleteAllBreaks();
    SetBreakPoints();
    is_sys_break_reached = true;
    DbgCmdExec("hide");
}

void cbBreakpoint(CBTYPE cbType, void* callbackInfo)
{
    PLUG_CB_BREAKPOINT* info = (PLUG_CB_BREAKPOINT*)callbackInfo;
    if (strstr(info->breakpoint->name, "TLS Callback") && is_auto_unpack_button_pressed)
    {
        DbgCmdExec("erun");
        return;
    }
    if (!is_sys_break_reached)
    {
        DbgCmdExec("erun");
        return;
    }
    if (is_auto_unpack_button_pressed)
    {
        if (strstr(info->breakpoint->name, "entry breakpoint"))
        {
            DbgCmdExec("erun");
            return;
        }
        char modname[MAX_MODULE_SIZE] = "";
        addr = DbgValFromString("[csp]");
        DbgGetModuleAt(addr, modname);
        if (!strstr(mainModule_name, modname))
        {
            DbgCmdExec("erun");
            return;
        }
        else
        {
            is_suitable_break_reached = true;
            if (g_unpackerTab)
            {
                g_unpackerTab->m_dumpBtn->setEnabled(true);
            }
            old_cip = DbgValFromString("cip");
            char cmd[20];
            #ifdef _WIN64
                sprintf(cmd, "set cip,%llx", addr);
            #else
                sprintf(cmd, "set cip,%x", addr);
            #endif
            DbgCmdExec(cmd);
            return;
        }
    }

}

void cbInitDebug(CBTYPE cbType, void* callbackInfo)
{
    PLUG_CB_INITDEBUG* info = (PLUG_CB_INITDEBUG*)callbackInfo;
    if (!is_open_file_button_pressed)
    {
        GuiDisplayWarning("Error unpacking!", "Invalid start!\nStop debugger and go to Unpacker Tab First!");
    }
    else
    {
        strcpy_s(mainModule_name, MAX_MODULE_SIZE, info->szFileName);
        if (g_unpackerTab)
        {
            g_unpackerTab->m_startBtn->setEnabled(true);
            g_unpackerTab->m_stopBtn->setEnabled(true);
        }
    }
}

void cdStopDebug(CBTYPE cbType, void* callbackInfo)
{
    is_auto_unpack_button_pressed = false;
    is_open_file_button_pressed = false;
    is_sys_break_reached = false;
    is_suitable_break_reached = false;
    if (g_unpackerTab)
    {
        g_unpackerTab->m_startBtn->setEnabled(false);
        g_unpackerTab->m_contBtn->setEnabled(false);
        g_unpackerTab->m_dumpBtn->setEnabled(false);
        g_unpackerTab->m_stopBtn->setEnabled(false);
    }
}

void cdLoadDll(CBTYPE cbType, void* callbackInfo)
{
    PLUG_CB_LOADDLL* info = (PLUG_CB_LOADDLL*)callbackInfo;
    if (is_auto_unpack_button_pressed)
    {
        for (int i = 0; i < functionsCount; i++)
        {
            if ((strstr(info->modname, functions[i].libname) != 0))
            {
                if (functions[i].is_enabled && !functions[i].current_state)
                {
                    char cmd[128];
                    sprintf(cmd, "bp %s:%s", functions[i].libname, functions[i].name);
                    functions[i].current_state = (DbgCmdExecDirect(cmd) != 0);
                }
            }
        }
    }
}
