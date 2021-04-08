
#include "Control.h"
#include <iostream>
#include <tchar.h>
#include <strsafe.h>
#include <Windows.h>
#include <atlstr.h>
#pragma comment(lib, "advapi32.lib")

using namespace std;

//*********************Service Control ******************************
void ServiceEnable() {

   cout << "Starting this service..." << endl;

   //Local variables
   BOOL bStartService = FALSE;
   SC_HANDLE schOpenSCManager = NULL;
   SC_HANDLE schService = NULL;
   SERVICE_STATUS_PROCESS SVCStatusProcess;
   BOOL bQueryServiceStatus = FALSE;
   DWORD dwBytes;

   //Service input name
   wstring myInput = L"";


   //1.) Open the SCM

   schOpenSCManager = OpenSCManager(
      NULL,
      NULL,
      SC_MANAGER_ALL_ACCESS);
   if (schOpenSCManager == NULL) {

      cout << "OpenSCManager Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
   }
   else {
      cout << "Opening the SCManager was a Success" << endl;
   }

   //2.) Open the service API

   cout << "Please enter the name of the service you'd like to start: ";
   wcin >> myInput;
   LPCWSTR input = myInput.c_str();

   schService = OpenService(
      schOpenSCManager,
      input,
      SC_MANAGER_ALL_ACCESS);
   if (schService == NULL) {
      cout << "OpenService Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
      CloseServiceHandle(schOpenSCManager);
   }
   else {
      cout << "OpenService Success..." << endl;
   }

   //3.) Check the status of the service before starting
   bQueryServiceStatus = QueryServiceStatusEx(
      schService,
      SC_STATUS_PROCESS_INFO,
      (LPBYTE)&SVCStatusProcess,
      sizeof(SERVICE_STATUS_PROCESS),
      &dwBytes);

   if (bQueryServiceStatus == FALSE) {
      cout << "QueryService Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
   }
   else {
      cout << "Querying Service Succeeded..." << endl;
   }

   //.4)Check and see if the service is running or stopped
   if ((SVCStatusProcess.dwCurrentState != SERVICE_STOPPED) &&
      (SVCStatusProcess.dwCurrentState != SERVICE_STOP_PENDING)) {

      cout << "The service may be already running.." << endl;
      CloseServiceHandle(schService);
      CloseServiceHandle(schOpenSCManager);
      return;

   }

   //If more complex, may need timers in order to prevent
   //race conditions for retriving certain resources

   //5.) Query the service if its stopped
   while (SVCStatusProcess.dwCurrentState == SERVICE_STOP_PENDING) {
      bQueryServiceStatus = QueryServiceStatusEx(
         schService,
         SC_STATUS_PROCESS_INFO,
         (LPBYTE)&SVCStatusProcess,
         sizeof(SERVICE_STATUS_PROCESS),
         &dwBytes);

      if (bQueryServiceStatus == FALSE) {
         cout << "QueryService Failed" << endl;
         cout << "Error No = " << GetLastError() << endl;
         CloseServiceHandle(schService);
         CloseServiceHandle(schOpenSCManager);
      }
      else {
         cout << "Querying Service Succeeded..." << endl;
      }

   }

   //.6) Starting the service
   bStartService = StartService(
      schService, //handle to service
      0,          //# of args            
      NULL);      //no arguements
   if (bStartService == FALSE) {

      cout << "The Service Start Failed here.. " << endl;
      cout << "Error No = " << GetLastError() << endl;
      CloseServiceHandle(schService);
      CloseServiceHandle(schOpenSCManager);
   }
   else {
      cout << "Service Start Pending.." << endl;
   }

   //7.) Check the status again for verification
   bQueryServiceStatus = QueryServiceStatusEx(
      schService,
      SC_STATUS_PROCESS_INFO,
      (LPBYTE)&SVCStatusProcess,
      sizeof(SERVICE_STATUS_PROCESS),
      &dwBytes);

   if (bQueryServiceStatus == FALSE) {
      cout << "Querying Service Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
   }
   else {
      cout << "Querying Service Succeeded..." << endl;
   }

   //.4)Check and see if the service is running or stopped
   if ((SVCStatusProcess.dwCurrentState != SERVICE_STOPPED) &&
      (SVCStatusProcess.dwCurrentState != SERVICE_STOP_PENDING)) {

      cout << "The service may be already running.." << endl;
      CloseServiceHandle(schService);
      CloseServiceHandle(schOpenSCManager);
      return;
   }

   //8.) Check if its finally running 
   if (SVCStatusProcess.dwCurrentState == SERVICE_RUNNING) {
      cout << "Service is finally running.." << endl;
   }
   else {
      cout << "Service Running Failed" << endl;
      cout << "Error No = " << GetLastError() << endl;
      CloseServiceHandle(schService);
      CloseServiceHandle(schOpenSCManager);
   }
   //9.) Closehandles
   CloseServiceHandle(schService);
   CloseServiceHandle(schOpenSCManager);
}


