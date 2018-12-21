######################################################################
# makefile
#
# This makefile assumes that the build environment is already correctly
# configured. (For example, the correct version of your compiler and
# linker should be on the PATH.)
######################################################################

# List the resource directories containing types
RES_DIRS = \
res/types/v1 \
res/types/v2

# List the source code directories that need to be compiled
SRC_DIRS = \
src/c++11/v1/waitset \
\
src/cpp/v1/listener \
src/cpp/v1/polling \
src/cpp/v1/waitset \
\
src/c++11/v2/cft \
src/c++11/v2/cft_xml \
src/c++11/v2/partitions_xml \
\
src/cpp/v2/waitset \
src/cpp/v2/waitset_xml \
src/cpp/v2/cft \
src/cpp/v2/instance \
src/cpp/v2/partitions \
src/cpp/v2/partitions_xml


## Build 
all:
	for dir in $(RES_DIRS) $(SRC_DIRS); do \
		echo; echo $$dir; \
		make -C $$dir ; \
	done

# Clean
clean:
	for dir in $(RES_DIRS) $(SRC_DIRS); do \
		echo; echo $$dir; \
		make -C $$dir clean; \
	done

	
# Generate makefiles or Visual Studio solutions, e.g.:
# 		 make makefile/x64Darwin17clang9.0
makefile/%:
	for dir in $(SRC_DIRS); do \
		echo; echo $$dir; \
		make -C $$dir makefile/$* ; \
	done
