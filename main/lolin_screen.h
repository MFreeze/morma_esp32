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
void initScreen ();

void initTracing (const char *);
void statusTracing (int);

#endif   /* ----- #ifndef __LOLIN_SCREEN_H__  ----- */
