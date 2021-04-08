/*
Name: Philip B.
Last Modified: 4/8/21
Description: Mainly outlines sc.exe implementation - see README

*/

#include <iostream>
#include <tchar.h>
#include <strsafe.h>
#include <Windows.h>
#include <atlstr.h>
#include "Control.h"
#pragma comment(lib, "advapi32.lib") 

using namespace std;

//=======================Global Variables==========================//
#define SVCNAME TEXT("NEW-SERVICE")
SERVICE_STATUS gServiceStatus = { 0 }; //Status Structure
SERVICE_STATUS_HANDLE hServiceStatusHandle = NULL; //Service status handle for registry
HANDLE hSvcEvent = NULL;

//=========================FUNCTIONS================================//

void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpArgv);// Service Main Function

void WINAPI ServiceControlHandler(DWORD dwControl);// Service Control Handler

void ServiceReportStatus(
   DWORD dwCurrentState,
   DWORD dwWin32ExitCode,
   DWORD dwWaitHint); // Service Report Status

void ServiceInit(DWORD dwArgc, LPTSTR* lpArgv); //Initialization or configure


int main(int argc, CHAR* argv[]) { //Will implement as command line later

   cout << "Here are a list of modes: " << endl;
   cout << "Create" << endl;
   cout << "Start" << endl;
   cout << "Stop" << endl;
   cout << "Delete" << endl;
   cout << "Qdescription" << endl;
   cout << "Query" << endl;
   cout << "Config - {Has additional Options}" << endl;
   cout << "Failure" << endl;

   cout << "Please choose an option above: ";

   string myInput = "";
   cin >> myInput;
   LPCSTR input = myInput.c_str(); //Convert string to LPCSTR

   BOOL StrTServiceCtrlDispatcher = FALSE; //Local dispatcher control - will help connect our main program and Service Main to main thread-SCM

   //Diffent options
   if (lstrcmpiA(input, "Create") == 0) {

      //Call to Service Installation
      ServiceCreate();
      cout << "Installation Success" << endl;

   }
   else if (lstrcmpiA(input, "Start") == 0) {

      //Call to service start
      ServiceEnable();
      cout << "Service Enable function finished.." << endl;

   }
   else if (lstrcmpiA(input, "Stop") == 0) {

      //Call to ServiceDisable
      ServiceDisable();
      cout << "Service Disable function finished..." << endl;

   }
   else if (lstrcmpiA(input, "Delete") == 0) {

      //Call to ServiceDelete
      ServiceDelete();
      cout << "Service will now delete.." << endl;

   }
   else if (lstrcmpiA(input, "Qdescription") == 0) {

      //Call to ServiceDescription
      ServiceDescription();
      cout << "Service has description has been displayed..." << endl;

   }
   else if (lstrcmpiA(input, "Query") == 0) {

      //Call to ServiceQuery
      cout << "Will now display queried response..." << endl;
      ServiceQuery();
   }
   else if (lstrcmpiA(input, "Config") == 0) {

      //Call to ServiceConfig
      cout << "Configuration process has started..." << endl;
      ServiceConfig();

   }
   else if (lstrcmpiA(input, "Failure") == 0) {

      //Call to ServiceFailure
      cout << "Here are some options for service failure: " << endl;
      ServiceFailure();
      

   }
   else {


      //Step 1 - Add additional services to array
      SERVICE_TABLE_ENTRYW DispatchTable[] =
      {
        { (LPWSTR)input,(LPSERVICE_MAIN_FUNCTION)ServiceMain},
        { NULL, NULL }
      };

      // This call returns when the service has stopped. 
      // The process should simply terminate when the call returns.
      //Step 2 - Start the service control dispatcher
      StrTServiceCtrlDispatcher = StartServiceCtrlDispatcherW(DispatchTable);

      if (StrTServiceCtrlDispatcher == FALSE)
      {
         cout << "StartServiceCtrlDispatcher Failed" << endl;
         cout << "Error Code - " << GetLastError() << endl;
      }
      else {

         cout << "Start Service Ctrl Dispatcher Success!" << endl;
      }

   }

   system("PAUSE"); //Necessary?

   return 0;

}

