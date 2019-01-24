#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)


# TODO use foreach make statment
morma.o: $(COMPONENT_PATH)/config.h $(COMPONENT_PATH)/sensor_debug.h $(COMPONENT_PATH)/constants.h
 
escreen_print.o: $(COMPONENT_PATH)/config.h $(COMPONENT_PATH)/sensor_debug.h $(COMPONENT_PATH)/constants.h

local_bme280.o: $(COMPONENT_PATH)/config.h $(COMPONENT_PATH)/sensor_debug.h $(COMPONENT_PATH)/constants.h

ds18b20.o: $(COMPONENT_PATH)/config.h $(COMPONENT_PATH)/sensor_debug.h $(COMPONENT_PATH)/constants.h

influx_data.o: $(COMPONENT_PATH)/config.h $(COMPONENT_PATH)/sensor_debug.h $(COMPONENT_PATH)/constants.h $(COMPONENT_PATH)/influxdb_cert.h

wifi_connect.o: $(COMPONENT_PATH)/config.h $(COMPONENT_PATH)/sensor_debug.h $(COMPONENT_PATH)/constants.h

$(COMPONENT_PATH)/config.h: $(COMPONENT_PATH)/config.def.h
	cp -n $(COMPONENT_PATH)/config.def.h $(COMPONENT_PATH)/config.h

$(COMPONENT_PATH)/influxdb_cert.h: $(COMPONENT_PATH)/influxdb_cert_valuebugs.def.h
	cp -n $(COMPONENT_PATH)/influxdb_cert_valuebugs.def.h $(COMPONENT_PATH)/influxdb_cert.h
