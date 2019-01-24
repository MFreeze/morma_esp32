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

// If the wifi ssid or the passphrase is not defined, we run the autoconnect component.
#if !defined(WIFI_NAME) || !defined(WIFI_PASS)
#define USE_AUTOCONNECT 1
#endif

#include <WiFi.h>

#if USE_AUTOCONNECT
#include <WebServer.h>
#include <AutoConnect.h>
#endif


/*-----------------------------------------------------------------------------
 *  Global variables
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Global variables -------- */
static int initialized = 0;
#if USE_AUTOCONNECT
static WebServer Server;
static AutoConnect Portal(Server);
#endif
/* }}} */




/*-----------------------------------------------------------------------------
 *  Local Function
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Local Function -------- */
# if USE_AUTOCONNECT
    void 
rootPage ()
{
    char content[] = "Hello, world";
    Server.send (200, "text/plain", content);
}
#endif
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

#if USE_AUTOCONNECT
    Server.on ("/", rootPage);
    Portal.begin ();
#else
    WiFi.begin (WIFI_NAME, WIFI_PASS);
    while (WiFi.status () != WL_CONNECTED)
        delay (100);
    Serial.println ("Wifi connected!");
#endif
    initialized = 1;    
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
#if USE_AUTOCONNECT
    Portal.handleClient ();
#endif
}		/* -----  end of function wifiLoopRoutine  ----- */
/* }}} */
/* }}} */
