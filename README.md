# SC.exe 
Implementation of Windows SC.exe utility in C++

NOTE -It is best to run this program in a virutal machine. Any antivirus will need to be disabled
Visual Studio must be run as admin.

>**Will update soon with installation notes and instruction on running program..

<b>PARAM OPTIONS:</b> <br />
* query -> Obtains and displays information about the specified service <br />
* create -> Creates a subkey and entries for a service in the registry and in the Service Control Manager database. <br />
* qdescription -> Displays the description string for a specified service. <br />
* start -> Starts a service. <br />
* stop -> Stops a service. <br />
* delete -> Deletes a service subkey from the registry. <br />
* config -> Modifies the value of a service's entries in the registry and in the Service Control Manager database. <br />
* failure -> Specifies one or more actions to take if a service fails. <br />


<b>USAGE EXAMPLE: </b><br />

>sc.exe config [servicename] [start= {auto | demand | disabled | delayed-auto}] [error= {normal | severe | critical | ignore}] [binpath= binarypathname] 

