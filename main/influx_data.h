/*
 * =====================================================================================
 *
 *       Filename:  influx_data.h
 *
 *    Description:  Header for the influxdb access
 *
 *        Version:  1.0
 *        Created:  23/01/2019 10:12:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  __INFLUX_DATA_H__
#define  __INFLUX_DATA_H__

void initInfluxConnection ();
int sendInfluxData ();

#endif   /* ----- #ifndef __INFLUX_DATA_H__  ----- */
