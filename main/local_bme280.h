/*
 * =====================================================================================
 *
 *       Filename:  bme280.h
 *
 *    Description:  Header for the externalization of BME280 sensors
 *
 *        Version:  1.0
 *        Created:  13/11/2018 19:53:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __LOCAL_BME280_H__
#define __LOCAL_BME280_H__

#include <Adafruit_BME280.h>

/*-----------------------------------------------------------------------------
 *  Enums
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Enums -------- */
/*
 * =====================================================================================
 *  Enumeration:  bme280_return_codes
 *  Description:  Keep track of the states of the different bme280 sensors
 * =====================================================================================
 */
enum bme280_return_codes
{
    BME280_SUCCESS = 0,
    BME280_ERROR_1 = 1
#if BME280_MEASURES > 1
    ,BME280_ERROR_2 = 2
#if BME280_MEASURES > 2
    ,BME280_ERROR_3 = 4
#if BME280_MEASURES > 3
    ,BME280_ERROR_4 = 8
#endif     /* -----  BME280_MEASURES > 3  ----- */
#endif     /* -----  BME280_MEASURES > 2  ----- */
#endif     /* -----  BME280_MEASURES > 1  ----- */
};				/* ----------  end of enum bme280_return_codes  ---------- */

typedef enum bme280_return_codes bme280_return_codes_t;
/* }}} */


/*-----------------------------------------------------------------------------
 *  Structs
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Structs -------- */
/*
 * =====================================================================================
 *    Structure:  bme280_sensor
 *  Description:  A structure that contains all the values measured by a BME280 sensor
 *      Members:  * Adafruit_BME280 interface: interface with the electronic component
 *                * const char *name: the human readable name of the sensor
 *                * float t: temperature
 *                * float h: humidity
 *                * float p: pressure
 *                * int missing: indicate if measure has been done or not
 * =====================================================================================
 */
struct bme280_sensor {
    Adafruit_BME280 interface;
    const char *name;
    float t, h, p;
    int missing;
};				/* ----------  end of struct bme280_sensor  ---------- */

typedef struct bme280_sensor bme280_sensor_t;
/* }}} */



/*-----------------------------------------------------------------------------
 *  Functions
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Functions -------- */

// Initialize the required variables and check whether every component is plugged
int initBmeSensors ();

// Read sensors measures and fill the corresponding structure
int readBmeMeasures ();

// Print measure in a string
int printBmeMeasures (char *str, size_t size);
/* }}} */

#endif
