#include "plugin.h"
#include "CbHandlers.h"
#include "MumaUnpackerWidget.h"

MumaUnpackerWidget* g_unpackerTab = nullptr;
bool is_auto_unpack_button_pressed = false;
bool is_open_file_button_pressed = false;
bool is_sys_break_reached = false;
bool is_suitable_break_reached = false;
duint addr = 0;
duint old_cip = 0;
char mainModule_name[MAX_MODULE_SIZE];

// Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    // Prefix of the functions to call here: _plugin_register
    _plugin_registercallback(pluginHandle, CB_MENUENTRY, cbMenuEntry);
    _plugin_registercallback(pluginHandle, CB_SYSTEMBREAKPOINT, cbSystemBreakpoint);
    _plugin_registercallback(pluginHandle, CB_BREAKPOINT, cbBreakpoint);
    _plugin_registercallback(pluginHandle, CB_INITDEBUG, cbInitDebug);
    _plugin_registercallback(pluginHandle, CB_LOADDLL, cdLoadDll);
    _plugin_registercallback(pluginHandle, CB_STOPDEBUG, cdStopDebug);
    // Return false to cancel loading the plugin.
    return true;
}

// Deinitialize your plugin data here.
// NOTE: you are responsible for gracefully closing your GUI
// This function is not executed on the GUI thread, so you might need
// to use WaitForSingleObject or similar to wait for everything to close.
void pluginStop()
{
    // Prefix of the functions to call here: _plugin_unregister
    _plugin_unregistercallback(pluginHandle, CB_MENUENTRY);
    _plugin_unregistercallback(pluginHandle, CB_SYSTEMBREAKPOINT);
    _plugin_unregistercallback(pluginHandle, CB_BREAKPOINT);
    _plugin_unregistercallback(pluginHandle, CB_INITDEBUG);
    _plugin_unregistercallback(pluginHandle, CB_LOADDLL);
    _plugin_unregistercallback(pluginHandle, CB_STOPDEBUG);

    if (g_unpackerTab)
    {
        GuiCloseQWidgetTab(g_unpackerTab);
    }
}

// Do GUI/Menu related things here.
// This code runs on the GUI thread: GetCurrentThreadId() == GuiGetMainThreadId()
// You can get the HWND using GuiGetWindowHandle()
void pluginSetup()
{
    // Prefix of the functions to call here: _plugin_menu
    //_plugin_menuaddentry(hMenu, PLUG_OPENTAB_BUTTON, "Open AutoUnpackMenu");
    if (!g_unpackerTab)
    {
        g_unpackerTab = new MumaUnpackerWidget();
        GuiAddQWidgetTab(g_unpackerTab);
        GuiShowQWidgetTab(g_unpackerTab);
    }
}

FunctionInfo functions[] = {
    {"OpenProcess", "kernelbase", true, false},
    {"CreateProcessA", "kernelbase", true, false},
    {"CreateProcessW", "kernelbase", true, false},
    {"VirtualAllocEx", "kernelbase", true, false},
    {"WriteProcessMemory", "kernelbase", true, false},
    {"CreateRemoteThread", "kernel32", true, false},
    {"CreateRemoteThreadEx", "kernelbase", true, false},
    {"NtCreateThreadEx", "ntdll", true, false},
    {"SetThreadContext", "kernelbase", true, false},
    {"GetThreadContext", "kernelbase", true, false},
    {"ResumeThread", "kernelbase", true, false},
    {"SetWindowsHookExA", "user32", false, false},
    {"SetWindowsHookExW", "user32", true, false},
    {"RtlCreateUserThread", "ntdll", true, false},
    {"ZwWriteVirtualMemory", "ntdll", true, false},
    {"GetProcAddress", "kernelbase", true, false},
    {"CreateFileA", "kernelbase", true, false},
    {"CreateFileW", "kernelbase", true, false},
    {"Process32First", "kernel32", true, false},
    {"Process32Next", "kernel32", true, false},
    {"ReadProcessMemory", "kernelbase", true, false},
    {"SuspendThread", "kernelbase", true, false},
    {"VirtualProtect", "kernelbase", false, false},
    {"VirtualProtectEx", "kernelbase", true, false},
    {"OpenFileMappingA", "kernelbase", true, false},
    {"NtCreateProcess", "ntdll", true, false},
    {"NtCreateProcessEx", "ntdll", true, false},
    {"NtCreateThread", "ntdll", true, false},
    {"NtOpenProcess", "ntdll", true, false},
    {"NtProtectVirtualMemory", "ntdll", true, false},
    {"Sleep", "kernel32", true, false},
    {"SleepEx", "kernel32", true, false},
    {"URLDownloadToFile", "urlmon", true, false},
    {"ShellExecuteA", "shell32", true, false},
    {"ShellExecuteExA", "shell32", true, false},
    {"CreateToolhelp32Snapshot", "kernel32", true, false},
    {"IsDebuggerPresent", "kernelbase", true, false},
    {"CreateMutexA", "kernelbase", true, false},
    {"CreateMutexExA", "kernelbase", true, false},
    {"WriteFile", "kernelbase", true, false},
    {"RegCreateKeyExA", "advapi32", true, false},
    {"RegCreateKeyA", "advapi32", true, false},
    {"RegSetValueExA", "advapi32", true, false},
    {"RegSetKeyValueA", "advapi32", true, false},
    {"NtCreateFile", "ntdll", true, false}
};

const int functionsCount = sizeof(functions) / sizeof(functions[0]);