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
#include <Fonts/FreeSans9pt7b.h>
#include <GxGDEP015OC1/GxGDEP015OC1.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

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
 *  Constants
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Constants -------- */

#ifndef  ESCREEN_OFFSET
#define  ESCREEN_OFFSET  14
#endif   /* ----- #ifndef ESCREEN_OFFSET  ----- */
/* }}} */


/*-----------------------------------------------------------------------------
 *  EScreen variables
 *-----------------------------------------------------------------------------*/
/* {{{ -------- EScreen variables -------- */
static display_sensor_t monitored_sensors[BME280_MEASURES + DS18B20_MEASURES];
static int currently_monitored = 0;
static int nb_lines = 0;
static GxIO_Class io(SPI, TFT_CS, TFT_DC, TFT_RST);
static GxEPD_Class display(io, TFT_RST, MISO_BUSY);
static const GFXfont* f = &FreeSans9pt7b;
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
 *                int label_in_title: indicates whether the label of the associated 
 *                      measures should be printed next to the sensor title
 *       Return:  ESCREEN_SUCCESS on success, ESCREEN_NO_MEM if malloc failed
 * =====================================================================================
 */
/* --------- addNewSensorToScreen --------- {{{ */
    int
addNewSensorToScreen (const char *label, int nb_measures, int label_in_title)
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
    monitored_sensors[currently_monitored].measure_label_in_title = label_in_title;

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


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  initScreen
 *  Description:  Initializes screen and compute the sizes and the positions of each
 *                  measures
 *   Parameters:  
 *       Return:  
 * =====================================================================================
 */
/* --------- initScreen --------- {{{ */
    void
initScreen ()
{
    int i, j;
    int nb_required_lines = 0;
    int line_height;
    int last_uncomplete_line_position = -1;
    int current_y_position = 0;
    int current_x_position = 0;
    int measure_w = 0;
    char measure_str[ESCREEN_MAX_STR_SIZE];
    display_sensor_t *cur_sensor = NULL;
    display_measure_t *cur_measure = NULL;

    for (i = 0; i < currently_monitored; i++)
    {
        switch (monitored_sensors[i].cur_monitored_measures)
        {
            case 0:
                break;
            case 1:
                nb_required_lines += 1;
                break;
            default:
                nb_required_lines += 2;
                break;
        }
    }

    nb_lines = (nb_required_lines + 1) / 2;

    // XXX At this time every line has the same height
    line_height = (GxGDEP015OC1_Y_PIXELS - 1) / nb_lines;

    for (i = 0, cur_sensor = monitored_sensors; i < currently_monitored; i++, cur_sensor++)
    {
        current_x_position = 0;
        if (cur_sensor->cur_monitored_measures == 0)
            continue;

        if (cur_sensor->cur_monitored_measures == 1)
        {
            measure_w = GxGDEP015OC1_X_PIXELS / 2;
            if (last_uncomplete_line_position != -1)
            {
                cur_sensor->x = measure_w;
                cur_sensor->y = last_uncomplete_line_position;

                cur_sensor->measures[0].x = measure_w;
                cur_sensor->measures[0].y = last_uncomplete_line_position + line_height / 2;
                last_uncomplete_line_position = -1;
            }
            else
            {
                cur_sensor->x = 0;
                cur_sensor->y = current_y_position;

                cur_sensor->measures[0].x = 0;
                cur_sensor->measures[0].y = current_y_position + line_height / 2;
                last_uncomplete_line_position = current_y_position;
                current_y_position += line_height;
            }

            cur_sensor->h = line_height / 2;
            cur_sensor->w = measure_w;

            cur_sensor->measures[0].h = line_height / 2;
            cur_sensor->measures[0].w = measure_w;
        }
        else
        {
            measure_w = GxGDEP015OC1_X_PIXELS / cur_sensor->cur_monitored_measures;

            cur_sensor->x = current_x_position;
            cur_sensor->y = current_y_position;
            cur_sensor->w = GxGDEP015OC1_X_PIXELS;
            cur_sensor->h = line_height / 2;

            for (j = 0, cur_measure = cur_sensor->measures; j < cur_sensor->cur_monitored_measures; j++, cur_measure++, current_x_position += measure_w)
            {
                cur_measure->x = current_x_position;
                cur_measure->y = current_y_position + line_height/2;
                cur_measure->w = measure_w;
                cur_measure->h = line_height / 2;
            }
            current_y_position += line_height;
        }
    }

    display.init();

    display.drawExampleBitmap(gImage_VBsplash, 0, 0, 200, 200, GxEPD_BLACK);
    display.update();
    delay(2000);

    display.fillScreen (GxEPD_WHITE);
    display.update ();
    delay (1000);
    
    // Print fix display
    for (i = 0, cur_sensor = monitored_sensors; i < currently_monitored; i++, cur_sensor++)
    {
        if (cur_sensor->measure_label_in_title)
        {
            int remaining_size = ESCREEN_MAX_STR_SIZE;
            int cur_written = 0;
            char *str_parser = measure_str;

            cur_written = snprintf (str_parser, remaining_size, "%s (", cur_sensor->label);
            remaining_size -= cur_written;
            str_parser += cur_written;
            // TODO check that string is note truncated
            for (j = 0, cur_measure = cur_sensor->measures; j < cur_sensor->cur_monitored_measures; j++, cur_measure++)
            {
                cur_written = snprintf (str_parser, remaining_size, "%s,", cur_measure->label);
                remaining_size -= cur_written;
                str_parser += cur_written;
            }

            *(str_parser - 1) = ')';
        }
        else
        {
            snprintf (measure_str, ESCREEN_MAX_STR_SIZE, "%s", cur_sensor->label);
        }
        PRINT_STRING_ON_SCREEN (display, 
                                f, 
                                measure_str, 
                                ESCREEN_OFFSET, 
                                cur_sensor->x,
                                cur_sensor->y,
                                cur_sensor->w,
                                cur_sensor->h,
                                GxEPD_BLACK,
                                GxEPD_WHITE);
        SENSOR_LOGI (DISPLAY_NAME, 
                     "Writing %s at (%d, %d) in r(%d, %d).\n", 
                     cur_sensor->label, 
                     cur_sensor->x, 
                     cur_sensor->y, 
                     cur_sensor->w, 
                     cur_sensor->h);

        for (j = 0, cur_measure = cur_sensor->measures; j < cur_sensor->cur_monitored_measures; j++, cur_measure++)
        {
            snprintf (measure_str, ESCREEN_MAX_STR_SIZE, "%s: %.1f", cur_measure->label, 0.0);
            PRINT_STRING_ON_SCREEN (display, 
                                    f, 
                                    measure_str, 
                                    ESCREEN_OFFSET, 
                                    cur_measure->x,
                                    cur_measure->y,
                                    cur_measure->w,
                                    cur_measure->h,
                                    GxEPD_BLACK,
                                    GxEPD_WHITE);
            SENSOR_LOGI (DISPLAY_NAME, 
                         "Writing %s at (%d, %d) in r(%d, %d).\n", 
                         measure_str, 
                         cur_measure->x, 
                         cur_measure->y, 
                         cur_measure->w, 
                         cur_measure->h);
        }
    }
}		/* -----  end of function initScreen  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  updateMeasure
 *  Description:  Update a measure on a the screen
 *   Parameters:  const char *sensor_label: the label of the sensor
 *                const char *measure_label: the label of the measure
 *                float value: the value to print
 *       Return:  ESCREEN_SUCCESS on success, ESCREEN_SENSOR_NOT_FOUND if the sensor
 *                  has not been registered yet or ESCREEN_MEASURE_NOT_FOUND if the
 *                  measure has not been registered yet.
 * =====================================================================================
 */
