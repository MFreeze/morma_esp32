/*
 * =====================================================================================
 *
 *       Filename:  bme280.cpp
 *
 *    Description:  Body file for bme280 externalization
 *
 *        Version:  1.0
 *        Created:  13/11/2018 20:58:38
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
#include "local_bme280.h"

#ifdef  E_SCREEN
#include "escreen_print.h"
#endif

#include <stdlib.h>
#include <esp_log.h>


/*-----------------------------------------------------------------------------
 *  Constants
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Constants -------- */
#ifndef BME280_INIT_ADDRESS
#define BME280_INIT_ADDRESS 0x76
#endif

#ifndef BME280_MODIFIED_ADDRESS
#define BME280_MODIFIED_ADDRESS 0x77
#endif

#ifndef  BME_TEMP_LABEL
#define  BME_TEMP_LABEL "T"
#endif   /* ----- #ifndef BME_TEMP_LABEL  ----- */

#ifndef  BME_TEMP_UNIT
#define  BME_TEMP_UNIT "C"
#endif   /* ----- #ifndef BME_TEMP_UNIT  ----- */

#ifndef  BME_PRESSURE_LABEL
#define  BME_PRESSURE_LABEL "P"
#endif   /* ----- #ifndef BME_PRESSURE_LABEL  ----- */

#ifndef  BME_PRESSURE_UNIT
#define  BME_PRESSURE_UNIT "hPa"
#endif   /* ----- #ifndef BME_PRESSURE_UNIT  ----- */

#ifndef  BME_HUMIDITY_LABEL
#define  BME_HUMIDITY_LABEL "H"
#endif   /* ----- #ifndef BME_HUMIDITY_LABEL  ----- */

#ifndef  BME_HUMIDITY_UNIT
#define  BME_HUMIDITY_UNIT "%"
#endif   /* ----- #ifndef BME_HUMIDITY_UNIT  ----- */
/* }}} */


/*-----------------------------------------------------------------------------
 *  Sensor Name
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Sensor Name -------- */
#ifndef  BME_NAME_1
#define	BME_NAME_1 "BME_Sensor_1"
#endif     /* -----  not BME_NAME_1  ----- */

#ifndef  BME_NAME_2
#define	BME_NAME_2 "BME_Sensor_2"
#endif     /* -----  not BME_NAME_2  ----- */

#ifndef  BME_NAME_3
#define	BME_NAME_3 "BME_Sensor_3"
#endif     /* -----  not BME_NAME_3  ----- */

#ifndef  BME_NAME_4
#define	BME_NAME_4 "BME_Sensor_4"
#endif     /* -----  not BME_NAME_3  ----- */

/* }}} */

/*-----------------------------------------------------------------------------
 *  Log Macros
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Log Macros -------- */
#define BME_NAME "BME280"
/* }}} */


/*-----------------------------------------------------------------------------
*  Pin declaration
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Pin declaration -------- */
#ifndef I2C1_DATA
#define I2C1_DATA  21
#endif

#ifndef I2C1_CLOCK
#define I2C1_CLOCK 22
#endif

#ifndef I2C2_DATA
#define I2C2_DATA   4
#endif

#ifndef I2C2_CLOCK
#define I2C2_CLOCK 15
#endif
/* }}} */

/*-----------------------------------------------------------------------------
 *  Sensors variables
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Sensors variables -------- */
static bme280_sensor_t bme_1;
#if BME280_MEASURES > 1
static bme280_sensor_t bme_2;
#if BME280_MEASURES > 2
static bme280_sensor_t bme_3;
#if BME280_MEASURES > 3
static bme280_sensor_t bme_4;
#endif     /* -----  BME280_MEASURES > 3  ----- */
#endif     /* -----  BME280_MEASURES > 2  ----- */
#endif     /* -----  BME280_MEASURES > 1  ----- */
/* }}} */


/*-----------------------------------------------------------------------------
 *  Sensors Macros Function
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Sensors Macros Function -------- */
#define BME_CHECK_AND_INIT_SENSOR(x, addr, w, n, rcode) do {\
    if (!bme_##x.interface.begin (addr, &w))\
    {\
        rcode |= BME280_ERROR_##x;\
        SENSOR_LOGE (BME_NAME, "Cannot find sensor %s. Check wiring!\n", n);\
    }\
    bme_##x.t = 0.0;\
    bme_##x.h = 0.0;\
    bme_##x.p = 0.0;\
    bme_##x.missing = 0;\
    bme_##x.name = n;\
} while (0);