//Objective - Stop a running service
//Manual - on deman

void ServiceDisable() {

   cout << "Stopping this service..." << endl;
   //Local variables

   BOOL StartService = FALSE;
   SC_HANDLE schOpenSCManager = NULL;
   SC_HANDLE schService = NULL;
   SERVICE_STATUS_PROCESS SVCStatusProcess;
   BOOL bQueryServiceStatus = TRUE;
   BOOL bControlService = TRUE;
   DWORD dwBytes;
   //Service input name
   wstring myInput = L"";

   //1.) Open the SCM

   schOpenSCManager = OpenSCManager(
      NULL,
      NULL,
      SC_MANAGER_ALL_ACCESS);
   if (schOpenSCManager == NULL)
   {
      cout << "OpenSCManager Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
   }
   else {
      cout << "Opening the SCManager was a Success" << endl;
   }

   //2.) Open the service API
   cout << "Please enter the name of the service you'd like to stop: ";
   wcin >> myInput;
   LPCWSTR input = myInput.c_str();

   schService = OpenService(
      schOpenSCManager,
      input,
      SC_MANAGER_ALL_ACCESS);
   if (schService == NULL) {
      cout << "OpenService Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
      CloseServiceHandle(schOpenSCManager);
   }
   else {
      cout << "OpenService Success..." << endl;
   }


   //3.) Check the status of the service before starting
   bQueryServiceStatus = QueryServiceStatusEx(
      schService,
      SC_STATUS_PROCESS_INFO,
      (LPBYTE)&SVCStatusProcess,
      sizeof(SERVICE_STATUS_PROCESS),
      &dwBytes);

   if (bQueryServiceStatus == FALSE) {
      cout << "QueryService Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
   }
   else {
      cout << "Querying Service Succeeded..." << endl;
   }

   //4.) Send a stop code to the SCM
   bControlService = ControlService(
      schService,
      SERVICE_CONTROL_STOP, //Sets the stop
      (LPSERVICE_STATUS)&SVCStatusProcess);

   if (bControlService == TRUE)
   {
      cout << "Control Service Succeeded" << endl;

   }
   else {

      cout << "Control Service has failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
      CloseServiceHandle(schService);
      CloseServiceHandle(schOpenSCManager);
   }
   //5.) Keep checking while service hasnt stopped yet
   while (SVCStatusProcess.dwCurrentState != SERVICE_STOPPED) {

      //Continue to query service as before
      bQueryServiceStatus = QueryServiceStatusEx(
         schService,
         SC_STATUS_PROCESS_INFO,
         (LPBYTE)&SVCStatusProcess,
         sizeof(SERVICE_STATUS_PROCESS),
         &dwBytes);

      if (bQueryServiceStatus == FALSE) {
         cout << "QueryService Failed " << endl;
         cout << "Error No = " << GetLastError() << endl;
      }
      else {
         cout << "Querying Service Succeeded..." << endl;
      }

      //6.) Check to see if service has stopped yet again
      if (SVCStatusProcess.dwCurrentState == SERVICE_STOPPED) {

         cout << "Your service has successfully stopped.." << endl;
         break; //Exit while loop

      }
      else {

         cout << "Service didnt stop correctly" << endl;
         cout << "Error No = " << GetLastError() << endl;
         CloseServiceHandle(schService);
         CloseServiceHandle(schOpenSCManager);

      }

   }

   //7.) Close handles once again
   CloseServiceHandle(schService);
   CloseServiceHandle(schOpenSCManager);
}

