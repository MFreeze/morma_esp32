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
#include "debug.h"
#include "local_bme280.h"
#include "screen_macros.h"

#include <stdlib.h>
#include <esp_log.h>

// TODO register only bme that plugged.

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


#ifndef  BME_LABEL_IN_TITLE
#define  BME_LABEL_IN_TITLE     1
#endif   /* ----- #ifndef BME_LABEL_IN_TITLE  ----- */

#ifndef  BME_TEMP_LABEL
#define  BME_TEMP_LABEL "T"
#endif   /* ----- #ifndef BME_TEMP_LABEL  ----- */

#ifndef  BME_TEMP_UNIT
#define  BME_TEMP_UNIT "C"
#endif   /* ----- #ifndef BME_TEMP_UNIT  ----- */

#ifndef  BME_TEMP_ERROR_VAL
#define  BME_TEMP_ERROR_VAL     -280
#endif   /* ----- #ifndef BME_TEMP_ERROR_VAL  ----- */

#ifndef  BME_PRESSURE_LABEL
#define  BME_PRESSURE_LABEL "P"
#endif   /* ----- #ifndef BME_PRESSURE_LABEL  ----- */

#ifndef  BME_PRESSURE_UNIT
#define  BME_PRESSURE_UNIT "hPa"
#endif   /* ----- #ifndef BME_PRESSURE_UNIT  ----- */

#ifndef  BME_PRESSURE_ERROR_VAL
#define  BME_PRESSURE_ERROR_VAL     0
#endif   /* ----- #ifndef BME_PRESSURE_ERROR_VAL  ----- */

#ifndef  BME_HUMIDITY_LABEL
#define  BME_HUMIDITY_LABEL "H"
#endif   /* ----- #ifndef BME_HUMIDITY_LABEL  ----- */

#ifndef  BME_HUMIDITY_UNIT
#define  BME_HUMIDITY_UNIT "%"
#endif   /* ----- #ifndef BME_HUMIDITY_UNIT  ----- */

#ifndef  BME_HUMIDITY_ERROR_VAL
#define  BME_HUMIDITY_ERROR_VAL     -1
#endif   /* ----- #ifndef BME_HUMIDITY_ERROR_VAL  ----- */
/* }}} */


/*-----------------------------------------------------------------------------
 *  Sensor Name
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Sensor Name -------- */
#ifndef  BME_NAME_1
#define	BME_NAME_1 "BME_1"
#endif     /* -----  not BME_NAME_1  ----- */

#ifndef  BME_NAME_2
#define	BME_NAME_2 "BME_2"
#endif     /* -----  not BME_NAME_2  ----- */

#ifndef  BME_NAME_3
#define	BME_NAME_3 "BME_3"
#endif     /* -----  not BME_NAME_3  ----- */

#ifndef  BME_NAME_4
#define	BME_NAME_4 "BME_4"
#endif     /* -----  not BME_NAME_4  ----- */

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
        SENSOR_LOGE (BME_NAME, "Cannot find sensor %s. Check wiring!", n);\
    }\
    bme_##x.t = BME_TEMP_ERROR_VAL;\
    bme_##x.h = BME_HUMIDITY_ERROR_VAL;\
    bme_##x.p = BME_PRESSURE_ERROR_VAL;\
    bme_##x.missing = 0;\
    bme_##x.name = n;\
} while (0);

