/*
 * =====================================================================================
 *
 *       Filename:  ds18b20.cpp
 *
 *    Description:  Body file for ds18b20 component
 *
 *        Version:  1.0
 *        Created:  13/11/2018 17:56:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */

#include "config.h"
#include "constants.h"
#include "sensor_debug.h"
#include "ds18b20.h"


#if  E_SCREEN
#include "escreen_print.h"
#endif     /* -----  E_SCREEN  ----- */

#include <stdlib.h>
#include <esp_log.h>


#ifndef  DS_TEMP_LABEL
#define  DS_TEMP_LABEL "T"
#endif   /* ----- #ifndef DS_TEMP_LABEL  ----- */

#ifndef  DS_TEMP_UNIT
#define  DS_TEMP_UNIT "C"
#endif   /* ----- #ifndef DS_TEMP_UNIT  ----- */

#ifndef  DS18B20_MAX_DISCOVER_TRY
#define  DS18B20_MAX_DISCOVER_TRY 5
#endif   /* ----- #ifndef DS18B20_MAX_DISCOVER_TRY  ----- */

#ifndef  DS18B20_DISCOVER_DELAY
#define  DS18B20_DISCOVER_DELAY 1000
#endif   /* ----- #ifndef DS18B20_DISCOVER_DELAY  ----- */

/*-----------------------------------------------------------------------------
 *  Log Macros
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Log Macros -------- */
#define DS_NAME "DS18B20"
#define DS_ADDR_FORMAT "%#04X%02X%02X%02X%02X%02X%02X%02X"
#define DS_ADDR_ARGS(addr) addr[0],addr[1],addr[2],addr[3],addr[4],addr[5],addr[6],addr[7]
#define DS_SHORT_FORMAT "%02X%02X%02X%02X"
#define DS_SHORT_ARGS(addr) addr[0],addr[1],addr[2],addr[3]
/* }}} */


/*-----------------------------------------------------------------------------
 *  Pin declaration
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Pin declaration -------- */
#ifndef ONEWIRE_PIN
#define ONEWIRE_PIN 25
#endif
/* }}} */


/*-----------------------------------------------------------------------------
 *  Default value
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Default value -------- */
#ifndef  DS_ERROR_VALUE
#define  DS_ERROR_VALUE -100.0
#endif   /* ----- #ifndef DS_ERROR_VALUE  ----- */

/* }}} */

/*-----------------------------------------------------------------------------
 *  Sensors variables
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Sensors variables -------- */
// Initialize the OneWire that will be the interface to the OneWire Sensors (DS18B20)
static OneWire ds(ONEWIRE_PIN);

// One structure for each ds18b20 sensor
static ds18b20_sensor_t ds_sensors[DS18B20_MEASURES];

// Number of discovered probes
static int nb_discovered = 0;
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  discoverDsSensors
 *  Description:  Try to retrieve the address of each sensor supposed to be plugged on
 *                  the board. Perform the search every DS18B20_DISCOVER_DELAY ms until
 *                  DS18B20_MEASURES sensors have been found or up to 
 *                  DS18B20_MAX_DISCOVER_TRY tries.
 *   Parameters:  
 *       Return:  The number of discovered DS18B20 sensors
 * =====================================================================================
 */
/* --------- discoverDsSensors --------- {{{ */
    int
