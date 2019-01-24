/*
 * =====================================================================================
 *
 *       Filename:  influx_data.cpp
 *
 *    Description:  Body File for Influxdb access
 *
 *        Version:  1.0
 *        Created:  23/01/2019 10:16:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */

/*-----------------------------------------------------------------------------
 *  Include libraries
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Include libraries -------- */
#include "influx_data.h"
#include "config.h"
#include "constants.h"
#include "sensor_debug.h"
#include "local_bme280.h"
#include "ds18b20.h"
#include "wifi_connect.h"
#include <cstdint>
#include <InfluxArduino.hpp>
/* }}} */



/*-----------------------------------------------------------------------------
 *  Variables declaration
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Variables declaration -------- */
static InfluxArduino influx;
static int _configured = 0;
/* }}} */



/*-----------------------------------------------------------------------------
 *  Methods
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Methods -------- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  initInfluxConnection
 *  Description:  Initialize an influxDB connection
 * =====================================================================================
 */
/* --------- initInfluxConnection --------- {{{ */
    void
initInfluxConnection ()
{
    // Initialize WiFi if needed
    initWifiConnection ();

    influx.configure(INFLUX_DATABASE, INFLUX_IP); //third argument (port number) defaults to 8086
    influx.authorize(INFLUX_USER, INFLUX_PASS);   //if you have set the Influxdb .conf variable auth-enabled to true, uncomment this

    // TODO enable https connection 
    // influx.addCertificate(ROOT_CERT);             //uncomment if you have generated a CA cert and copied it into InfluxCert.hpp
    // Serial.print("Using HTTPS: ");
    // Serial.println(influx.isSecure()); //will be true if you've added the InfluxCert.hpp file.
    
    _configured = 1;
}		/* -----  end of function initInfluxConnection  ----- */
/* }}} */



/*
 * ===  FUNCTION  ======================================================================
 *         Name:  sendInfluxData
 *  Description:  Collect and send data to influxdb instance
 *       Return:  an integer indicating whether everything went fine
 * =====================================================================================
 */
/* --------- sendInfluxData --------- {{{ */
    int
sendInfluxData ()
{
    char fields[INFLUX_BUFFER_SIZE] = {0};
    int remaining_size = INFLUX_BUFFER_SIZE;
    int cur_written = 0;
    int first = 1;
    char *fields_parser = fields;
    int result;

    if (!_configured)
        initInfluxConnection ();

#if DS18B20_MEASURES
    cur_written = printDsMeasures (fields_parser, remaining_size, first);
    remaining_size -= cur_written;
    first = 0;
    fields_parser += cur_written;
#endif

#if BME280_MEASURES
    readBmeMeasures ();
    cur_written = printBmeMeasures (fields_parser, remaining_size, first);
    remaining_size -= cur_written;
    first = 0;
    fields_parser += cur_written;
#endif     /* -----  BME280_MEASURES  ----- */

    fields[INFLUX_BUFFER_SIZE - 1] = '\0';
    ESP_LOGI ("influxdb", "Data sent: <%s>\n", fields);

    /* Write data to influxDB */
    result = influx.write(INFLUX_MEASUREMENT, INFLUX_TAGS, fields);

    if (!result)
        ESP_LOGE("influxdb", "error: %d\n", influx.getResponse());

    return (result);
}		/* -----  end of function sendInfluxData  ----- */
/* }}} */

/* }}} */
