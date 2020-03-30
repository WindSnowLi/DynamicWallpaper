#define _AFXDLL
#include <iostream>
#include <afxwin.h>
using namespace std;
//查询服务状态
//返回值0，错误
//返回值1，关闭
//返回值2，停止
//返回值3，开启
//返回值4，设置完成
int queryServiceStatus(TCHAR szSvcName[]);
TCHAR* char2TCAHR(const char* str);

int main(int argc, char* argv[])
{
    /*
    cout << argc << endl;

    路径 参数1 参数2 参数3
    cout << argv[0] << endl;
    cout << argv[1] << endl;
    cout << argv[2] << endl;
    cout << argv[3] << endl;
    */
    if (strcmp(argv[1], "查") == 0) {
        TCHAR* Tc;
        int queryStatus = 0;
        Tc = char2TCAHR(argv[2]);
        queryStatus = queryServiceStatus(Tc);
        delete Tc;
        return queryStatus;
    }
    else if (strcmp(argv[1], "设") == 0) {
        string setStatus;
        if (atoi(argv[3]) == 0) {
            setStatus = "stop ";
        }
        else {
            setStatus = "start ";
        }
        string cmdHead = "net ";
        string cmdContent = argv[2];
        string cmd = cmdHead + setStatus + cmdContent;
        system(cmd.c_str());
        return 4;
    }
}


int queryServiceStatus(TCHAR szSvcName[]) {
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;

    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwOldCheckPoint = 0;
    DWORD dwStartTickCount = 0;
    DWORD dwWaitTime = 0;
    DWORD dwBytesNeeded = 0;

    // Get a handle to the SCM database.

    schSCManager = OpenSCManager(
        NULL,                                // local computer
        NULL,                                // ServicesActive database
        SC_MANAGER_ALL_ACCESS);              // full access rights

    if (NULL == schSCManager)
    {
        return 0;
    }

    // Get a handle to the service.

    schService = OpenService(
        schSCManager,                      // SCM database
        szSvcName,                         // name of service
        SERVICE_QUERY_STATUS |
        SERVICE_ENUMERATE_DEPENDENTS);     // full access

    if (schService == NULL)
    {
        CloseServiceHandle(schSCManager);
        return 0;
    }

    // Check the status in case the service is not stopped.

    if (!QueryServiceStatusEx(
        schService,                         // handle to service
        SC_STATUS_PROCESS_INFO,             // information level
        (LPBYTE)&ssStatus,                 // address of structure
        sizeof(SERVICE_STATUS_PROCESS),     // size of structure
        &dwBytesNeeded))                  // size needed if buffer is too small
    {
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 0;
    }
    else
    {
        // Check if the service is already running. It would be possible
        // to stop the service here, but for simplicity this example just returns.
        switch (ssStatus.dwCurrentState)
        {
        case SERVICE_STOPPED:
        case SERVICE_STOP_PENDING:
            return 1;
            break;
        case SERVICE_PAUSED:
        case SERVICE_PAUSE_PENDING:
            return 2;
            break;
        case SERVICE_CONTINUE_PENDING:
        case SERVICE_RUNNING:
        case SERVICE_START_PENDING:
            return 3;
            break;
        }
    }
}
TCHAR* char2TCAHR(const char* str)

{

    int size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);

    TCHAR* retStr = new TCHAR[size * sizeof(TCHAR)];

    MultiByteToWideChar(CP_ACP, 0, str, -1, retStr, size);

    return retStr;

}