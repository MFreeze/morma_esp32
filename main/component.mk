#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)


morma.o: $(COMPONENT_PATH)/config.h
 
escreen_print.o: $(COMPONENT_PATH)/config.h

local_bme280.o: $(COMPONENT_PATH)/config.h

$(COMPONENT_PATH)/config.h: $(COMPONENT_PATH)/config.def.h
	cp -n $(COMPONENT_PATH)/config.def.h $(COMPONENT_PATH)/config.h
