/*
 * =====================================================================================
 *
 *       Filename:  screen_macros.h
 *
 *    Description:  Macros for the escreen 
 *
 *        Version:  1.0
 *        Created:  22/10/2019 02:26:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  __SCREEN_MACROS_H__
#define  __SCREEN_MACROS_H__


#ifdef  E_SCREEN
#include "lolin_screen.h"

extern char screen_macros_buffer[256];

#define StartTracer(str_format, ...) do {\
    snprintf (screen_macros_buffer, 256, str_format, ##__VA_ARGS__); \
    initTracing (screen_macros_buffer); \
} while (0);

#define StopTracer(x) do {\
    statusTracing (x); \
} while (0);

#define ClearScreen() do {\
    clearScreen (); \
} while (0);

#else

#define StartTracer(str_format, ...)
#define StopTracer(x)
#define ClearScreen()

#endif     /* -----  not E_SCREEN  ----- */

#endif   /* ----- #ifndef __SCREEN_MACROS_H__  ----- */
