/*
 * =====================================================================================
 *
 *       Filename:  ds18b20.h
 *
 *    Description:  Header file for DS18B20 sensor
 *
 *        Version:  1.0
 *        Created:  06/11/2018 09:50:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef __DS18B20_H__
#define __DS18B20_H__
#include <OneWire.h>

// default values
#define DS_ERROR_VALUE -100.0



/*-----------------------------------------------------------------------------
 *  Enums
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Enums -------- */
/*
 * =====================================================================================
 *  Enumeration:  ds18b20_return_codes
 *  Description:  Description of the errors associated to the ds18b20 probe
 * =====================================================================================
 */
enum ds18b20_return_codes
{
    DS18B20_SUCCESS,
    DS18B20_NO_SENSOR,  // Pas de capteur
    DS18B20_INVALID_ADDRESS,  // Adresse reçue invalide
    DS18B20_INVALID_SENSOR  // Capteur invalide (pas un DS18B20)
};				/* ----------  end of enum ds18b20_return_codes  ---------- */

typedef enum ds18b20_return_codes ds18b20_return_codes_t;
/* }}} */


/*-----------------------------------------------------------------------------
 *  Structures
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Structures -------- */
/*
 * =====================================================================================
 *    Structure:  ds18b20_sensor
 *  Description:  Structure containing every information related to DS18B20 sensors
 *      Members:  * float t: the measured temperature
 *                * byte addr[8]: the address of the sensor
 *                * int initialized: a boolean indicating whether the sensor has
 *                      been initialized or not
 *                * int id: a single id (awful trick to get EScreen working)
 * =====================================================================================
 */
struct ds18b20_sensor 
{
    float t;
    byte addr[8];
    int initialized;
};				/* ----------  end of struct ds18b20_sensor  ---------- */

typedef struct ds18b20_sensor ds18b20_sensor_t;
/* }}} */


/*-----------------------------------------------------------------------------
 *  Functions
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Functions -------- */
// Perform a search of the ds18b20 sensors
int discoverDsSensors ();

// Read temperature on ds18b20 sensors
void readOneWireTemp ();

// print measures in a string
int printDsMeasures (char *str, size_t size, int first);

#ifdef  E_SCREEN
// print measure on the displau
void updateDsScreenValue ();
#endif     /* -----  E_SCREEN  ----- */
/* }}} */

#endif     /* -----  not __DS18B20_H__  ----- */
