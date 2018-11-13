#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)


morma.o: config.h

escreen_print.o: config.h

config.h: $(COMPONENT_PATH)/config.def.h
	cp $(COMPONENT_PATH)/config.def.h $(COMPONENT_PATH)/config.h
