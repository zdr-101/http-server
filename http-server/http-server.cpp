#include "crow.h"
#include <iostream>
#include <Windows.h>
#include <winsvc.h>
#include <string>
#include <thread>
#pragma comment (lib,"Advapi32.lib")
#define SERVICE_NAME "TestService"
#define SERVICE_DESC "TestService cao shang pa"
SERVICE_STATUS gServiceStatus;
SERVICE_STATUS_HANDLE gServiceStatusHandle;
HANDLE gServiceStopEvent = NULL;
crow::SimpleApp app;
void WINAPI ServiceMain(int argc, char** argv);
void WINAPI ServiceCtrlHandler(DWORD request);
void ReportServiceStatus(DWORD CurrentStatus, DWORD Win32ExitCode, DWORD WaitHint);
void RunApp();
void WINAPI ServiceMain(int argc, char** argv)
{
	gServiceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
	if (!gServiceStatusHandle)
	{
		std::cout << "RegisterServiceCtrlHandler failed" << std::endl;
		OutputDebugString("RegisterServiceCtrlHandler failed");
		return;
	}
	std::cout << "RegisterServiceCtrlHandler succeed" << std::endl;
	OutputDebugString("RegisterServiceCtrlHandler succeed");
	// 向SCM 报告运行状态.
	ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000);
	OutputDebugString("Service starting...");
	// 在此处添加你自己希望服务做的工作，
   // TODO
	std::thread appThread(RunApp);
	appThread.detach();
	gServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!gServiceStopEvent)
	{
		ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
		OutputDebugString("Service stopped");
		return;
	}
	else
	{
		ReportServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);
		OutputDebugString("Service running...");
	}
	WaitForSingleObject(gServiceStopEvent, INFINITE);
	ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
	OutputDebugString("Service stopped");
}
void WINAPI ServiceCtrlHandler(DWORD request)
{
	switch (request)
	{
	case SERVICE_CONTROL_STOP:
		ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
		OutputDebugString("Service stopping...");
		SetEvent(gServiceStopEvent);
		app.stop();
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		break;
	default:
		break;
	}
	ReportServiceStatus(gServiceStatus.dwCurrentState, NO_ERROR, 0);
}
void ReportServiceStatus(DWORD CurrentStatus, DWORD Win32ExitCode, DWORD WaitHint)
{
	static DWORD nCheckPoint = 1;
	gServiceStatus.dwServiceType = SERVICE_WIN32;
	gServiceStatus.dwCurrentState = CurrentStatus;
	gServiceStatus.dwWin32ExitCode = Win32ExitCode;
	gServiceStatus.dwWaitHint = WaitHint;
	if (CurrentStatus == SERVICE_START_PENDING)
	{
		gServiceStatus.dwControlsAccepted = 0;
	}
	else
	{
		gServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	}
	if ((CurrentStatus == SERVICE_RUNNING) || (CurrentStatus == SERVICE_STOPPED))
	{
		gServiceStatus.dwCheckPoint = 0;
	}
	else
	{
		gServiceStatus.dwCheckPoint = nCheckPoint++;
	}
	gServiceStatus.dwServiceSpecificExitCode = 0;
	SetServiceStatus(gServiceStatusHandle, &gServiceStatus);
}
int InstallService()
{
	// 打开服务控制管理器.
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		std::cout << "OpenSCManager failed" << std::endl;
		OutputDebugString("OpenSCManager failed");
		return -1;
	}
	// 获取可执行文件路径
	TCHAR szFilePath[MAX_PATH];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	// 创建服务，并设置服务自动启动.
	SC_HANDLE hService = CreateService(
		hSCM, SERVICE_NAME, SERVICE_NAME,
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		szFilePath, NULL, NULL, NULL, NULL, NULL);
	if (hService == NULL)
	{
		CloseServiceHandle(hSCM);
		std::cout << "CreateService failed" << std::endl;
		OutputDebugString("CreateService failed");
		return -1;
	}
	// 设置服务描述（在服务列表中可与看到）
	SERVICE_DESCRIPTION Description;
	TCHAR szDescription[1024];
	ZeroMemory(szDescription, 0);
	ZeroMemory(&Description, sizeof(SERVICE_DESCRIPTION));
	lstrcpy(szDescription, SERVICE_DESC);
	Description.lpDescription = szDescription;
	ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &Description);
	if (StartService(hService, 0, NULL) == FALSE)
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		std::cout << "StartService failed" << std::endl;
		OutputDebugString("StartService failed");
		return -1;
	}
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	std::cout << "Install service succeed" << std::endl;
	OutputDebugString("Install service succeed");
	return 0;
}
int UninstallService()
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		std::cout << "OpenSCManager failed" << std::endl;
		OutputDebugString("OpenSCManager failed");
		return -1;
	}
	SC_HANDLE hService = OpenService(hSCM, SERVICE_NAME, SERVICE_QUERY_STATUS | SERVICE_STOP | DELETE);
	if (hService == NULL)
	{
		CloseServiceHandle(hSCM);
		std::cout << "OpenService failed" << std::endl;
		OutputDebugString("OpenService failed");
		return -1;
	}
	SERVICE_STATUS status;
	if (QueryServiceStatus(hService, &status) == FALSE)
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		std::cout << "QueryServiceStatus failed" << std::endl;
		OutputDebugString("QueryServiceStatus failed");
		return -1;
	}
	if (status.dwCurrentState == SERVICE_RUNNING)
	{
		if (ControlService(hService, SERVICE_CONTROL_STOP, &status) == FALSE)
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			std::cout << "Stop Service failed" << std::endl;
			OutputDebugString("Stop Service failed");
			return -1;
		}
	}
	// 如果服务状态为已停止，则卸载该系统服务.
	if (status.dwCurrentState == SERVICE_STOPPED)
	{
		// 删除服务.
		BOOL bDelete = DeleteService(hService);
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		if (bDelete)
		{
			std::cout << "Uninstall service succeed" << std::endl;
			OutputDebugString("Uninstall service succeed");
			return 0;
		}
		else
		{
			std::cout << "Uninstall service failed" << std::endl;
			OutputDebugString("Uninstall service failed");
			return -1;
		}
	}
	return -1;
}
void RunApp()
{
	CROW_ROUTE(app, "/")
		([]() {
		return "Hello, world!";
			});
	app.port(18080).run();
}
int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		if (strcmp(argv[1], "-install") == 0)
		{
			InstallService();
		}
		else if (strcmp(argv[1], "-uninstall") == 0)
		{
			UninstallService();
		}
	}
	else
	{
		SERVICE_TABLE_ENTRY entrytable[2];
		entrytable[0].lpServiceName = const_cast<char*>(std::string(SERVICE_NAME).c_str());
		entrytable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
		entrytable[1].lpServiceName = NULL;
		entrytable[1].lpServiceProc = NULL;
		if (!StartServiceCtrlDispatcher(entrytable))
		{
			std::cout << "StartServiceCtrlDispatcher failed";
			OutputDebugString("StartServiceCtrlDispatcher failed");
		}
	}
	return 0;
}