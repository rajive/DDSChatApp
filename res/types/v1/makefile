######################################################################
# makefile
#
# This makefile assumes that the build environment is already correctly
# configured. (For example, the correct version of your compiler and
# linker should be on the PATH.)
######################################################################

# If undefined in the environment default NDDSHOME to install dir
ifndef NDDSHOME
NDDSHOME := "/opt/rti/NDDSHOME"
endif

# Generate XML from IDL types
#
Chat.xml : \
Chat.idl
	$(NDDSHOME)/bin/rtiddsgen -convertToXml -d .  Chat.idl

clean:
	rm -rf Chat.xml
	