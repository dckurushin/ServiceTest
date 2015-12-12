#include "MyService.h"
#include "CommonDefines.h"
#include <Tchar.h>
#include <Windows.h>
#include <WinBase.h>

static MyService s_myServiceInstance;

MyService& MyServiceInstance()
{
	return s_myServiceInstance;
}

MyService::MyService()
{
	ZeroMemory(&m_serviceHandle, sizeof (m_serviceHandle));
}

MyService::~MyService()
{
	CloseHandle(m_serviceStopEvent);
}

void MyService::SetServiceStatus(DWORD dwStatus)
{
	switch (dwStatus)
	{
	case SERVICE_STOPPED:
		// Tell service controller we are stopped and exit
		m_serviceStatus.dwControlsAccepted = 0;
		m_serviceStatus.dwCurrentState = SERVICE_STOPPED;
		m_serviceStatus.dwWin32ExitCode = GetLastError();
		m_serviceStatus.dwCheckPoint = 1;
		break;
	case SERVICE_START_PENDING:
		// Will tell the service controller we are starting
		m_serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		m_serviceStatus.dwControlsAccepted = 0;
		m_serviceStatus.dwCurrentState = SERVICE_START_PENDING;
		m_serviceStatus.dwWin32ExitCode = S_OK;
		m_serviceStatus.dwServiceSpecificExitCode = S_OK;
		m_serviceStatus.dwCheckPoint = 0;
		break;
	case SERVICE_RUNNING:
		// Tell the service controller we are started
		m_serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		m_serviceStatus.dwCurrentState = SERVICE_RUNNING;
		m_serviceStatus.dwWin32ExitCode = S_OK;
		m_serviceStatus.dwCheckPoint = 0;
		break;
	case SERVICE_STOP_PENDING:
		m_serviceStatus.dwControlsAccepted = 0;
		m_serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		m_serviceStatus.dwWin32ExitCode = 0;
		m_serviceStatus.dwCheckPoint = 4;
		break;
	default:
		break;
	}

	if (::SetServiceStatus(m_serviceStatusHandle, &m_serviceStatus) == FALSE)
	{
		DebugPrint(_T(
			"My Sample Service: ServiceMain: SetServiceStatus returned error"));
	}
	else
	{
		std::ostringstream stream;
		stream << "Service Status changed to " << dwStatus;
		DebugPrint(stream.str().c_str());
	}
}

BOOL MyService::CreateServiceStopEvent()
{
	m_serviceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	return m_serviceStopEvent != NULL;
}

BOOL MyService::SetServiceStatusHandle(SERVICE_STATUS_HANDLE handle)
{
	if (NULL == handle) {
		return FALSE;
	}

	m_serviceStatusHandle = handle;

	return TRUE;
}