//=======================Service Program=========================//


void ServiceInit(DWORD dwArgc, LPTSTR* lpArgv)
{
   // If initialization fails, call
   //   ReportSvcStatus with SERVICE_STOPPED.

   cout << "Starting Service Initialization" << endl;

   // 1.) Create an event. The control handler function, SvcCtrlHandler,
   // signals this event when it receives the stop control code.

   hSvcEvent = CreateEvent(
      NULL,    // security attributes
      TRUE,    // manual reset event
      FALSE,   // not signaled
      NULL);   // no name

   if (hSvcEvent == NULL)
   {
      //Call to service report status
      ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
      return;
   }

   // Report running status when initialization is complete.

   ServiceReportStatus(SERVICE_RUNNING, NO_ERROR, 0);

   // 2.) Check whether to stop Service.

   while (1) {

      WaitForSingleObject(hSvcEvent, INFINITE);
      //Send the report to the SCM
      ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
      return;
   }
}


//Objective -Entry Point for our service
//params:
//	-dwArgc - number of arguements in the lpszArgv array
//	-lspzArgv - the array of strings
//
//Return value - none

void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpArgv) {

   //Local variables 
   BOOL bServiceStatus = FALSE;

   //1.) Register the handler function for the service

   hServiceStatusHandle = RegisterServiceCtrlHandler(
      SVCNAME,
      ServiceControlHandler);

   if (hServiceStatusHandle == NULL) {

      cout << "Registering Service Control Handler Failed..." << endl;
      cout << "Error Code -> " << GetLastError() << endl;

   }
   else {
      cout << "Reigistration of Ctrl Handler Succeeded" << endl;
   }

   // These SERVICE_STATUS members remain as set here
   //2.) Initial service setup
   gServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; //Service Own Process - A service type flag that indicates a 
   gServiceStatus.dwServiceSpecificExitCode = 0;             //Win32 service that runs in its own process

   // 3.) Call that reports initial status to the SCM

   ServiceReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

   // 4.) Add a Check for the status of the service
   bServiceStatus = SetServiceStatus(
      hServiceStatusHandle,
      &gServiceStatus
   );
   if (bServiceStatus == FALSE) {

      cout << "Service Status check failed to setup with - " << endl;
      cout << GetLastError() << endl;

   }
   else {

      cout << "Service Status setup Succeeded" << endl;
   }


   // Perform service-specific initialization and work.
   //Call to Service Init function
   ServiceInit(dwArgc, lpArgv);

}

//Objective - Sets the current report status and send it to the SCM
//Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation, 
//   n milliseconds

void ServiceReportStatus(DWORD dwCurrentState,
   DWORD dwWin32ExitCode,
   DWORD dwWaitHint) {

   cout << "Starting report status" << endl;
   //local variable
   static DWORD dwCheckPoint = 1;
   BOOL bSetServiceStatus = FALSE;
   //1.) Add service status
   gServiceStatus.dwCurrentState = dwCurrentState;
   gServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
   gServiceStatus.dwWaitHint = dwWaitHint;

   //2.) Check the current state
   if (dwCurrentState == SERVICE_START_PENDING) {

      gServiceStatus.dwControlsAccepted = 0;
   }
   else {

      gServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
   }

   //3.) Service Operation
   if ((dwCurrentState == SERVICE_RUNNING) ||
      (dwCurrentState == SERVICE_STOPPED))
   {
      gServiceStatus.dwCheckPoint = 0;
   }
   else {
      gServiceStatus.dwCheckPoint = dwCheckPoint++;
   }
   //.4) Inform SCM of status

   bSetServiceStatus = SetServiceStatus(
      hServiceStatusHandle,
      &gServiceStatus);

   if (FALSE == bSetServiceStatus) {

      cout << "Service Status update failed... " << endl;
      cout << "Error No - " << GetLastError() << endl;
   }
   else {

      cout << "Service Status has Succeeded.." << endl;
   }

}