discoverDsSensors ()
{
    int i;
    int cur_try = 0;
    int not_found;

#if  E_SCREEN
    char label[ESCREEN_MAX_STR_SIZE];
#endif     /* -----  E_SCREEN  ----- */

    nb_discovered = 0;

    while (nb_discovered != DS18B20_MEASURES && cur_try < DS18B20_MAX_DISCOVER_TRY)
    {
        delay (DS18B20_DISCOVER_DELAY);

        // Reset search
        ds.reset_search ();
        
        while (ds.search (ds_sensors[nb_discovered].addr))
        {
            // Check the correct reception of the address
            if (OneWire::crc8 (ds_sensors[nb_discovered].addr, 7) 
                    != ds_sensors[nb_discovered].addr[7])
            {
                SENSOR_LOGW (DS_NAME, "Bad reception of address for OneWire component %d.", nb_discovered + 1);
                continue;
            }

            // Check if DS18B20 sensor
            if (ds_sensors[nb_discovered].addr[0] != 0x28)
            {
                SENSOR_LOGI (DS_NAME, "Found OneWire Component but not DS18B20 (%#04X != 0x28).", 
                         ds_sensors[nb_discovered].addr[0]);
                continue;
            }

            // Check if sensor has not already been discovered
            for (i = 0, not_found = 1; not_found && i < nb_discovered; i++)
            { 
                int j = 0;
                for (not_found = 0; j < 8; j++)
                    not_found = ds_sensors[nb_discovered].addr[j] != ds_sensors[i].addr[j] ? 1 : not_found;
            }

            // New sensor found
            if (not_found)
            {
                ds_sensors[nb_discovered].t = 0.0;
                ds_sensors[nb_discovered].initialized = 1;

                SENSOR_LOGI (DS_NAME, 
                             "New sensor found for a total of %d (address: " DS_ADDR_FORMAT ").", 
                             nb_discovered, 
                             DS_ADDR_ARGS (ds_sensors[nb_discovered].addr));

#if  E_SCREEN
                // First register the sensor...
                snprintf (label, 
                          ESCREEN_MAX_STR_SIZE, 
                          DS_SHORT_FORMAT, 
                          DS_SHORT_ARGS (ds_sensors[nb_discovered].addr));
                if (addNewSensorToScreen (label, 1) == ESCREEN_NO_MEM)
                {
                    SENSOR_LOGW (DS_NAME, 
                                 DS_ADDR_FORMAT " won't be displayed on screen (not enough mem error).",
                                 DS_ADDR_ARGS (ds_sensors[nb_discovered].addr));
                }
                else
                {
                    SENSOR_LOGI (DS_NAME,
                                 DS_ADDR_FORMAT " correctly registered to escreen.",
                                 DS_ADDR_ARGS (ds_sensors[nb_discovered].addr));
                }
                
                // ... then the measure
                switch (addNewMeasureToSensorDisplay (label, DS_TEMP_LABEL, DS_TEMP_UNIT))
                {
                    case ESCREEN_TOO_MANY_MEASURES:
                        SENSOR_LOGW (DS_NAME,
                                     DS_ADDR_FORMAT " temperature already added.",
                                     DS_ADDR_ARGS (ds_sensors[nb_discovered].addr));
                        break;
                    case ESCREEN_SENSOR_NOT_FOUND:
                        SENSOR_LOGW (DS_NAME,
                                     DS_ADDR_FORMAT 
                                     " not found, check that the ESCREEN_MAX_STR_SIZE is large enough.",
                                     DS_ADDR_ARGS (ds_sensors[nb_discovered].addr));
                        break;
                    case ESCREEN_SUCCESS:
                        SENSOR_LOGI (DS_NAME,
                                    DS_ADDR_FORMAT " temperature correctly registered to escreen.",
                                    DS_ADDR_ARGS (ds_sensors[nb_discovered].addr));
                        break;
                }
#endif     /* -----  E_SCREEN  ----- */

                nb_discovered++;

                if (nb_discovered == DS18B20_MEASURES)
                    break;
            }
        }

        cur_try++;
        SENSOR_LOGI (DS_NAME, "End of try %d", cur_try);
    }

    SENSOR_LOGI (DS_NAME, "Found %d devices.", nb_discovered);

    return nb_discovered;
}		/* -----  end of function discoverDsSensors  ----- */
/* }}} */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  readOneWireTemp
 *  Description:  Read temperature on a DS18B20 sensor
 *   Parameters:  float *temperatures: a float that will contain the value
 *                byte reset_search: indicates whether a the search for new 
 *       Return:  
 * =====================================================================================
 */
