######################################################################
# makefile
#
# This makefile assumes that the build environment is already correctly
# configured. (For example, the correct version of your compiler and
# linker should be on the PATH.)
######################################################################

# Recursively build all the directories
DIRS = \
v1_singleton/impl_cpp_listener \
v1_singleton/impl_cpp_polling \
v1_singleton/impl_cpp_waitset \
v2_by_usr/impl_cpp \
v2_by_usr/impl_cpp_cft \
v2_by_usr/impl_cpp_instance \
v2_by_usr/impl_cpp_partitions \
v3_by_usr_doi/impl_xml_cpp


## Build 
all:
	for dir in $(DIRS); do \
		echo; echo $$dir; \
		make -C $$dir/../if ; \
		make -C $$dir ; \
	done

# Clean
clean:
	for dir in $(DIRS); do \
		echo; echo $$dir; \
		make -C $$dir/../if clean ; \
		make -C $$dir clean ; \
	done

	
# Generate makefiles or Visual Studio solutions, e.g.:
# 		 make makefile/x64Darwin17clang9.0
makefile/%:
	for dir in $(DIRS); do \
		echo; echo $$dir; \
		rm $$dir/makefile_Chat_* ;\
		rtiddsgen $$dir/../if/Chat.idl -d $$dir -update makefiles -platform $* ;\
	done
