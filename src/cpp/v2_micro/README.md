# Connext Micro C++ Implementation

## Environment variables

The RTIMEARCH variable specifies the target architecture to build for.

	set RTIMEARCH x64Darwin17clang9.0

The RTIMEHOME environment variable is used to specify where the Connext Micro is installed. The libs for $RTIMEARCH are expected to be found here.
	
	set RTIMEHOME /path/to/connext/micro/home/with/libs

For example:

	set RTIMEHOME ~/rti_workspace/6.0.0/rti_connext_dds_micro-3.0.0

## Example application

This example application was generated from IDL as follows:

	$RTIMEHOME/rtiddsgen/scripts/rtiddsgen  -micro -language C++ -namespace -create examplefiles -create makefiles -d . ../../../res/types/v2/Chat.idl

(generates CMakeLists.txt, README.txt, \*Application.h|cxx, \*_publisher|subscriber.cxx))

The above step are optional, to bootstrap a working example. They do not need to be done again. 

The example files have are checked into this directory. 

The generated files have been modified (modifications are marked with the comment: /*>>><<<*/)


### Generate build system

Release:

	mkdir -p build/Release/$RTIMEARCH

	Darwin:
	
	cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -B./build/Release/$RTIMEARCH -H. -DRTIME_TARGET_NAME=$RTIMEARCH -DPLATFORM_LIBS="dl;m;pthread"

Debug:

	mkdir -p build/Release/$RTIMEARCH

	Darwin:
	
	cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" -B./build/Debug/$RTIMEARCH -H. -DRTIME_TARGET_NAME=$RTIMEARCH -DPLATFORM_LIBS="dl;m;pthread"


### Build using the generated build system

(see README.txt)

Release:
	
	cmake --build build/Release/x64Darwin17clang9.0

Debug:
	
	cmake --build build/Debug/x64Darwin17clang9.0
	

## [Re]Generating type handling code

The type handling code is located in a sub-directory: 

	gen/

The type handling code should regenerated every-time the IDL changes, as follows:

	$RTIMEHOME/rtiddsgen/scripts/rtiddsgen  -micro -language C++ -namespace -update typefiles -d gen ../../../res/types/v2/Chat.idl

The `cmake` generated build system automatically regenerates the type handling code every-time the IDL changes.

