#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)
COMPONENT_ADD_INCLUDEDIRS = ./ \
							./../app \
							./../hal_driver 

COMPONENT_SRCDIRS := ./ \
					./../app \
					./../hal_driver 						
							