/*
 * =====================================================================================
 *
 *       Filename:  screen_macros.h
 *
 *    Description:  Macros for the escreen. This library is only a wrapper for the 
 *                      lolin_screen library. It aims at avoiding heavy use of #ifdef 
 *                      macros all along the code. It also can be used as an interface
 *                      if we decide to provide support for other type of screens.
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

extern char __local_screen_macros_buffer[256];

#define InitScreen(ret_val) do {\
    ret_val = initScreen (); \
} while (0);

#define StartTracer(str_format, ...) do {\
    snprintf (__local_screen_macros_buffer, 256, str_format, ##__VA_ARGS__); \
    initTracing (__local_screen_macros_buffer); \
} while (0);

#define StopTracer(x) do {\
    statusTracing (x); \
} while (0);

#define ClearScreen() do {\
    clearScreen (); \
} while (0);

#define UpdateScreenMeasure(ret_val, sensor_name, measure_name, val) do {\
    ret_val = updateScreenMeasure (sensor_name, measure_name, val); \
} while (0);

#define PrintMeasure() do {\
    printMeasures (); \
} while (0);

#define RegisterSensor(ret_val, sensor_name, sensor_name_len, measure_names, lengths, error_vals, nb_meas) do {\
    ret_val = registerSensor (sensor_name, sensor_name_len, measure_names, lengths, error_vals, nb_meas); \
} while (0);

#else

#define InitScreen(ret_val) do { ret_val = 1; } while (0);
#define StartTracer(str_format, ...)
#define StopTracer(x)
#define ClearScreen()
#define UpdateScreenMeasure(ret_val, sensor_name, measure_name, val) do { ret_val = 1; } while (0);
#define PrintMeasure()
#define RegisterSensor(ret_val, sensor_name, sensor_name_len, measure_names, lengths, error_vals, nb_meas) do { ret_val = 1; } while (0);

#endif     /* -----  not E_SCREEN  ----- */

#endif   /* ----- #ifndef __SCREEN_MACROS_H__  ----- */
