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

#define StartTracer(x) do {\
    initTracing (x); \
} while (0);

#define StopTracer(x) do {\
    statusTracing (x); \
} while (0);

#else

#define StartTracer(x)
#define StopTracer(x)
#endif     /* -----  not E_SCREEN  ----- */

#endif   /* ----- #ifndef __SCREEN_MACROS_H__  ----- */