#if  E_SCREEN
#define BME_ADD_TO_SCREEN(n) do {\
    if (addNewSensorToScreen (n, 3, 1) == ESCREEN_NO_MEM)\
    {\
        SENSOR_LOGW (BME_NAME, "%s won't be displayed on screen (not enough mem error).\n", n);\
    }\
    else\
    {\
        SENSOR_LOGI (BME_NAME,"%s correctly registered to escreen.\n", n);\
    }\
    switch (addNewMeasureToSensorDisplay (n, BME_TEMP_LABEL, BME_TEMP_UNIT))\
    {\
        case ESCREEN_TOO_MANY_MEASURES:\
            SENSOR_LOGW (BME_NAME, "%s temperature already added.\n", n);\
            break;\
        case ESCREEN_SENSOR_NOT_FOUND:\
            SENSOR_LOGW (BME_NAME, "%s not found, check that the ESCREEN_MAX_STR_SIZE is large enough.\n", n);\
            break;\
        case ESCREEN_SUCCESS:\
            SENSOR_LOGI (BME_NAME, "%s temperature correctly registered to escreen.\n", n);\
            break;\
    }\
    switch (addNewMeasureToSensorDisplay (n, BME_HUMIDITY_LABEL, BME_HUMIDITY_UNIT))\
    {\
        case ESCREEN_TOO_MANY_MEASURES:\
            SENSOR_LOGW (BME_NAME, "%s humidity already added.\n", n);\
            break;\
        case ESCREEN_SENSOR_NOT_FOUND:\
            SENSOR_LOGW (BME_NAME, "%s not found, check that the ESCREEN_MAX_STR_SIZE is large enough.\n", n);\
            break;\
        case ESCREEN_SUCCESS:\
            SENSOR_LOGI (BME_NAME, "%s humidity correctly registered to escreen.\n", n);\
            break;\
    }\
    switch (addNewMeasureToSensorDisplay (n, BME_PRESSURE_LABEL, BME_PRESSURE_UNIT))\
    {\
        case ESCREEN_TOO_MANY_MEASURES:\
            SENSOR_LOGW (BME_NAME, "%s pressure already added.\n", n);\
            break;\
        case ESCREEN_SENSOR_NOT_FOUND:\
            SENSOR_LOGW (BME_NAME, "%s not found, check that the ESCREEN_MAX_STR_SIZE is large enough.\n", n);\
            break;\
        case ESCREEN_SUCCESS:\
            SENSOR_LOGI (BME_NAME, "%s pressure correctly registered to escreen.\n", n);\
            break;\
    }\
} while(0);
#endif     /* -----  E_SCREEN  ----- */

