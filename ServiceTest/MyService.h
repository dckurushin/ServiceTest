#pragma once

#include <Windows.h>
#include <Winsvc.h>
#include <WinDef.h>
#include <WinNT.h>

class MyService
{
public:
	MyService();
	virtual ~MyService();

	SERVICE_STATUS& GetServiceStatus() { return m_serviceStatus; }
	void SetServiceStatus(DWORD dwStatus);
	BOOL CreateServiceStopEvent();
	BOOL SetServiceStatusHandle(SERVICE_STATUS_HANDLE handle);
	HANDLE& GetServiceStopEvent() { return m_serviceStopEvent; }
	void SetServiceStopEvent() { SetEvent(m_serviceStopEvent); }

	SERVICE_STATUS m_serviceStatus;
	SERVICE_STATUS_HANDLE m_serviceStatusHandle = NULL;
	HANDLE m_serviceHandle = INVALID_HANDLE_VALUE;
	HANDLE m_serviceStopEvent = INVALID_HANDLE_VALUE;
};

MyService& MyServiceInstance();