//Objective - Query service configuration and display to user general makeup
//Obtain:
   //Type
   //StartType
   //Errorcontrol
   //BinaryPath
   //Account

void ServiceQuery() {

   SC_HANDLE schOpenSCManager = NULL;
   SC_HANDLE schService = NULL;
   BOOL bQueryServiceConfig = FALSE;
   BOOL bQueryServiceConfig2 = FALSE;
   LPQUERY_SERVICE_CONFIG lpsvcConfig = NULL;
   LPSERVICE_DESCRIPTION lpsvcDscrpt = NULL;
   DWORD pcBytesNeeded, cbBufSize;


   //1.) Open the SCM

   schOpenSCManager = OpenSCManager(
      NULL,
      NULL,
      SC_MANAGER_ALL_ACCESS);
   if (schOpenSCManager == NULL) {

      cout << "OpenSCManager Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
   }
   else {
      cout << "Opening the SCManager was a Success" << endl;
   }

   //2.) Open the service API
   cout << "Please enter the service you'd like to query: ";
   wstring myInput = L"";
   wcin >> myInput;
   LPCWSTR input = myInput.c_str();

   schService = OpenService(
      schOpenSCManager,
      input,
      SERVICE_QUERY_CONFIG);
   if (schService == NULL) {
      cout << "OpenService Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
      CloseServiceHandle(schOpenSCManager);
   }
   else {
      cout << "OpenService Success..." << endl;
   }

   //3.) Query the service config info
   bQueryServiceConfig = QueryServiceConfig(
      schService,
      NULL,
      0,
      &pcBytesNeeded);

   if (bQueryServiceConfig == FALSE) {

      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {

         cbBufSize = pcBytesNeeded; //Specify the amount of bytes needed to retrieve config info
         lpsvcConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LMEM_FIXED, cbBufSize);
      }
      else {
         cout << "Error No = " << GetLastError() << endl;
         goto cleanup;
      }

   }
   else {
      cout << "Querying Service Succeeded..." << endl;
   }

   bQueryServiceConfig = QueryServiceConfig(
      schService,
      lpsvcConfig,
      cbBufSize,
      &pcBytesNeeded);

   if (bQueryServiceConfig == FALSE) {
      cout << "Querying Service Config Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
      goto cleanup;
   }



   //4.) Get config description info
   bQueryServiceConfig2 = QueryServiceConfig2(
      schService,
      SERVICE_CONFIG_DESCRIPTION,
      NULL,
      0,
      &pcBytesNeeded);

   if (bQueryServiceConfig2 == FALSE) {
      cout << "Querying Service Config attempted...reconfiguring" << endl;

      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
         cbBufSize = pcBytesNeeded; //Specify the amount of bytes needed to retrieve config info
         lpsvcDscrpt = (LPSERVICE_DESCRIPTION)LocalAlloc(LMEM_FIXED, cbBufSize);
      }
      else {
         cout << "Error No = " << GetLastError() << endl;
         goto cleanup;
      }

   }
   else {
      cout << "Querying Service Succeeded..." << endl;
   }

   bQueryServiceConfig2 = QueryServiceConfig2(
      schService,
      SERVICE_CONFIG_DESCRIPTION,
      (LPBYTE)lpsvcDscrpt,
      cbBufSize,
      &pcBytesNeeded);

   if (bQueryServiceConfig == FALSE) {
      cout << "Querying Service Config Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
      goto cleanup;
   }


   //5.) Print config and description information
   _tprintf(TEXT("%s configuration: \n"), input);
   _tprintf(TEXT("  Type: 0x%x\n"), lpsvcConfig->dwServiceType);
   _tprintf(TEXT("  Start Type: 0x%x\n"), lpsvcConfig->dwStartType);
   _tprintf(TEXT("  Error Control: 0x%x\n"), lpsvcConfig->dwErrorControl);
   _tprintf(TEXT("  Binary path: %s\n"), lpsvcConfig->lpBinaryPathName);
   _tprintf(TEXT("  Account: %s\n"), lpsvcConfig->lpServiceStartName);


   if (lpsvcDscrpt->lpDescription != NULL && lstrcmp(lpsvcDscrpt->lpDescription, TEXT("")) != 0)
      _tprintf(TEXT("  Description: %s\n"), lpsvcDscrpt->lpDescription);
   if (lpsvcConfig->lpLoadOrderGroup != NULL && lstrcmp(lpsvcConfig->lpLoadOrderGroup, TEXT("")) != 0)
      _tprintf(TEXT("  Load order group: %s\n"), lpsvcConfig->lpLoadOrderGroup);
   if (lpsvcConfig->dwTagId != 0)
      _tprintf(TEXT("  Tag ID: %d\n"), lpsvcConfig->dwTagId);
   if (lpsvcConfig->lpDependencies != NULL && lstrcmp(lpsvcConfig->lpDependencies, TEXT("")) != 0)
      _tprintf(TEXT("  Dependencies: %s\n"), lpsvcConfig->lpDependencies);

   LocalFree(lpsvcConfig);
   LocalFree(lpsvcDscrpt);

