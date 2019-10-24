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
#include <WebServer.h>
#include <AutoConnect.h>


/*-----------------------------------------------------------------------------
 *  Global variables
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Global variables -------- */
static int initialized = 0;
static WebServer Server;
static AutoConnect Portal(Server);
/* }}} */




/*-----------------------------------------------------------------------------
 *  Local Function
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Local Function -------- */
    void 
rootPage ()
{
    WIFI_LOGI ("Serving root page!");
    char content[] = "Hello, world";
    Server.send (200, "text/plain", content);
}
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
    Server.on ("/", rootPage);
    if (Portal.begin ())
    {
        WIFI_LOGI ("Connected to Wifi (%s)", WiFi.localIP ().toString ().c_str());        
        StopTracer (1);
    }
    else
    {
        WIFI_LOGW ("Unable to connect, fallback to access point mode.");
        StopTracer (0);
    }
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
    Portal.handleClient ();
}		/* -----  end of function wifiLoopRoutine  ----- */
/* }}} */
/* }}} */
