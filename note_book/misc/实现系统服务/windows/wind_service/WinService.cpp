#include <windows.h>
#include <winsvc.h>
#include <string.h>
#include <iostream>



#pragma comment (lib,"Advapi32.lib")

#define SERVICE_NAME TEXT("MemStatus")

#define LOGFILE"C:\\MyService\\memstatus.txt"
int WriteToLog(char* str)
{
    FILE* log;
    log =fopen(LOGFILE, "a+");
    if (log ==NULL)
        return -1;
    fprintf(log,"%s\n", str);
    fclose(log);
    return 0;
}


int InitService()
{
    int result;
    result =WriteToLog("Monitoring started.");
    return(result);
}


#include<stdio.h>
#include<Windows.h>
SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;
void WINAPI ServiceMain(DWORD dwArgc, LPSTR *lpszArgv);
void WINAPI ServiceHandler(DWORD fdwControl);DWORD WINAPI MyWork(LPVOID lpParam);
int main(void)
{
    SERVICE_TABLE_ENTRY ServTable[2];
    ServTable[0].lpServiceName = SERVICE_NAME;
    ServTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
    ServTable[1].lpServiceName = NULL;
    ServTable[1].lpServiceProc = NULL;
    // StartServiceCtrlDispatcher(ServTable);

    if (!StartServiceCtrlDispatcher(ServTable))
    {
        WriteToLog("StartServiceCtrlDispatcher failed\n");
    }
    return 0;
}

void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    ServiceStatus.dwServiceType = SERVICE_WIN32;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
                                       SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwWaitHint = 0;
    hStatus = RegisterServiceCtrlHandler(SERVICE_NAME,  (LPHANDLER_FUNCTION)ServiceHandler);
    if (!hStatus)
    {
        WriteToLog("Register Service Error!\n");
        system("pause");
        return;
    }
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;
    SetServiceStatus(hStatus, &ServiceStatus);    // 从这里开始可以放入你想服务为你所做的事情。
    HANDLE hThread = CreateThread(NULL, 0, MyWork, NULL, 0, NULL);
    if(hThread = NULL)
        return;
}

void WINAPI ServiceHandler(DWORD fdwControl)
{
    switch (fdwControl)
    {
    case SERVICE_CONTROL_PAUSE:
        ServiceStatus.dwCurrentState = SERVICE_PAUSED;
        break;
    case SERVICE_CONTROL_CONTINUE:
        ServiceStatus.dwCurrentState = SERVICE_RUNNING;
        break;
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        ServiceStatus.dwCheckPoint = 0;
        ServiceStatus.dwWaitHint = 0;
        SetServiceStatus(hStatus, &ServiceStatus);
        system("taskkill.exe /F /IM xdpcard_server.exe");
        return;
    default:
        break;
    }
    SetServiceStatus(hStatus, &ServiceStatus);
    return;
}

DWORD WINAPI MyWork(LPVOID lpParam)
{
    // int i = 0;
    // while(1){
    //     if(i == 0){
            system("D:\\MyProj\\code_local\\Qt\\qt_http\\build-qt_http-Desktop_Qt_6_6_1_MinGW_64_bit-Debug\\xdpcard_server.exe");
    //         i = 1;
    //     }
    //     Sleep(30000);
    // }
    return 0;
}