cleanup:
   CloseServiceHandle(schService);
   CloseServiceHandle(schOpenSCManager);

}

//Objective- Query the service description and print to console
void ServiceDescription() {

   SC_HANDLE schOpenSCManager = NULL;
   SC_HANDLE schService = NULL;
   BOOL bQueryServiceConfig2 = FALSE;
   LPSERVICE_DESCRIPTION lpsvcDscrpt = NULL;
   DWORD pcBytesNeeded, cbBufSize;


   //1.) Open the SCM
   schOpenSCManager = OpenSCManager(
      NULL,
      NULL,
      SC_MANAGER_ALL_ACCESS);
   if (schOpenSCManager == NULL) {

      cout << "OpenSCManager Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
   }
   else {
      cout << "Opening the SCManager was a Success" << endl;
   }

   //2.) Open the service API
   cout << "Please enter the service name to retrieve the description: ";
   wstring myInput = L"";
   wcin >> myInput;
   LPCWSTR input = myInput.c_str();

   schService = OpenService(
      schOpenSCManager,
      input,
      SERVICE_QUERY_CONFIG);
   if (schService == NULL) {
      cout << "OpenService Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
      CloseServiceHandle(schOpenSCManager);
   }
   else {
      cout << "OpenService Success..." << endl;
   }

   //3.) Get config description info
   bQueryServiceConfig2 = QueryServiceConfig2(
      schService,
      SERVICE_CONFIG_DESCRIPTION,
      NULL,
      0,
      &pcBytesNeeded);

   if (bQueryServiceConfig2 == FALSE) {
      cout << "Querying Service Config has failed.. insufficent buffer " << endl;
      cout << "Attempting to allocate space...\n\n" << endl;

      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
         cbBufSize = pcBytesNeeded; //Specify the amount of bytes needed to retrieve config info
         lpsvcDscrpt = (LPSERVICE_DESCRIPTION)LocalAlloc(LMEM_FIXED, cbBufSize);
      }
      else {
         cout << "Error No = " << GetLastError() << endl;
         goto cleanup;
      }

   }
   else {
      cout << "Querying Service Succeeded..." << endl;
   }

   bQueryServiceConfig2 = QueryServiceConfig2(
      schService,
      SERVICE_CONFIG_DESCRIPTION,
      (LPBYTE)lpsvcDscrpt,
      cbBufSize,
      &pcBytesNeeded);

   if (bQueryServiceConfig2 == FALSE) {
      cout << "Querying Service Config Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
      goto cleanup;
   }


   if (lpsvcDscrpt->lpDescription != NULL && lstrcmp(lpsvcDscrpt->lpDescription, TEXT("")) != 0) {
      _tprintf(TEXT("Description: %s\n"), lpsvcDscrpt->lpDescription);//Print unicode data types

   }
   LocalFree(lpsvcDscrpt);

