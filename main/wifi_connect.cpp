/*
 * =====================================================================================
 *
 *       Filename:  wifi_connect.cpp
 *
 *    Description:  Body file for wifi connection
 *
 *        Version:  1.0
 *        Created:  24/01/2019 01:01:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */

#include "wifi_connect.h"
#include "config.h" // This file is included only to be sure that wifi_connect is recompiled as soon as config.h is modified.
#include "debug.h"
#include "screen_macros.h"

// If the wifi ssid or the passphrase is not defined, we run the autoconnect component.
#include <WiFi.h>
#include <WiFiUdp.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <NTPClient.h>


/*-----------------------------------------------------------------------------
 *  Global variables
 *-----------------------------------------------------------------------------*/

#ifndef  PORTAL_TIMEOUT
#define  PORTAL_TIMEOUT     180
#endif   /* ----- #ifndef PORTAL_TIMEOUT  ----- */

/* {{{ -------- Global variables -------- */
static int initialized = 0;
static WebServer Server;
static WiFiManager wifimanager;
static WiFiUDP ntpUDP;
static NTPClient timeClient (ntpUDP, "europe.pool.ntp.org", 3600, 600000);
static char buffer[256];
/* }}} */




/*-----------------------------------------------------------------------------
 *  Methods
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Methods -------- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  initWifiConnection
 *  Description:  Initialize a wifi connection
 * =====================================================================================
 */
/* --------- initWifiConnection --------- {{{ */
    void
initWifiConnection ()
{
    if (initialized)
        return; 

    StartTracer ("Connection Wifi");
    wifimanager.setTimeout (PORTAL_TIMEOUT);
    initialized = wifimanager.autoConnect ("ESP32_AP", "12345678") ? 1 : 0;

    if (initialized)
    {
        StopTracer (1);
        StartTracer ("Init NTP");
        timeClient.begin ();
        StopTracer (1);
    }
    else
    {
        StopTracer (0);
    }
}		/* -----  end of function initWifiConnection  ----- */
/* }}} */



/*
 * ===  FUNCTION  ======================================================================
 *         Name:  wifiLoopRoutine
 *  Description:  A routine that contains the code related to Wifi to be executed at
 *                  each loop. If autoconnect component is used, it runs the Portal;
 *                  otherwise it does nothing.
 * =====================================================================================
 */
/* --------- wifiLoopRoutine --------- {{{ */
    void
wifiLoopRoutine ()
{
    if (initialized)
    {
        timeClient.update ();
    }
}		/* -----  end of function wifiLoopRoutine  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  getFormattedNTPTime
 *  Description:  Return a string containing the formatted NTP Time 
 *   Parameters:  
 *       Return:  a string containing the formatted NTP time
 * =====================================================================================
 */
/* --------- getFormattedNTPTime --------- {{{ */
    const char *
getFormattedNTPTime ()
{
    strncpy (buffer, timeClient.getFormattedTime ().c_str (), 256);
    return buffer;
}		/* -----  end of function getFormattedNTPTime  ----- */
/* }}} */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  getWifiState
 *  Description:  Get the wifi state 
 *   Parameters:  
 *       Return:  Return 1 if connected, 0 otherwise
 * =====================================================================================
 */
/* --------- getWifiState --------- {{{ */
    int
getWifiState ()
{
    return initialized;
}		/* -----  end of function getWifiState  ----- */
/* }}} */
/* }}} */
