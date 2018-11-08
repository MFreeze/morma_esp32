/*
 * =====================================================================================
 *
 *       Filename:  escreen_print.h
 *
 *    Description:  Header for the escreen related library
 *
 *        Version:  1.0
 *        Created:  01/11/2018 23:49:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef ESCREEN_PRINT_H
#define ESCREEN_PRINT_H
#include <GxEPD.h>
#include <GxGDEP015OC1/GxGDEP015OC1.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include "bitmap_VBsplashScreen.h"
#include "bitmap_measureBG.h"
#include <Fonts/FreeSans12pt7b.h>
#include "config.h"

#define TFT_CS      14
#define SCK_CLK     18
#define MISO_BUSY   19
#define MOSI_DIN    23
#define TFT_DC      27
#define TFT_RST     33

// Initilize the screen
void initEInkScreen (GxEPD_Class display);

// Partial update the screen
void showPartialUpdate (GxEPD_Class display
#if DS18B20_MEASURES
        ,float tSubs
#endif
#if SOIL_MEASURES
        ,float hSubs
#endif
#if BME280_MEASURES
        ,float tIn
        ,float hIn
        ,float tOut
        ,float hOut
#endif
        );
#endif
