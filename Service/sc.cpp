#include <iostream>
#include <tchar.h>
#include <strsafe.h>
#include <Windows.h>
#include <atlstr.h>
#include "Control.h"
#include "Config.h"
 

using namespace std;

//=======================Global Variables==========================//
#define SVCNAME TEXT("NEW-TEMP")
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
   cout << "Failure\n" << endl;

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
      //cout << "Here are some options for service failure: " << endl;
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


   return 0;

}

//=======================Service Program=========================//

//Intialization function of a service
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