/* --------- updateMeasure --------- {{{ */
    int
updateMeasure (const char *sensor_label, const char *measure_label, float value)
{
    int i, j;
    display_sensor_t *cur_sensor = NULL;
    display_measure_t *cur_measure = NULL;
    char measure_str[ESCREEN_MAX_STR_SIZE];

    // Look for the requested measure
    for (i = 0, cur_sensor = monitored_sensors; i < currently_monitored; i++, cur_sensor++)
    {
        if (!strcmp (cur_sensor->label, sensor_label))
        {
            for (j = 0, cur_measure = cur_sensor->measures; j < cur_sensor->cur_monitored_measures; j++, cur_measure++)
            {
                if (!strcmp (cur_measure->label, measure_label))
                {
                    if (!cur_sensor->measure_label_in_title)
                    {
                        snprintf (measure_str, ESCREEN_MAX_STR_SIZE, "%s: %.1f", cur_measure->label, value);
                    }
                    else
                    {
                        snprintf (measure_str, ESCREEN_MAX_STR_SIZE, "%.1f", value);
                    }
                    PRINT_STRING_ON_SCREEN (display, 
                                            f, 
                                            measure_str, 
                                            ESCREEN_OFFSET, 
                                            cur_measure->x,
                                            cur_measure->y,
                                            cur_measure->w,
                                            cur_measure->h,
                                            GxEPD_BLACK,
                                            GxEPD_WHITE);
                    SENSOR_LOGI (DISPLAY_NAME, 
                                "Writing %s at (%d, %d) in r(%d, %d).\n", 
                                measure_str, 
                                cur_measure->x, 
                                cur_measure->y, 
                                cur_measure->w, 
                                cur_measure->h);

                    return ESCREEN_SUCCESS;
                }
            }

            SENSOR_LOGW (DISPLAY_NAME, "%s measure not found for sensor %s.\n", measure_label, sensor_label);
            return ESCREEN_MEASURE_NOT_FOUND;
        }
    }

    SENSOR_LOGW (DISPLAY_NAME, "%s sensor not found.\n", sensor_label);
    return ESCREEN_SENSOR_NOT_FOUND;
}		/* -----  end of function updateMeasure  ----- */
/* }}} */
/* }}} */