#define BME_ADD_TO_SCREEN(name) do {\
    int __local_bme_ret_val = 1; \
    const char *__local_bme_measures[6] = { BME_TEMP_LABEL, BME_TEMP_UNIT, \
                                            BME_HUMIDITY_LABEL, BME_HUMIDITY_UNIT, \
                                            BME_PRESSURE_LABEL, BME_PRESSURE_UNIT }; \
    int __local_bme_lengths[6] = { (int) strlen (BME_TEMP_LABEL), \
                                   (int) strlen (BME_TEMP_UNIT), \
                                   (int) strlen (BME_HUMIDITY_LABEL), \
                                   (int) strlen (BME_HUMIDITY_UNIT), \
                                   (int) strlen (BME_PRESSURE_LABEL), \
                                   (int) strlen (BME_PRESSURE_UNIT) }; \
    float __local_bme_error[3] = { BME_TEMP_ERROR_VAL, \
                                   BME_HUMIDITY_ERROR_VAL, \
                                   BME_PRESSURE_ERROR_VAL }; \
    RegisterSensor (__local_bme_ret_val, name, strlen (name), __local_bme_measures, __local_bme_lengths, __local_bme_error, 3); \
    if (!__local_bme_ret_val) \
    { \
        SENSOR_LOGE (BME_NAME, "Unable to register BME %s", name); \
    } \
    else \
    { \
        SENSOR_LOGI (BME_NAME, "BME %s registered", name); \
    } \
} while (0);

#define BME_READ_MEASURES(x, addr, w, rcode) do {\
    int __local_bme_ret_val = 1; \
    if (!bme_##x.interface.begin (addr, &w))\
    {\
        bme_##x.missing = 1;\
        bme_##x.t = BME_TEMP_ERROR_VAL; \
        bme_##x.h = BME_HUMIDITY_ERROR_VAL; \
        bme_##x.p = BME_PRESSURE_ERROR_VAL; \
        rcode |= BME280_ERROR_##x;\
        SENSOR_LOGE (BME_NAME, "Cannot find sensor %s. Check wiring!", bme_##x.name);\
    }\
    else\
    {\
        bme_##x.t = bme_##x.interface.readTemperature ();\
        bme_##x.h = bme_##x.interface.readHumidity ();\
        bme_##x.p = bme_##x.interface.readPressure () / 100;\
        bme_##x.missing = 0;\
    }\
    UpdateScreenMeasure (__local_bme_ret_val, bme_##x.name, BME_TEMP_LABEL, bme_##x.t); \
    if (!__local_bme_ret_val) \
    { \
        SENSOR_LOGE (BME_NAME, "Cannot update temp value for %s.", bme_##x.name); \
    } \
    else \
    { \
        SENSOR_LOGI (BME_NAME, "Temp updated with val %lf for %s.", bme_##x.t, bme_##x.name); \
    } \
    UpdateScreenMeasure (__local_bme_ret_val, bme_##x.name, BME_HUMIDITY_LABEL, bme_##x.h); \
    if (!__local_bme_ret_val) \
    { \
        SENSOR_LOGE (BME_NAME, "Cannot update humidity value for %s.", bme_##x.name); \
    } \
    else \
    { \
        SENSOR_LOGI (BME_NAME, "Humidity updated with val %lf for %s.", bme_##x.h, bme_##x.name); \
    } \
    UpdateScreenMeasure (__local_bme_ret_val, bme_##x.name, BME_PRESSURE_LABEL, bme_##x.p); \
    if (!__local_bme_ret_val) \
    { \
        SENSOR_LOGE (BME_NAME, "Cannot update pressure value for %s.", bme_##x.name); \
    } \
    else \
    { \
        SENSOR_LOGI (BME_NAME, "Pressure updated with val %lf for %s.", bme_##x.p, bme_##x.name); \
    } \
} while (0);

#define BME_PRINT_TO_STR(x, parser, size, cur_written, total_written, sep) do {\
    if (!bme_##x.missing) \
    {\
        cur_written = snprintf (parser, \
                                size, \
                                "%s_t=%." MEASURE_PRECISION "f%c"\
                                "%s_h=%." MEASURE_PRECISION "f%c",\
                                bme_##x.name,\
                                bme_##x.t,\
                                STR_SEP,\
                                bme_##x.name,\
                                bme_##x.h,\
                                STR_SEP);\
        total_written += cur_written;\
        parser += cur_written;\
        if (cur_written >= size)\
        {\
            SENSOR_LOGW (BME_NAME, "Buffer has insufficient space to store all BME values for %s. Consider resizing.", bme_##x.name);\
            *(parser - 1) = '\0';\
            return total_written;\
        }\
        size -= cur_written;\
    }\
} while (0);
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

