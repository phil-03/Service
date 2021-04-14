# SC.EXE
Partial implementation of Windows SC.exe utility in C++


## NOTE 
It is best to run this program in a virutal machine. Any antivirus will need to be disabled and
Visual Studio must be run as admin.


## Installation
For regular install, just clone the cpp and header file to local directory. Compile and run as normal.
In VS, on the start window, select Clone a repository. Enter or type the repository location, and then select Clone.


### OPTIONS: 
* create       - Creates a subkey and entries for a service in the registry and in the Service Control Manager database. <br />
* qdescription - Displays the description string for a specified service. <br />
* start        - Starts a service. <br />
* stop         - Stops a service. <br />
* delete       - Deletes a service subkey from the registry. <br />


### Additional Option & Arguments:
* query        - Obtains and displays information about the specified service <br />

* config       - Modifies the value of a service's entries in the registry and in the Service Control Manager database. <br />
                    - start type = {auto | manual | disabled} <br />

* failure(TBD) - Specifies one or more actions to take if a service fails. <br />


### USAGE EXAMPLE: 

>Program is run with standard input instead of commandline. Simple run as is. Demonstrates walkthrough of options as you progress.


### Future TODOS:
- Switch to command line usage
- Fix ServiceFailure func.
- Add more function capabilities
