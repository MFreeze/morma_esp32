/*
 * =====================================================================================
 *
 *       Filename:  config.h
 *
 *    Description:  Configuration file
 *
 *        Version:  1.0
 *        Created:  08/11/2018 12:13:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __CONFIG_H
#define __CONFIG_H
/*-----------------------------------------------------------------------------
 *  Configuration
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Configuration -------- */
#define E_SCREEN            1       // Control the type of display
#define WIFI                0       // Use of Wifi
#define BME280_MEASURES     1       // Should morma try to perform measure with BME280 sensors?
#define DS18B20_MEASURES    1       // Should morma try to perform measure with DS18B20 sensors?
#define SOIL_MEASURES       0       // Should morma try to perform soil measures?
#define SEND_DATA_INFLUXDB  0       // Should morma send data to InfluxDB server?
#define END_OF_MEASURE_LINE "\n"    
#define WEB_SERVER          0
#define USE_SPIFFS          1       // Determine whether we should use internal storage
/* }}} */
#endif
