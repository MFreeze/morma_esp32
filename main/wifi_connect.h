/*
 * =====================================================================================
 *
 *       Filename:  wifi_connect.h
 *
 *    Description:  Header file for wifi connection
 *
 *        Version:  1.0
 *        Created:  24/01/2019 00:58:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  __WIFI_CONNECTION_H__
#define  __WIFI_CONNECTION_H__


void initWifiConnection ();
void wifiLoopRoutine ();
const char *getFormattedNTPTime ();
int getWifiState ();

#endif   /* ----- #ifndef __WIFI_CONNECTION_H__  ----- */
