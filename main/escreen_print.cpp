/*
 * =====================================================================================
 *
 *       Filename:  escreen_print.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/11/2018 23:53:23
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
#include "escreen_print.h"

#include <string.h>
/* TODO Check whether this is required... */
// #include <GxGDEP015OC1/GxGDEP015OC1.cpp>
// #include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
// #include <GxIO/GxIO.cpp>

/*-----------------------------------------------------------------------------
 *  Log Macros
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Log Macros -------- */
#define DISPLAY_NAME "EScreen"
/* }}} */

/*-----------------------------------------------------------------------------
 *  Pin Declaration
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Pin Declaration -------- */
#ifndef  TFT_CS
#define  TFT_CS      14
#endif   /* ----- #ifndef TFT_CS  ----- */


#ifndef  SCK_CLK
#define  SCK_CLK     18
#endif   /* ----- #ifndef SCK_CLK  ----- */

#ifndef  MISO_BUSY
#define  MISO_BUSY   19
#endif   /* ----- #ifndef MISO_BUSY  ----- */

#ifndef  MOSI_DIN
#define  MOSI_DIN    23
#endif   /* ----- #ifndef MOSI_DIN  ----- */

#ifndef  TFT_DC
#define  TFT_DC      27
#endif   /* ----- #ifndef TFT_DC  ----- */

#ifndef  TFT_RST
#define  TFT_RST     33
#endif   /* ----- #ifndef TFT_RST  ----- */
/* }}} */

#define PRINT_STRING_ON_SCREEN(display, font, string, offset, x, y, w, h, fgcol, bgcol) do {\
    display.setRotation(45); \
    display.setFont(font); \
    display.setTextColor(fgcol); \
    display.fillRect(x, y, w, h, bgcol); \
    display.setCursor(x, y + offset); \
    display.print(string); \
    display.updateWindow(x, y, w, h, true); \
} while (0);



/*-----------------------------------------------------------------------------
 *  EScreen variables
 *-----------------------------------------------------------------------------*/
/* {{{ -------- EScreen variables -------- */
static display_sensor_t monitored_sensors[BME280_MEASURES + DS18B20_MEASURES];
static int currently_monitored = 0;
/* }}} */

/*-----------------------------------------------------------------------------
 *  Functions
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Functions -------- */
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  addNewSensorToScreen
 *  Description:  Add a new sensor to the list of displayed sensor
 *   Parameters:  const char *label: the label of the sensor to monitor
 *                int nb_measures: the number of associated measures
 *       Return:  ESCREEN_SUCCESS on success, ESCREEN_NO_MEM if malloc failed
 * =====================================================================================
 */
/* --------- addNewSensorToScreen --------- {{{ */
    int
addNewSensorToScreen (const char *label, int nb_measures)
{
    // Copy the label into the structure
    strncpy (monitored_sensors[currently_monitored].label, label, ESCREEN_MAX_STR_SIZE);
    // Ensure that the string is null terminated:
    monitored_sensors[currently_monitored].label[ESCREEN_MAX_STR_SIZE - 1] = '\0';


    // Initialize the measures
    monitored_sensors[currently_monitored].measures = (display_measure_t *) malloc (nb_measures * sizeof (display_measure_t));
    if (!monitored_sensors[currently_monitored].measures)
    {
        SENSOR_LOGE (DISPLAY_NAME, "Unable to allocate memory for %s sensor's measures.\n", label);
        return ESCREEN_NO_MEM;
    }

    monitored_sensors[currently_monitored].nb_measures = nb_measures;
    monitored_sensors[currently_monitored].cur_monitored_measures = 0;

    currently_monitored++;

    return ESCREEN_SUCCESS;
}		/* -----  end of function addNewSensorToScreen  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  addNewMeasureToSensorDisplay
 *  Description:  Add monitoring of a new measure to the given sensor
 *   Parameters:  const char *sensor_label: the sensor associated to the measure
 *                const char *measure_label: the label of the measure
 *                const char *unit: the unit of the measure
 *       Return:  ESCREEN_SUCCESS on success, ESCREEN_TOO_MANY_MEASURES if the maximum
 *                  number of measures has already been reached, 
 *                  ESCREEN_SENSOR_NOT_FOUND if the sensor is not monitored
 * =====================================================================================
 */
/* --------- addNewMeasureToSensorDisplay --------- {{{ */
    int
addNewMeasureToSensorDisplay (const char *sensor_label, const char *measure_label, const char *unit)
{
    int i;
    display_measure_t *cur_measure = NULL;
    display_sensor_t *cur_sensor = NULL;

    for (i = 0; i < currently_monitored; i++)
    {
        if (!strncmp (sensor_label, monitored_sensors[i].label, ESCREEN_MAX_STR_SIZE - 1))
        {
            cur_sensor = monitored_sensors + i;
            break;
        }
    }

    if (!cur_sensor)
    {
        SENSOR_LOGW (DISPLAY_NAME, "Sensor %s has not been yet registered...\n", sensor_label);
        return ESCREEN_SENSOR_NOT_FOUND;
    }

    if (cur_sensor->cur_monitored_measures == cur_sensor->nb_measures)
    {
        SENSOR_LOGW (DISPLAY_NAME, 
                     "The max number of registered measures for sensor %s has already been reached.\n",
                     sensor_label);
        return ESCREEN_TOO_MANY_MEASURES;
    }

    cur_measure = cur_sensor->measures + cur_sensor->cur_monitored_measures;

    // Fill the measure structure
    strncpy (cur_measure->label, measure_label, ESCREEN_MAX_STR_SIZE);
    cur_measure->label[ESCREEN_MAX_STR_SIZE - 1] = '\0';

    strncpy (cur_measure->unit, unit, ESCREEN_MAX_STR_SIZE);
    cur_measure->unit[ESCREEN_MAX_STR_SIZE - 1] = '\0';

    cur_sensor->cur_monitored_measures++;

    return ESCREEN_SUCCESS;
}		/* -----  end of function addNewMeasureToSensorDisplay  ----- */
/* }}} */
/* }}} */
