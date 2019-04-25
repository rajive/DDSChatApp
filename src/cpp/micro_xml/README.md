# Connext Micro C++ Implementation using XML App Creation (MAG/appgen)

## Environment variables

The RTIMEARCH variable specifies the target architecture to build for.

	set RTIMEARCH x64Darwin17clang9.0

The RTIMEHOME environment variable is used to specify where the Connext Micro is installed. The libs for $RTIMEARCH are expected to be found here.
	
	set RTIMEHOME /path/to/connext/micro/home/with/libs

For example:

	set RTIMEHOME ~/rti_workspace/6.0.0/rti_connext_dds_micro-3.0.0


### Generate build system

#### Release

	mkdir -p build/Release/$RTIMEARCH

macOS (Darwin):
	
	cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -B./build/Release/$RTIMEARCH -H. -DRTIME_TARGET_NAME=$RTIMEARCH -DPLATFORM_LIBS="dl;m;pthread"

#### Debug

	mkdir -p build/Release/$RTIMEARCH

macOS (Darwin):
	
	cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" -B./build/Debug/$RTIMEARCH -H. -DRTIME_TARGET_NAME=$RTIMEARCH -DPLATFORM_LIBS="dl;m;pthread"


### Build using the generated build system

Release:
	
	cmake --build build/Release/$RTIMEARCH

Debug:
	
	cmake --build build/Debug/$RTIMEARCH
	

## [Re]Generating type handling and application code

The generated code is located in a sub-directory: 

	gen/

The type handling code should regenerated every-time the IDL files change.
The application generation code should regenerated every-time the XML files change.

The `CMakeLists.txt` script includes rules so that the generated build system automatically regenerates the code when the corresponding source files change.
