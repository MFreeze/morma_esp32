#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)


# TODO use foreach make statment
morma.o: $(COMPONENT_PATH)/config.h $(COMPONENT_PATH)/sensor_debug.h $(COMPONENT_PATH)/constants.h
 
escreen_print.o: $(COMPONENT_PATH)/config.h $(COMPONENT_PATH)/sensor_debug.h $(COMPONENT_PATH)/constants.h

local_bme280.o: $(COMPONENT_PATH)/config.h $(COMPONENT_PATH)/sensor_debug.h $(COMPONENT_PATH)/constants.h

ds18b20.o: $(COMPONENT_PATH)/config.h $(COMPONENT_PATH)/sensor_debug.h $(COMPONENT_PATH)/constants.h

$(COMPONENT_PATH)/config.h: $(COMPONENT_PATH)/config.def.h
	cp -n $(COMPONENT_PATH)/config.def.h $(COMPONENT_PATH)/config.h
