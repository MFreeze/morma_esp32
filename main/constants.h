/*
 * =====================================================================================
 *
 *       Filename:  constants.h
 *
 *    Description:  A file containing global default values (not related to any sensor)
 *
 *        Version:  1.0
 *        Created:  16/11/2018 08:46:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  __CONSTANTS_H__
#define  __CONSTANTS_H__

#ifndef  STR_SEP
#define  STR_SEP ','
#endif   /* ----- #ifndef STR_SEP  ----- */

// TODO Change name to be more clearly idnetifiable
#ifndef  BUFFER_SIZE
#define  BUFFER_SIZE 1024
#endif   /* ----- #ifndef BUFFER_SIZE  ----- */


#ifndef  INFLUX_BUFFER_SIZE
#define  INFLUX_BUFFER_SIZE 256
#endif   /* ----- #ifndef INFLUX_BUFFER_SIZE  ----- */


#ifndef  ESCREEN_MAX_STR_SIZE
#define  ESCREEN_MAX_STR_SIZE 64
#endif   /* ----- #ifndef E_SCREEN_MAX_STR_SIZE  ----- */


#ifndef  END_OF_LINE_MEASURE
#define  END_OF_LINE_MEASURE "\n"
#endif   /* ----- #ifndef END_OF_LINE_MEASURE  ----- */


#ifndef  MEASURE_PRECISION
#define  MEASURE_PRECISION "1"
#endif   /* ----- #ifndef MEASURE_PRECISION  ----- */
#endif   /* ----- #ifndef __CONSTANTS_H__  ----- */
