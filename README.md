# Monitor Organic Matter (MOrMa)

MOrMa aims at providing a reliable, cheap and easy to use measures station to monitor temperature
and hygrometry of a compost or substrate of flies farming. It is part of the [ValueBugs
Project](https://valuebugs.org/).

The station relies on a [LoLin D32 Pro](https://wiki.wemos.cc/products:d32:d32_pro) board based on
ESP32-ROVER.

This repos aims at providing an easy to use development and produciton platform to compile the
firmware for the board using [Espressig IoT Development
Framework](https://github.com/espressif/esp-idf).

## Prerequisite

The esp-idf must be installed and configured. See the [Get Started
Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/) for more informations.

## Electric scheme

Presently being carried out.

## Install

### Clone the repository

The repository use submodules to manage the components needed for the compilation. To clone the repo
and fetch every submodule use the following command:

    git clone --recursive https://github.com/MFreeze/morma_esp32


### Setup

#### Components.mk

Some of the components of the project are developed for Arduino hardware (like
[GxEPD](https://github.com/ZinggJM/GxEPD), the driver for the waveshare epaper screen). To be
recognized as comonents by esp-idf, a `components.mk` file has to be provided. This repo provides a 
small script to generate those files.

    ./manage_component_make

#### arduino-esp32

This component requires some further configuration to be used as such. Follow instructions given in
the Installation section of [this
file](https://github.com/espressif/arduino-esp32/blob/master/docs/esp-idf_component.md) starting at
the `make menuconfig` point.

#### ESP32 configuration

The first thing is to run the following command

    make menuconfig

You'll have to ensure that your configuration respects the following points.

* Under `SDK tool configuration`, check that the python2 interpreter is python2 (if your system uses python3 as default python interpreter)
* Under `Arduino Configuration`, Autostart Arduino setup and loop on boot is activated
* Under `Serial flasher config`, Flash size is equal to 4MB
* Under `Compiler options`, ensure that `Enable C++ exceptions` option is enabled

### Software configuration

The configuration of the software is managed thanks to a `config.h` file. This file is generated, if
needed, by the `make` command from the `config.def.h` file containing default configuration. 

A large number of parameters can be defined in the `config.h` file and the latters are partitionned
into three categories. 

1. The **core parameters** are the options defined in the `config.def.h` file determining the
   functionnalities of the MOrMa station. Here is the list of such options:
    * `E_SCREEN (0|1)`: enable the screen support
    * `WIFI (0|1)`: enable the wifi connection
    * `BME280_MEASURES (0|1|2|3|4)`: indicates the number of BME280 sensors used on the device
    * `DS18B20_MEASURES (int)`: indicates the number of DS18B20 sensors plugged on the device
    * `SOIL_MEASURES (0|1)`: enable the measure of soil humidity (inactive at this time)
    * `SEND_DATA_INFLUXDB (0|1)`: enable the sending of measure to InfluxDB server
    * `USE_SPIFFS (0|1)`: enable the storage of data on the SPIFFS
2. The **required parameters** are options that do not have a default value. Those are commented in
   the `config.def.h` to ensure that compiling process fails if not expressly defined by the user.
   At this time the number of such options is limited:
    * `ESP_ID "string"` determine the ID of the device
3. The **optional parameters** are parameters that have default values defined in the different
   files of the project. The default value of the parameters is defined in the associated library
   ( *e.g.* the BME_NAME_1 is defined in `local_bme.cpp` ). Theses parameters are listed hereafter.

#### DS18B20 parameters

* `DS_TEMP_LABEL "T"`: the label of the temperature field on the screen or on the debug strings
* `DS_TEMP_UNIT "C"`: the way the unit of the tempertature field will be displayed on the screen
    or on the debug strings
* `DS18B20_MAX_DISCOVER_TRY 5`: the maximum number of tries that can be performed to discover the
    declared number of DS18B20 probes
* `DS18B20_DISCOVER_DELAY 1000`: the delay in ms between each discover try
* `ONEWIRE_PIN 25`: the pin of the ESP32 on which data is plugged
* `DS_ERROR_VALUE -100.0`: the default printed value if DS18B20 measure failed

#### BME280 parameters

* `BME280_INIT_ADDRESS 0x76`: the default address of a BME280 sensor
* `BME280_MODIFIED_ADDRESS 0x77`: the address of a physically hacked BME280 sensor 
* `BME_LABEL_IN_TITLE 1`: should the label of the fields related to BME280 sensors be displayed in
    the header line ( *e.g.* BME_1 (T,H,P) ), ot should they be displayed next to the the measure?
    (only active if a screen is used)
* `BME_TEMP_LABEL "T"`: the label for the temperature field
* `BME_TEMP_UNIT "C"`: the label fot the temperature unit
* `BME_PRESSURE_LABEL "P"`: the label for the pressure field
* `BME_PRESSURE_UNIT "hPa"`: the label fot the pressure unit
* `BME_HUMIDITY_LABEL "H"`: the label fot the humidity field
* `BME_HUMIDITY_UNIT "%"`: the label for the humidity unit
* `BME_NAME_1 "BME_Sensor_1"`: the name of the first BME280 sensor
* `BME_NAME_2 "BME_Sensor_2"`: the name of the second BME280 sensor
* `BME_NAME_3 "BME_Sensor_3"`: the name of the third BME280 sensor
* `BME_NAME_4 "BME_Sensor_4"`: the name of the fourth BME280 sensor
* `I2C1_DATA  21`: the default data pin for the first I2C port
* `I2C1_CLOCK 22`: the default clock pin for the first I2C port
* `I2C2_DATA   4`: the default data pin for the second I2C port
* `I2C2_CLOCK 15`: the default clock pin for the second I2C port

#### ESCREEN parameters

* `TFT_CS      14`: default Chip Select pin
* `SCK_CLK     18`: default clock pin
* `MISO_BUSY   19`: default "Master Input Slave Output" pin
* `MOSI_DIN    23`: default "Master Output Slave Input" pin 
* `TFT_DC      27`: default Data Control pin
* `TFT_RST     33`: default Reset pin

### Compilation

A simple `make flash` should do the trick to compile and flash the board.

## Update

To update the associated components use the following command:

    git submodule update --init --recursive