//  Objective - Called by SCM whenever a control code is sent to the service
//  using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
// 
void WINAPI ServiceControlHandler(DWORD dwControl)
{
   cout << "Starting the Service Control Handler" << endl;

   switch (dwControl)
   {
   case SERVICE_CONTROL_STOP:
   {
      //Call ServiceReportStatus Function
      ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
      cout << "Service stopped" << endl;
      break;
   }
   default:
      break;
   }//Switch
   cout << "Service Control Handler Ended" << endl;

}


//=======================Service Configuration==================//
//Objective - Installs our service in SCM database
//Steps:
  //USe GetModuleFilename API for file path check
  //OpenSCManeger
  //Create Service
  //Close Handle for SCM and Create Service

void ServiceCreate() {

   cout << " Creating New Service!" << endl;

   SC_HANDLE schOpenSCManager = NULL;
   SC_HANDLE schCreateService = NULL;

   DWORD dwGetModuleFileName = 0;
   TCHAR szPath[MAX_PATH];

   //1.) Get the file
   dwGetModuleFileName = GetModuleFileName(
      NULL,
      szPath,
      MAX_PATH
   );

   if (dwGetModuleFileName == 0) {

      cout << "Failed to Install... " << endl;
      cout << "Error No  - " << GetLastError() << endl;
   }
   else {
      cout << "Install was a success for your File\n" << endl;

   }

   //2.) Open SCM
   schOpenSCManager = OpenSCManager(
      NULL,//Local Machine
      NULL,
      SC_MANAGER_ALL_ACCESS//Gives access rights
   );

   if (schOpenSCManager == NULL) {

      cout << "Failed to Open SCM..." << endl;
      cout << "Error No  - " << GetLastError() << endl;
   }
   else {
      cout << "Opening SCM Success...\n" << endl;

   }

   //3.) Create the Service
   schCreateService = CreateService(
      schOpenSCManager,//SCM database handle
      SVCNAME,//Service's Name
      SVCNAME,//Display Name
      SERVICE_ALL_ACCESS,//Desired access
      SERVICE_WIN32_OWN_PROCESS,//Service Type
      SERVICE_DEMAND_START,//Service Start Type
      SERVICE_ERROR_NORMAL,//Service Error Code
      szPath,//Service Path
      NULL,
      NULL,
      NULL,
      NULL,
      NULL);

   if (NULL == schCreateService)
   {
      cout << "CreateService Failed " << endl;
      cout << "Error No - " << GetLastError() << endl;
      CloseServiceHandle(schOpenSCManager);
   }
   else {
      cout << "CreateService Success!" << endl;
   }

   //4.) Close your handles
   CloseServiceHandle(schCreateService);
   CloseServiceHandle(schOpenSCManager);

}



//Objective - Deletes our service from SCM
void ServiceDelete() {

   cout << " Starting the Delete Service Function!" << endl;

   SC_HANDLE schOpenSCManager = NULL;
   SC_HANDLE schOpenService = NULL;
   BOOL bDeleteService = FALSE;

   cout << "Please enter the service you'd like to delete: ";
   wstring myInput = L"";
   wcin >> myInput;
   LPCWSTR input = myInput.c_str();


   //1.) Open SCM
   schOpenSCManager = OpenSCManager(
      NULL,//Local Machine
      NULL,
      SC_MANAGER_ALL_ACCESS//Gives access rights
   );

   if (schOpenSCManager == NULL) {

      cout << "Failed to Open SCM..." << endl;
      cout << "Error No  - " << GetLastError() << endl;
   }
   else {
      cout << "Opening SCM Success...\n" << endl;

   }

   //2.) Open the actual service
   schOpenService = OpenService(
      schOpenSCManager,
      input,
      SERVICE_ALL_ACCESS);
   if (schOpenService == NULL)
   {
      cout << "Open this service failed " << endl;
      cout << "Error No- " << GetLastError() << endl;
   }
   else
   {
      cout << "Open this service succeeded... " << endl;
   }

   //3.)Delete the service
   bDeleteService = DeleteService(schOpenService);
   if (bDeleteService == FALSE) {
      cout << "Deleteing this service failed... " << endl;
      cout << "Error No- " << GetLastError() << endl;

   }
   else {

      cout << "Service deletion was a success..." << endl;
   }

   //4.) Close Handles
   CloseServiceHandle(schOpenService);
   CloseServiceHandle(schOpenSCManager);

}


