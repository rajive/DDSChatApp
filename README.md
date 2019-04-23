# ChatApp

Building a peer-to-peer Chat application using
[DDS](http://portals.omg.org/dds), the data connectivity framework for
Internet of Things (IoT).

The examples show evolution of datatypes versions as well as multiple 
component implementations for a given interface.

The common aspects are described below.


## Organization

Each version is organized according to the following directory
structure.

- `doc/<ver>/`
   - a `README.md` for the version `<ver>` 

- `if/<ver>/`
  - the component interfaces: data-oriented interfaces (DDS-XML files)
     
- `res/types/<ver>/`
   - the datatype definitions (IDL files)
    
- `res/qos/<ver>/`
   - the qos profiles (DDS-XML files)

- `src/<lang>/<ver>/<variant>/`
  - a component implementation in the programming language of choice
    (e.g. C++, C#, Lua, ...)
  - in the instructions below, `impl_xyz` is a placeholder for one of these


## Prerequisites

- [RTI Connext DDS 5.3.x or higher](http://www.rti.com/downloads/index.html)


## Generated Examples as Baseline

The starting point for the examples is the generated example code from the IDL
file(s) in the [res/](./res/) directory.

Modifications to the generated code are marked with for code blocks as follows.

    /* >>> begin */
        :
    /* <<< end */

Changes to an item within a single line are marked as follows.

    /*>>><<<*/

The generated example code serves as the baseline to which the above
modifications are applied.

## Building the components

The top-level makefile can be used to recursively build the components 
in the repository as follows.

- Determine your platform
   
  For, example on Mac OSX:
  
        export PLATFORM=x64Darwin17clang9.0 
             
- Generate the makefile or solutions for the platform, e.g.

        make makefile/$PLATFORM
        
   For windows platform, a visual studio solution is generated.
        
 - Clean old artifacts
    
        make clean
 
 - Ensure that the NDDSHOME is set to the *RTI Connext DDS* install directory, 
   e.g.

        export NDDSHOME=/opt/rti/NDDSHOME
        
 - [Re]Build the artifacts

        make
        
 
Alternatively, the steps can be implemented one at a time as follows.
 

### Generating the XML representation of the datatypes from IDL 

- Run RTI Connext Launcher > Utilities > Type Convert

            Input file: res/types/<ver>/Chat.idl
            Output directory: src/<lang>/<ver>/<variant>/

- OR (non-Windows), simply use the makefile in the `res/types/<ver>/` directory:
 
            cd res/types/<ver>/Chat.idl
            make

            
### Generating the datatype handling code for a target language

- Run RTI Connext Launcher > Utilities > Code Generator

           Input file: res/types/<ver>/Chat.idl
           Output directory: src/<lang>/<ver>/<variant>/
           
           Language: Traditional C++ (namespaces)   
             -language C++ -unboundedSupport  -namespace -useStdString    
                           
           Example files: <disable>
           Type files: update
           Makefiles: update
           
		  Confirm command: Y
		  Run
            
  - On the command line
    - copy and paste the generated command line into a terminal window
    - append any additional arguments, e.g. ** -useStdString **
    - execute
   
           rtiddsgen -ppDisable -language C++ -unboundedSupport -namespace -useStdString -platform $PLATFORM -updatetypefiles -update makefiles -update examplefiles -d src/cpp res/types/Chat.idl 
       
       
-  OR, simply use the makefile:

          cd src/<lang>/<ver>/<variant>/
          make Chat.h



### Building the components

- Open Terminal

        RTI Connext Launcher > Utilities > Code Generator > Open Terminal

- Do the following steps in that terminal window.

- Switch to the component directory:

		cd src/<lang>/<ver>/<variant>/

- Build the C++ code

  - Linux | MacOS

            make

  - Windows

            Open Visual Studio Solution

## Running the components (any language)


- Switch to the top-level working directory

    cd <root-of-this-repository>
     
- Define the list of DDS-XML files to be loaded via the `NDDS_QOS_PROFILES` 
  environment variable   
  
  - QoS profiles
  
        export NDDS_QOS_PROFILES="res/qos/micro/Chat_qos.xml"
        
  - XML App Creation
  
        export NDDS_QOS_PROFILES="res/qos/micro/Chat_qos.xml;if/micro/Chat.xml"
                    

### Running the C++ components

- Run Subscriber Example:

        ./src/<lang>/<ver>/<variant>/objs/$PLATFORM/Chat_subscriber
    
- Run Publisher Example:
  
        ./src/<lang>/<ver>/<variant>/objs/$PLATFORM/Chat_publisher


### Running the C# components

Replace `Debug-VS2010` with your target
   
- Run Subscriber Example:

        ./src/<lang>/<ver>/<variant>/bin/Debug-VS2010/Chat_subscriber.exe

   
- Run Publisher Example:

		./src/<lang>/<ver>/<variant>/bin/Debug-VS2010/Chat_publisher.exe

   
  
### Running the Lua components

- Run Subscriber Example:

        rtiddsprototyper -cfgName ChatIfLib::Chat_Sub -luaFile src/lua/Chat_subscriber.lua
 
- Run Publisher Example:

        rtiddsprototyper -cfgName ChatIfLib::Chat_Pub -luaFile src/lua/Chat_publisher.lua

        
### Running the Web Integration Service components

- [svc/web/README.md](./svc/web/README.md)
