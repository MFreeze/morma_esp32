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
#define E_SCREEN                1       // Control the type of display
#define WIFI                    0       // Use of Wifi
#define BME280_MEASURES         2       // Should morma try to perform measure with BME280 sensors?
#define DS18B20_MEASURES        1       // Should morma try to perform measure with DS18B20 sensors?
#define SOIL_MEASURES           0       // Should morma try to perform soil measures?
#define SEND_DATA_INFLUXDB      0       // Should morma send data to InfluxDB server?
#define WEB_SERVER              0
#define USE_SPIFFS              1       // Determine whether we should use internal storage
#define SENSOR_DEBUG            1

#define BME_NAME_1              "out"
#define BME_NAME_2              "in"

#define MEASURE_DELAY           600     // The time between two loops of measures
#define SAVED_MEASURE_PERIOD    1       // Only the first measure of each cycle of SAVED_MEASURE_PERIOD will be
                                        // saved. Thus a measure is saved every 
                                        // MEASURE_DELAY * SAVED_MEASURE_PERIOD seconds

#define NB_MEASURES_BEFORE_FILE_ROTATION 2000
// #define ESP_ID "Ecole2"
/* }}} */
#endif
