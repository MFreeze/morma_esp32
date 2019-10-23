/*
 * =====================================================================================
 *
 *       Filename:  lolin_screen.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21/10/2019 14:28:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */



#ifndef  __LOLIN_SCREEN_H__
#define  __LOLIN_SCREEN_H__

#include <LOLIN_EPD.h>
#include <Adafruit_GFX.h>




// Initializes the screen
int initScreen ();

// Clear screen
void clearScreen ();

// Print status of a given operation
void initTracing (const char *);
void statusTracing (int);

// Register a new sensor
int registerNewSensor (const char *, int, const char**, int*, float *, int);

#endif   /* ----- #ifndef __LOLIN_SCREEN_H__  ----- */
