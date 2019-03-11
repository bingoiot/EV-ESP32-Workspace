#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

PLUTO_PATH = ./../..

COMPONENT_ADD_INCLUDEDIRS += $(PLUTO_PATH)/pluto			
COMPONENT_ADD_INCLUDEDIRS += $(PLUTO_PATH)/pluto/fs
COMPONENT_ADD_INCLUDEDIRS += $(PLUTO_PATH)/pluto/socket

COMPONENT_SRCDIRS += $(PLUTO_PATH)/pluto	
COMPONENT_SRCDIRS += $(PLUTO_PATH)/pluto/fs
COMPONENT_SRCDIRS += $(PLUTO_PATH)/pluto/socket

COMPONENT_ADD_LDFLAGS += $(COMPONENT_PATH)/../../pluto/libplutolib.a







COMPONENT_ADD_INCLUDEDIRS += ./
COMPONENT_ADD_INCLUDEDIRS += ./../app
COMPONENT_ADD_INCLUDEDIRS += ./../hal_driver
							
COMPONENT_SRCDIRS += ./
COMPONENT_SRCDIRS += ./../app
COMPONENT_SRCDIRS += ./../hal_driver



	