//Objective - Specify path to binary service path 
//          - Change start option
//          - Specify error if service fails
//          - Change displayname 
//{May need to implement other start options}
void ServiceConfig() {

   SC_HANDLE schOpenSCManager = NULL;
   SC_HANDLE schService = NULL;

   //1.) Open SCM
   schOpenSCManager = OpenSCManager(
      NULL,//Local Machine
      NULL,
      SC_MANAGER_ALL_ACCESS//Gives access rights
   );

   if (schOpenSCManager == NULL) {

      cout << "Failed to Open SCM..." << endl;
      cout << "Error No  - " << GetLastError() << endl;
   }
   else {
      cout << "Opening SCM Success...\n" << endl;

   }

   //2.) Get a handle to the service

   cout << "Please enter the name of the service you'd like to configure: ";
   wstring myInput = L"";
   wcin >> myInput;
   LPCWSTR input = myInput.c_str();

   schService = OpenService(
      schOpenSCManager,            // SCM database 
      input,               // name of service 
      SERVICE_CHANGE_CONFIG);  // need change config access 

   if (schService == NULL)
   {
      cout << "Open Service failed... " << GetLastError() << endl;
      CloseServiceHandle(schOpenSCManager);
      return;
   }

   //3.) Let user choose method below.

   cout << "Please choose an option below \n\n" << endl;

   cout << "Start - {auto | manual | disabled}" << endl;
   cout << "Binpath= " << endl;
   cout << "Error - {normal | severe | critical | ignore}" << endl;
   cout << "Displayname= \n\n" << endl;

   cout << "Option: ";

   string myOption1 = "";
   cin >> myOption1;
   LPCSTR option1 = myOption1.c_str();


   //Diffent options
   if (lstrcmpiA(option1, "Start") == 0) {

      //call to modify start
      cout << "Modifying start option..." << endl;
      cout << "Please specify which start option from the selections above: ";


      string myOption2 = "";
      cin >> myOption2;
      LPCSTR option2 = myOption2.c_str();

      if (lstrcmpiA(option2, "auto") == 0) {

         if (!ChangeServiceConfig(
            schService,        // handle of service 
            SERVICE_NO_CHANGE, // service type: no change 
            SERVICE_AUTO_START,  // service start type 
            SERVICE_NO_CHANGE, // error control: no change 
            NULL,              // binary path: no change 
            NULL,              // load order group: no change 
            NULL,              // tag ID: no change 
            NULL,              // dependencies: no change 
            NULL,              // account name: no change 
            NULL,              // password: no change 
            NULL))            // display name: no change
         {
            cout << "ChangeServiceConfig failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
         }
         else {
            cout << "Service set to auto start successfully.\n" << endl;
         }

         CloseServiceHandle(schService);
         CloseServiceHandle(schOpenSCManager);

      }
      else if (lstrcmpiA(option2, "manual") == 0) {

         if (!ChangeServiceConfig(
            schService,        // handle of service 
            SERVICE_NO_CHANGE, // service type: no change 
            SERVICE_DEMAND_START,  // service start type 
            SERVICE_NO_CHANGE, // error control: no change 
            NULL,              // binary path: no change 
            NULL,              // load order group: no change 
            NULL,              // tag ID: no change 
            NULL,              // dependencies: no change 
            NULL,              // account name: no change 
            NULL,              // password: no change 
            NULL))            // display name: no change
         {
            cout << "ChangeServiceConfig failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
         }
         else {
            cout << "Service set to demand start successfully.\n" << endl;
         }

         CloseServiceHandle(schService);
         CloseServiceHandle(schOpenSCManager);

      }
      /*else if (lstrcmpiA(option2, "boot") == 0) {

         if (!ChangeServiceConfig(
            schService,        // handle of service
            SERVICE_NO_CHANGE, // service type: no change
            SERVICE_BOOT_START,  // service start type
            SERVICE_NO_CHANGE, // error control: no change
            NULL,              // binary path: no change
            NULL,              // load order group: no change
            NULL,              // tag ID: no change
            NULL,              // dependencies: no change
            NULL,              // account name: no change
            NULL,              // password: no change
            NULL))            // display name: no change
         {
            cout << "ChangeServiceConfig failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
         }
         else {
            cout << "Service set to boot start successfully.\n" << endl;
         }

         CloseServiceHandle(schService);
         CloseServiceHandle(schOpenSCManager);

      }
      else if (lstrcmpiA(option2, "system") == 0) {

         if (!ChangeServiceConfig(
            schService,        // handle of service
            SERVICE_NO_CHANGE, // service type: no change
            SERVICE_SYSTEM_START,  // service start type
            SERVICE_NO_CHANGE, // error control: no change
            NULL,              // binary path: no change
            NULL,              // load order group: no change
            NULL,              // tag ID: no change
            NULL,              // dependencies: no change
            NULL,              // account name: no change
            NULL,              // password: no change
            NULL))            // display name: no change
         {
            cout << "ChangeServiceConfig failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
         }
         else {
            cout << "Service set to system start successfully.\n" << endl;
         }

         CloseServiceHandle(schService);
         CloseServiceHandle(schOpenSCManager);

      }*/
      else if (lstrcmpiA(option2, "disabled") == 0) {

         if (!ChangeServiceConfig(
            schService,        // handle of service 
            SERVICE_NO_CHANGE, // service type: no change 
            SERVICE_DISABLED,  // service start type 
            SERVICE_NO_CHANGE, // error control: no change 
            NULL,              // binary path: no change 
            NULL,              // load order group: no change 
            NULL,              // tag ID: no change 
            NULL,              // dependencies: no change 
            NULL,              // account name: no change 
            NULL,              // password: no change 
            NULL))            // display name: no change
         {
            cout << "ChangeServiceConfig failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
         }
         else {
            cout << "Service set to system start successfully.\n" << endl;
         }

         CloseServiceHandle(schService);
         CloseServiceHandle(schOpenSCManager);

      }
      else {

         cout << "Sorry that option is not available..." << endl;
      }


   }
   else if (lstrcmpiA(option1, "Error") == 0) {

      //Call to specify error level upon boot
      cout << "Error options again- {normal | severe | critical | ignore}\n" << endl;
      cout << "Please choose the severity of error: ";
      string myError = "";
      cin >> myError;
      LPCSTR error = myError.c_str();

      if (lstrcmpiA(error, "normal") == 0) {

         if (!ChangeServiceConfig(
            schService,        // handle of service 
            SERVICE_NO_CHANGE, // service type: no change 
            SERVICE_NO_CHANGE,  // service start type 
            SERVICE_ERROR_NORMAL, // error control: no change 
            NULL,              // binary path: no change 
            NULL,              // load order group: no change 
            NULL,              // tag ID: no change 
            NULL,              // dependencies: no change 
            NULL,              // account name: no change 
            NULL,              // password: no change 
            NULL))            // display name: no change
         {
            cout << "ChangeServiceConfig failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
         }
         else {
            cout << "Service error set successfully.\n" << endl;
         }

      }
      else if (lstrcmpiA(error, "severe") == 0) {

         if (!ChangeServiceConfig(
            schService,        // handle of service 
            SERVICE_NO_CHANGE, // service type: no change 
            SERVICE_NO_CHANGE,  // service start type 
            SERVICE_ERROR_SEVERE, // error control: no change 
            NULL,              // binary path: no change 
            NULL,              // load order group: no change 
            NULL,              // tag ID: no change 
            NULL,              // dependencies: no change 
            NULL,              // account name: no change 
            NULL,              // password: no change 
            NULL))            // display name: no change
         {
            cout << "ChangeServiceConfig failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
         }
         else {
            cout << "Service error set successfully.\n" << endl;
         }

      }
      else if (lstrcmpiA(error, "critical") == 0) {

         if (!ChangeServiceConfig(
            schService,        // handle of service 
            SERVICE_NO_CHANGE, // service type: no change 
            SERVICE_NO_CHANGE,  // service start type 
            SERVICE_ERROR_CRITICAL, // error control: no change 
            NULL,              // binary path: no change 
            NULL,              // load order group: no change 
            NULL,              // tag ID: no change 
            NULL,              // dependencies: no change 
            NULL,              // account name: no change 
            NULL,              // password: no change 
            NULL))            // display name: no change
         {
            cout << "ChangeServiceConfig failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
         }
         else {
            cout << "Service error set successfully.\n" << endl;
         }

      }
      else if (lstrcmpiA(error, "ignore") == 0) {

         if (!ChangeServiceConfig(
            schService,        // handle of service 
            SERVICE_NO_CHANGE, // service type: no change 
            SERVICE_NO_CHANGE,  // service start type 
            SERVICE_ERROR_IGNORE, // error control: no change 
            NULL,              // binary path: no change 
            NULL,              // load order group: no change 
            NULL,              // tag ID: no change 
            NULL,              // dependencies: no change 
            NULL,              // account name: no change 
            NULL,              // password: no change 
            NULL))            // display name: no change
         {
            cout << "ChangeServiceConfig failed...\n" << endl;
            cout << "Error No  - " << GetLastError() << endl;
         }
         else {
            cout << "Service error set successfully.\n" << endl;
         }

      }
      else {

         cout << "Sorry that option is not available... please check spelling." << endl;

      }

   }
   else if (lstrcmpiA(option1, "Binpath=") == 0) {

      //Call to set binpath
      //Note - Example minus parens: (""d:\my share\myservice.exe"")  
      cout << "IMPORTANT - Please double quote if file path includes spaces!\n" << endl;
      cout << "Please enter the new binpath: ";

      wstring fileP = L"";
      wcin >> fileP;
      LPCWSTR path;
      path = fileP.c_str();

      if (!ChangeServiceConfig(
         schService,        // handle of service 
         SERVICE_NO_CHANGE, // service type: no change 
         SERVICE_NO_CHANGE,  // service start type 
         SERVICE_NO_CHANGE, // error control: no change 
         path,              // binary path: no change 
         NULL,              // load order group: no change 
         NULL,              // tag ID: no change 
         NULL,              // dependencies: no change 
         NULL,              // account name: no change 
         NULL,              // password: no change 
         NULL))            // display name: no change
      {
         cout << "ChangeServiceConfig failed...\n" << endl;
         cout << "Error No  - " << GetLastError() << endl;
      }
      else {
         cout << "Service bin path changed successfully.\n" << endl;
      }

   }
   else if (lstrcmpiA(option1, "Displayname=") == 0) {

      //Call to change display
      //Read in wstring for display change
      cout << "Please enter the new name: ";
      wstring name = L"";
      wcin >> name;
      LPCWSTR tmp;
      tmp = name.c_str();


      if (!ChangeServiceConfig(
         schService,        // handle of service 
         SERVICE_NO_CHANGE, // service type: no change 
         SERVICE_NO_CHANGE,  // service start type 
         SERVICE_NO_CHANGE, // error control: no change 
         NULL,              // binary path: no change 
         NULL,              // load order group: no change 
         NULL,              // tag ID: no change 
         NULL,              // dependencies: no change 
         NULL,              // account name: no change 
         NULL,              // password: no change 
         tmp))            // display name: no change
      {
         cout << "ChangeServiceConfig failed...\n" << endl;
         cout << "Error No  - " << GetLastError() << endl;
      }
      else {
         cout << "Service displayname changed successfully.\n" << endl;
      }

      CloseServiceHandle(schService);
      CloseServiceHandle(schOpenSCManager);//Call

   }
   else {

      cout << "Sorry that option doesn't exist... try again. \n" << endl;
   }


}

