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

Don't forget to **enable** "Autostart Arduino setup and loop on boot" option.

### Compilation

A simple `make flash` should do the trick to compile and flash the board.

## Update

To update the associated components use the following command:

    git submodule update --init --recursive



