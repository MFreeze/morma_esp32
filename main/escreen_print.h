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

// Initilize the screen
void initEInkScreen (GxEPD_Class display);

// Partial update the screen
void showPartialUpdate (GxEPD_Class display, float tSubs, float hSubs, float tIn, float hIn, float tOut, float hOut);
#endif
