#include "MyService.h"
#include "Main.h"


VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
VOID WINAPI ServiceCtrlHandler(DWORD ctrlCode);

int _tmain(int argc, TCHAR *argv[])
{
	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{ ServiceNameStr, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};

	if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
	{
		return GetLastError();
	}

	return 0;
}

DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
	//  Periodically check if the service has been requested to stop
	while (WaitForSingleObject(MyServiceInstance().GetServiceStopEvent(), 0) != WAIT_OBJECT_0)
	{
		/*
		* Perform main service function here
		*/

		DebugPrint(_T("The Thread is Running!"));// prints to DbgView
		//  Simulate some work by sleeping
		Sleep(3000);
	}

	return ERROR_SUCCESS;
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
{
	DWORD Status = E_FAIL;

	MyService& mySvc = MyServiceInstance();

	// Register our service control handler with the SCM
	if (mySvc.SetServiceStatusHandle(RegisterServiceCtrlHandler(ServiceNameStr, ServiceCtrlHandler)) == FALSE) {
		DebugPrint("RegisterServiceCtrlHandler Failed");
		goto EXIT;
	}

	mySvc.SetServiceStatus(SERVICE_START_PENDING);

	/*
	* Perform tasks necessary to start the service here
	*/

	// Create a service stop event to wait on later
	
	if (mySvc.CreateServiceStopEvent() == FALSE)
	{
		DebugPrint("Failed to create service stop event");
		// Error creating event
		// Tell service controller we are stopped and exit
		mySvc.SetServiceStatus(SERVICE_STOPPED);
		goto EXIT;
	}

	mySvc.SetServiceStatus(SERVICE_RUNNING);

	{
		// Start a thread that will perform the main task of the service
		HANDLE h = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);
		std::shared_ptr<void> autoHandle(h, &::CloseHandle);

		// Wait until our worker thread exits signaling that the service needs to stop
		WaitForSingleObject(h, INFINITE);
	}
	/*
	* Perform any cleanup tasks
	*/

	// Tell the service controller we are stopped
	mySvc.SetServiceStatus(SERVICE_STOPPED);

EXIT:
	DebugPrint("Main thread exited");
	return;
}

VOID WINAPI ServiceCtrlHandler(DWORD ctrlCode)
{
	DebugPrint2("Service Control Handler called with code: ", ctrlCode);

	switch (ctrlCode)
	{
	case SERVICE_CONTROL_STOP:

		if (MyServiceInstance().GetServiceStatus().dwCurrentState != SERVICE_RUNNING) {
			DebugPrint("Service is not in running mode!");
			break;
		}

		/*
		* Perform tasks necessary to stop the service here
		*/
		MyServiceInstance().SetServiceStatus(SERVICE_STOP_PENDING);

		// This will signal the worker thread to start shutting down
		MyServiceInstance().SetServiceStopEvent();
		
		break;

	default:
		break;
	}
}