#define BME_READ_MEASURES(x, addr, w, rcode) do {\
    if (!bme_##x.interface.begin (addr, &w))\
    {\
        bme_##x.missing = 1;\
        rcode |= BME280_ERROR_##x;\
        SENSOR_LOGE (BME_NAME, "Cannot find sensor %s. Check wiring!\n", bme_##x.name);\
    }\
    else\
    {\
        bme_##x.t = bme_##x.interface.readTemperature ();\
        bme_##x.h = bme_##x.interface.readHumidity ();\
        bme_##x.p = bme_##x.interface.readPressure () / 100.;\
        bme_##x.missing = 0;\
    }\
} while (0);

#define BME_PRINT_TO_STR(x, parser, size, cur_written, total_written, sep) do {\
    if (!bme_##x.missing) \
    {\
        cur_written = snprintf (parser, \
                                size, \
                                "%s_t:%.3f%c"\
                                "%s_h:%.3f%c"\
                                "%s_p:%.3f%c",\
                                bme_##x.name,\
                                bme_##x.t,\
                                STR_SEP,\
                                bme_##x.name,\
                                bme_##x.h,\
                                STR_SEP,\
                                bme_##x.name,\
                                bme_##x.p,\
                                STR_SEP);\
        total_written += cur_written;\
        if (cur_written >= size)\
        {\
            SENSOR_LOGW (BME_NAME, "Buffer has insufficient space to store all BME values for %s. Consider resizing.\n", bme_##x.name);\
            *(parser - 1) = '\0';\
            return total_written;\
        }\
        parser += cur_written;\
        size -= cur_written;\
    }\
} while (0);


#ifdef  E_SCREEN
#define BME_UPDATE_ON_SCREEN(x, n) do {\
    if (!bme_##x.missing) {\
        updateMeasure (n, "T", bme_##x.t);\
        updateMeasure (n, "H", bme_##x.h);\
        updateMeasure (n, "P", bme_##x.p);\
    }\
} while (0);
#endif     /* -----  E_SCREEN  ----- */
/* }}} */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  initBmeSensors
 *  Description:  Initialize the required Wire variables and check whether every 
 *                  is plugged.
 *   Parameters:  
 *       Return:  BME280_SUCCESS on Success, and a binary combination of BME280_Errors
 *                  if something fails.
 * =====================================================================================
 */
/* --------- initBmeSensors --------- {{{ */
    int
initBmeSensors ()
{
    int return_code = BME280_SUCCESS;

    Wire.begin (I2C1_DATA, I2C1_CLOCK);
#if BME280_MEASURES > 1
    Wire1.begin (I2C2_DATA, I2C2_CLOCK);
#endif

    BME_CHECK_AND_INIT_SENSOR (1, BME280_INIT_ADDRESS, Wire, BME_NAME_1, return_code);
    BME_ADD_TO_SCREEN (BME_NAME_1);
#if BME280_MEASURES > 1
    BME_CHECK_AND_INIT_SENSOR (2, BME280_INIT_ADDRESS, Wire1, BME_NAME_2, return_code);
    BME_ADD_TO_SCREEN (BME_NAME_2);
#if BME280_MEASURES > 2
    BME_CHECK_AND_INIT_SENSOR (3, BME280_MODIFIED_ADDRESS, Wire, BME_NAME_3, return_code);
    BME_ADD_TO_SCREEN (BME_NAME_3);
#if BME280_MEASURES > 3
    BME_CHECK_AND_INIT_SENSOR (4, BME280_MODIFIED_ADDRESS, Wire1, BME_NAME_4, return_code);
    BME_ADD_TO_SCREEN (BME_NAME_4);
#endif     /* -----  BME280_MEASURES > 3  ----- */
#endif     /* -----  BME280_MEASURES > 2  ----- */
#endif     /* -----  BME280_MEASURES > 1  ----- */

    return return_code;
}		/* -----  end of function initBmeSensors  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  readBmeMeasures
 *  Description:  Read sensors measures and fill the corresponding structures
 *   Parameters:  
 *       Return:  BME280_SUCCESS on Success, and a binary combination of BME280_Errors
 *                  if one or more measures fail.
 * =====================================================================================
 */
/* --------- readBmeMeasures --------- {{{ */
    int
readBmeMeasures ()
{
    int return_code = BME280_SUCCESS;

    Wire.begin (I2C1_DATA, I2C1_CLOCK);
#if BME280_MEASURES > 1
    Wire1.begin (I2C2_DATA, I2C2_CLOCK);
#endif

    BME_READ_MEASURES (1, BME280_INIT_ADDRESS, Wire, return_code);
#if BME280_MEASURES > 1
    BME_READ_MEASURES (2, BME280_INIT_ADDRESS, Wire1, return_code);
#if BME280_MEASURES > 2
    BME_READ_MEASURES (3, BME280_MODIFIED_ADDRESS, Wire, return_code);
#if BME280_MEASURES > 3
    BME_READ_MEASURES (4, BME280_MODIFIED_ADDRESS, Wire1, return_code);
#endif     /* -----  BME280_MEASURES > 3  ----- */
#endif     /* -----  BME280_MEASURES > 2  ----- */
#endif     /* -----  BME280_MEASURES > 1  ----- */

    return return_code;
}		/* -----  end of function readBmeMeasures  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  printBmeMeasures
 *  Description:  Print all measure in a string if the string is large enough. Otherwise
 *                  print up to size characters in the string, including the '\0' 
 *                  character
 *   Parameters:  char *str: the buffer string
 *                size_t size: the remaining space in the string
 *                int first: a boolean specifying whether it is the first write in the
 *                  string
 *       Return:  the number of printed character
 * =====================================================================================
 */
/* --------- printBmeMeasures --------- {{{ */
    int
printBmeMeasures (char *str, size_t size, int first)
{
    int currently_printed_chars = 0;
    int total_printed_chars = 0;
    int remaining_size = size;
    char *str_parser = str;

    if (!first)
        *(str_parser - 1) = ',';

    BME_PRINT_TO_STR(1, str_parser, remaining_size, currently_printed_chars, total_printed_chars, sep);
#if  BME280_MEASURES > 1
    BME_PRINT_TO_STR(2, str_parser, remaining_size, currently_printed_chars, total_printed_chars, sep);
#if  BME280_MEASURES > 2
    BME_PRINT_TO_STR(3, str_parser, remaining_size, currently_printed_chars, total_printed_chars, sep);
#if  BME280_MEASURES > 3
    BME_PRINT_TO_STR(4, str_parser, remaining_size, currently_printed_chars, total_printed_chars, sep);
#endif     /* -----  BME280_MEASURES > 3  ----- */
#endif     /* -----  BME280_MEASURES > 2  ----- */
#endif     /* -----  BME280_MEASURES > 1  ----- */

    if (total_printed_chars)
        *(str_parser - 1) = '\0';

    return total_printed_chars;
}		/* -----  end of function printBmeMeasures  ----- */
/* }}} */

#ifdef  E_SCREEN
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  printBmeMeasuresOnScreen
 *  Description:  Print all measures on the screen
 *   Parameters:  
 *       Return:  
 * =====================================================================================
 */
/* --------- printBmeMeasuresOnScreen --------- {{{ */
    void
printBmeMeasuresOnScreen ()
{
    BME_UPDATE_ON_SCREEN (1, BME_NAME_1);
#if BME280_MEASURES > 1
    BME_UPDATE_ON_SCREEN (2, BME_NAME_2);
#if BME280_MEASURES > 2
    BME_UPDATE_ON_SCREEN (3, BME_NAME_3);
#if BME280_MEASURES > 3
    BME_UPDATE_ON_SCREEN (4, BME_NAME_4);
#endif     /* -----  BME280_MEASURES > 3  ----- */
#endif     /* -----  BME280_MEASURES > 2  ----- */
#endif     /* -----  BME280_MEASURES > 1  ----- */
}		/* -----  end of function printBmeMeasuresOnScreen  ----- */
/* }}} */
#endif     /* -----  E_SCREEN  ----- */
