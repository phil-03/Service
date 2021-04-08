#include "Config.h"
#include <iostream>
#include <tchar.h>
#include <strsafe.h>
#include <Windows.h>
#include <atlstr.h>
#pragma comment(lib, "advapi32.lib")

using namespace std;


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