/*
 * =====================================================================================
 *
 *       Filename:  escreen_print.h
 *
 *    Description:  Header for the escreen related library
 *
 *        Version:  1.0
 *        Created:  01/11/2018 23:49:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef ESCREEN_PRINT_H
#define ESCREEN_PRINT_H
#include <GxEPD.h>
#include <GxGDEP015OC1/GxGDEP015OC1.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include "bitmap_VBsplashScreen.h"
#include "bitmap_measureBG.h"
#include "config.h"


/*
 * =====================================================================================
 *  Enumeration:  escreen_return_codes
 *  Description:  Errors of the escreen modules
 * =====================================================================================
 */
enum escreen_return_codes
{
    ESCREEN_SUCCESS = 0,
    ESCREEN_NO_MEM = 1,
    ESCREEN_TOO_MANY_MEASURES = 2,
    ESCREEN_SENSOR_NOT_FOUND = 4,
    ESCREEN_MEASURE_NOT_FOUND = 8
};				/* ----------  end of enum escreen_return_codes  ---------- */

typedef enum escreen_return_codes escreen_return_codes_t;

/*-----------------------------------------------------------------------------
 *  Structures
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Structures -------- */

/*
 * =====================================================================================
 *    Structure:  display_measure
 *  Description:  Structure containing every informations needed to print a measure
 *      Members:  * char label[ESCREEN_MAX_STR_SIZE]: the label of the measure
 *                * char unit|ESCREEN_MAX_STR_SIZE]:  the unit of the measure
 *                * int x: the abscissa of the top left corner of the rectangle
 *                      containing the measure
 *                * int y: the ordinate of the top left corner
 *                * int w: the width of the rectangle
 *                * int h: the height of the rectangle
 *                * float last_value: the last value for this measure, can be updated
 *                      thank to updateMeasure
 * =====================================================================================
 */
struct display_measure {
    char label[ESCREEN_MAX_STR_SIZE];
    char unit[ESCREEN_MAX_STR_SIZE];
    int x, y;
    int w, h;
    float last_value;
};				/* ----------  end of struct display_measure  ---------- */

typedef struct display_measure display_measure_t;


/*
 * =====================================================================================
 *    Structure:  display_sensor
 *  Description:  Structure contaning all the informations related to the display of 
 *                  sensor
 *      Members:  * char label[ESCREEN_MAX_STR_SIZE]: the name of the sensor (should
 *                      be unique)
 *                * display_measure_t *measures: an array containing every measure 
 *                      associated to the current sensor
 *                * int nb_measures: the number of measures supposed to be monitored
 *                * int cur_monitored_measures: the number of measures effectively
 *                      monitored            
 *                * int x, y, w, h: position, width and length of the rectangle
 *                      containing sensor name
 *                * int measure_label_in_title: indicates whether the label of the
 *                      associated measures should be printed next to the sensor title.
 * =====================================================================================
 */
struct display_sensor {
    char label[ESCREEN_MAX_STR_SIZE];
    display_measure_t *measures;
    int nb_measures;
    int cur_monitored_measures;
    int x, y, w, h;
    int measure_label_in_title;
};				/* ----------  end of struct display_sensor  ---------- */

typedef struct display_sensor display_sensor_t;
/* }}} */


/*-----------------------------------------------------------------------------
 *  Functions
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Functions -------- */
// Add a new sensor to the displau
int addNewSensorToScreen (const char *label, int nb_measures, int label_in_title = false);

// Add a new measure associated to a sensor
int addNewMeasureToSensorDisplay (const char *sensor_label, const char *meas_label, const char *unit);

// Initializes the screen
void initScreen ();

// Update measure
int updateMeasure (const char *sensor_label, const char *measure_label, float value);

// Update the screen
void updateScreen (int full_update=0);
/* }}} */

#endif
