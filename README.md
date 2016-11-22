# ChatApp

Building a peer-to-peer Chat application using
[DDS](http://portals.omg.org/dds), the data connectivity framework for
Internet of Things (IoT).

Each sub-directory represents a step in the evolution of ChatApp. The
versions illustrate the progression from a basic app with a single conversation
that uses DDS simply as *messaging* transport; to a fully featured app
that uses the capabilities of DDS as a **data-centric connectivity framework**.

Each sub-directory has a README.md with more specific instructions. Below, the
common aspects are described.


## Organization

Each version of the component is organized according to the following directory
structure.

- `if/`
  - the component interface, comprising of
     - the datatype definitions (IDL files)
     - the data-oriented interface (XML files)
	 - use this as the **working directory to launch programs** so that the
	   `USER_QOS_PROFILES.xml` from the working directory gets loaded

- `impl_*/`
  - a component implementation in the programming language of choice
    (e.g. C++, C#, Lua, ...)
  - in the instructions below, `impl_xyz` is a placeholder for one of these

There can be multiple components of the same interface. Components are
launched from `if/` as the working directory. This ensures that the
implementation variants use the same interface settings.

## Prerequisites

- [RTI Connext DDS 5.2 +](http://www.rti.com/downloads/index.html)

## Generated Examples as Baseline

The starting point for the examples is the generated example code from the IDL
file(s) in the if/ directory.

Modifications to the generated code are marked with for code blocks as follows.

    /* >>> begin */
        :
    /* <<< end */

Changes to an item within a single line are marked as follows.

    /*>>><<<*/

The generated example code serves as the baseline to which the above
modifications are applied.


## Generating the datatype handling code

- e.g. C++ Datatypes: Run RTI Connext Launcher > Utilities > Code Generator

           Input file: if/Chat.idl
           Output directory: impl_xyz/
           Language: Traditional C++ (namespaces)
           Example files: <disable>
           Type files: update
           Makefiles: update

  **Run**

  OR (non-Windows), simply use the makefile, once a target specific makefile 
  has been generated (as above):

          cd impl_xyz/
          make

- XML Datatypes: Run RTI Connext Launcher > Utilities > Type Convert

            Input file: if/Chat.idl
            Output directory: impl_xyz/

 **Run**

  OR, simply use the makefile:

        cd if/
        make

## Building the components

- Open Terminal

        RTI Connext Launcher > Utilities > Code Generator > Open Terminal

- Do the following steps in that terminal window.

- Switch to the component directory:

		cd impl_xyz/

- Build the C++ code

  - Linux | MacOS

            make

  - Windows

            Open Visual Studio Solution


## Running the C++ components

- Switch the working directory to the `if/` directory, so that the
    `USER_QOS_PROFILES.xml` file is automatically loaded.

		cd if/

- Run Publisher Example:

		../impl_xyz/objs/x64Darwin14clang6.0/Chat_publisher

   (replace `x64Darwin14clang6.0` with your target)


- Run Subscriber Example:

        ../impl_xyz/objs/x64Darwin14clang6.0/Chat_subscriber

   (replace `x64Darwin14clang6.0` with your target)

## Running the C# components

- Switch the working directory to the `if/` directory, so that the
    `USER_QOS_PROFILES.xml` file is automatically loaded.

		cd if/

- Run Publisher Example:

		../impl_xyz/bin/Debug-VS2010/Chat_publisher.exe

   (replace `Debug-VS2010` with your target)


- Run Subscriber Example:

		../impl_xyz/bin/Debug-VS2010/Chat_publisher.exe

   (replace `Debug-VS2010` with your target)
   
  
## Running the Lua components


- Switch the working directory to the if/ directory, so that the
    USER_QOS_PROFILES.xml file is automatically loaded.

		cd if/

- Run Publisher Example:

        rtiddsprototyper -cfgName MyParticipantLibrary::MyPublicationParticipant -luaFile ../impl_xyz/Chat_publisher.lua


- Run Subscriber Example:

        rtiddsprototyper -cfgName MyParticipantLibrary::MySubscriptionParticipant -luaFile ../impl_xyz/Chat_subscriber.lua