/* --------- readOneWireTemp --------- {{{ */
    void
readOneWireTemp ()
{
    int i;
    byte data[9];       // Data read from the scratchpad

    for (ds18b20_sensor_t *cur = ds_sensors; cur != ds_sensors + nb_discovered; cur++)
    {
        /* Set default value to indicate error if no temperature has been found */
        cur->t = DS_ERROR_VALUE;

        /* Reset le bus 1-Wire et sélectionne le capteur */
        ds.reset ();
        ds.select (cur->addr);

        /* Lance une prise de mesure de température et attend la fin de la mesure */
        ds.write (0x44, 1);
        delay (800);

        /* Reset le bus 1-Wire, sélectionne le capteur et envoie une demande de lecture du scratchpad */
        ds.reset ();
        ds.select (cur->addr);
        ds.write (0xBE);

        /* Lecture du scratchpad */
        for (i = 0; i < 9; i++)
            data[i] = ds.read ();

        /* Calcul de la température en degré Celsius */
        cur->t = (int16_t) ((data[1] << 8) | data[0]) * 0.0625; 
        SENSOR_LOGI (DS_NAME, "[" DS_ADDR_FORMAT "] Read temperature: %.3f.", DS_ADDR_ARGS (cur->addr), cur->t);
    }
}		/* -----  end of function readOneWireTemp  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  printDsMeasures
 *  Description:  Try to write all measured temperatures in a string. If the string is 
 *                  large enough, all temperatures and sensors aliases are concatenated
 *                  and '\0' is written at the end. Otherwise, print up to size - 1 
 *                  characters in the string and add the '\0' character after.
 *   Parameters:  char *str: the buffer string
 *                size_t size; the remaining space in the string
 *                int first: a boolean specifying whether it is the first write in the
 *       Return:  the number of printed characters
 * =====================================================================================
 */
/* --------- printDsMeasures --------- {{{ */
    int
printDsMeasures (char *str, size_t size, int first)
{
    int currently_printed_chars = 0;
    int total_printed_chars = 0;
    int remaining_size = size;
    char *str_parser = str;
    ds18b20_sensor_t *cur;

    *(str_parser - 1) = STR_SEP;
    
    for (cur = ds_sensors; cur != ds_sensors + nb_discovered; cur ++)
    {
        if (cur->t == DS_ERROR_VALUE)
            continue;

        currently_printed_chars = snprintf (str_parser,
                                            remaining_size,
                                            DS_SHORT_FORMAT "_t=%." MEASURE_PRECISION "f%c",
                                            DS_SHORT_ARGS (cur->addr),
                                            cur->t,
                                            STR_SEP);
        total_printed_chars += currently_printed_chars;
        if (currently_printed_chars >= size)
        {
            SENSOR_LOGW (DS_NAME, "Buffer has insufficient space to store all DS values. Consider resizing.");
            break;
        }

        str_parser += currently_printed_chars;
        size -= currently_printed_chars;
    }

    *(str_parser - 1) = '\0';
    return total_printed_chars;
}		/* -----  end of function printDsMeasures  ----- */


#if  E_SCREEN
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  updateDsScreenValue
 *  Description:  Print the DS Measures on screen
 *   Parameters:  
 *       Return:  
 * =====================================================================================
 */
/* --------- updateDsScreenValue --------- {{{ */
    void
updateDsScreenValue ()
{
    char label[ESCREEN_MAX_STR_SIZE];
    ds18b20_sensor_t *cur;

    for (cur = ds_sensors; cur != ds_sensors + nb_discovered; cur ++)
    {
        snprintf (label, 
                  ESCREEN_MAX_STR_SIZE, 
                  DS_SHORT_FORMAT, 
                  DS_SHORT_ARGS (cur->addr));
        updateMeasure (label, DS_TEMP_LABEL, cur->t);
    }
}		/* -----  end of function updateDsScreenValue  ----- */
/* }}} */
#endif     /* -----  E_SCREEN  ----- */
/* }}} */