cleanup:
   CloseServiceHandle(schService);
   CloseServiceHandle(schOpenSCManager);

}


//*********************Still needs work - invalid handle or pointer error ***********************
//Objective - Specifies the failure option of a service dies
void ServiceFailure() {

   SC_HANDLE schOpenSCManager = NULL;
   SC_HANDLE schService = NULL;
   SERVICE_FAILURE_ACTIONS sfActions; //Service controller for failure actions


   //1.) Open the SCM
   schOpenSCManager = OpenSCManager(
      NULL,
      NULL,
      SC_MANAGER_ALL_ACCESS);
   if (schOpenSCManager == NULL) {

      cout << "OpenSCManager Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
   }
   else {
      cout << "Opening the SCManager was a Success" << endl;
   }

   //2.) Open the service API
   cout << "Please enter the service you'd like to set service failure for: ";
   wstring myInput = L"";
   wcin >> myInput;
   LPCWSTR input = myInput.c_str();

   schService = OpenService(
      schOpenSCManager,
      input,
      SERVICE_QUERY_CONFIG);
   if (schService == NULL) {
      cout << "OpenService Failed " << endl;
      cout << "Error No = " << GetLastError() << endl;
      CloseServiceHandle(schOpenSCManager);
   }
   else {
      cout << "OpenService Success..." << endl;
   }

   cout << "{rebootMsg - resetPeriod - command - action }" << endl;
   cout << "Please enter the failure option from choices above: " << endl;
   string option1 = "";
   cin >> option1;
   LPCSTR option = option1.c_str();

   if (lstrcmpiA(option, "rebootMsg") == 0) {

      const char* tmp = "Your service is the ultimate failure";
      size_t size = strlen(tmp) + 1;
      wchar_t* text = new wchar_t[size];
      size_t outSize;
      mbstowcs_s(&outSize, text, size, tmp, size - 1);// Need to convert to user input * **
                                                   //mbstowcs_s- Converts a sequence of multibyte characters to a 
                                                   //corresponding sequence of wide characters.

      sfActions.lpRebootMsg = text;

      if (!ChangeServiceConfig2(
         schService,                     // handle of service 
         SERVICE_CONFIG_FAILURE_ACTIONS, //The configuration information to be changed.
         &sfActions                      //A pointer to the new value to be set for the configuration information. 
      ))
      {
         cout << "ChangeServiceConfig2 failed...\n" << endl;
         cout << "Error No  - " << GetLastError() << endl;
         goto cleanup;
      }
      else {
         cout << "Service failure rebootMsg changed successfully.\n" << endl;
      }

   }
   else  if (lstrcmpiA(option, "resetPeriod") == 0) {

      sfActions.dwResetPeriod = 30; //Need to convert to user input ***

      if (!ChangeServiceConfig2(
         schService,                     // handle of service 
         SERVICE_CONFIG_FAILURE_ACTIONS, //The configuration information to be changed.
         &sfActions                      //A pointer to the new value to be set for the configuration information. 
      ))
      {
         cout << "ChangeServiceConfig2 failed...\n" << endl;
         cout << "Error No  - " << GetLastError() << endl;
         goto cleanup;
      }
      else {
         cout << "Service resetPeriod changed successfully.\n" << endl;
      }


   }
   else  if (lstrcmpiA(option, "command") == 0) {

      const char* tmp = " C:^\\Windows^\\System32^\\cmd.exe";
      size_t size = strlen(tmp) + 1;
      wchar_t* text = new wchar_t[size];
      size_t outSize;
      mbstowcs_s(&outSize, text, size, tmp, size - 1);// Need to convert to user input * **
                                                   //mbstowcs_s- Converts a sequence of multibyte characters to a 
                                                   //corresponding sequence of wide characters.
      sfActions.lpCommand = text;

      if (!ChangeServiceConfig2(
         schService,                     // handle of service 
         SERVICE_CONFIG_FAILURE_ACTIONS, //The configuration information to be changed.
         &sfActions                      //A pointer to the new value to be set for the configuration information. 
      ))
      {
         cout << "ChangeServiceConfig2 failed...\n" << endl;
         cout << "Error No  - " << GetLastError() << endl;
         goto cleanup;
      }
      else {
         cout << "Service failure command changed successfully.\n" << endl;
      }

   }
   else  if (lstrcmpiA(option, "action") == 0) {


      cout << "{none - reboot - restart(service) - command }" << endl;
      cout << "Please enter the action to take from choices above: " << endl;
      string option2 = "";
      cin >> option2;
      LPCSTR option3 = option2.c_str();

      if (lstrcmpiA(option3, "reboot") == 0) {
      
         SC_ACTION scActions[2]; // control variable

         if (!ChangeServiceConfig2(
            schService,        // handle of service 
            SERVICE_CONFIG_FAILURE_ACTIONS, //The configuration information to be changed.
            &scActions //A pointer to the new value to be set for the configuration information. 
         ))
         {
            cout << "ChangeServiceConfig2 failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
            goto cleanup;
         }
         else {
            cout << "Service failure reboot option changed successfully.\n" << endl;
         }
      }
      else if (lstrcmpiA(option3, "restart") == 0) {

         SC_ACTION scActions[1]; // control variable

         if (!ChangeServiceConfig2(
            schService,        // handle of service 
            SERVICE_CONFIG_FAILURE_ACTIONS, //The configuration information to be changed.
            &scActions   //A pointer to the new value to be set for the configuration information. 
         ))
         {
            cout << "ChangeServiceConfig2 failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
            goto cleanup;
         }
         else {
            cout << "Service failure reboot option changed successfully.\n" << endl;
         }

      }
      else if (lstrcmpiA(option3, "command") == 0) {

         SC_ACTION scActions[3]; // control variable

         if (!ChangeServiceConfig2(
            schService,        // handle of service 
            SERVICE_CONFIG_FAILURE_ACTIONS, //The configuration information to be changed.
            &scActions   //A pointer to the new value to be set for the configuration information. 
         ))
         {
            cout << "ChangeServiceConfig2 failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
            goto cleanup;
         }
         else {
            cout << "Service failure reboot option changed successfully.\n" << endl;
         }

      }
      else {

         SC_ACTION scActions{ SC_ACTION_NONE,10000};
         SERVICE_FAILURE_ACTIONS sfa = { 60,0,0,1,&scActions };

         if (!ChangeServiceConfig2(
            schService,        // handle of service 
            SERVICE_CONFIG_FAILURE_ACTIONS, //The configuration information to be changed.
            &sfa  //A pointer to the new value to be set for the configuration information. 
         ))
         {
            cout << "ChangeServiceConfig2 failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
            goto cleanup;
         }
         else {
            cout << "Service failure reboot option changed successfully.\n" << endl;
         }


      }
   }
cleanup:
   CloseServiceHandle(schService);
   CloseServiceHandle(schOpenSCManager);

}