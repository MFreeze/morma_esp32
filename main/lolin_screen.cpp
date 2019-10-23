/*
 * =====================================================================================
 *
 *       Filename:  lolin_screen.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21/10/2019 14:30:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "screen_macros.h"
#include "lolin_screen.h"
#include "debug.h"

/*-----------------------------------------------------------------------------
 *  Pin Declaration
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Pin Declaration -------- */
#ifndef  EPD_CS
#define  EPD_CS      14
#endif   /* ----- #ifndef EPD_CS  ----- */

#ifndef  EPD_CLK
#define  EPD_CLK     18
#endif   /* ----- #ifndef EPD_CLK  ----- */

#ifndef  EPD_BUSY
#define  EPD_BUSY   -1
#endif   /* ----- #ifndef EPD_BUSY  ----- */

#ifndef  EPD_MOSI
#define  EPD_MOSI    23
#endif   /* ----- #ifndef EPD_MOSI  ----- */

#ifndef  EPD_DC
#define  EPD_DC      27
#endif   /* ----- #ifndef EPD_DC  ----- */

#ifndef  EPD_RST
#define  EPD_RST     33
#endif   /* ----- #ifndef EPD_RST  ----- */
/* }}} */


/*-----------------------------------------------------------------------------
 *  Data Structures
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Data Structures -------- */

/*
 * =====================================================================================
 *    Structure:  sensor_signature
 *  Description:  A structure containing the signature of the sensor 
 *      Members:  * char *name: the name of the sensor
 *                * int len: the length of the name string
 *                * int nb_measures: the number of measures
 * =====================================================================================
 */
struct sensor_signature {
    char *name;
    int len;
    int nb_measures;
};				/* ----------  end of struct sensor_signature  ---------- */

typedef struct sensor_signature sensor_signature_t;



/*
 * =====================================================================================
 *    Structure:  sensor_measure
 *  Description:  A structure containing information about sensor measures 
 *      Members:  * const char *label: the label of the measure
 *                * int label_len: the length of the label string
 *                * const char *unit: the symbol for the unit
 *                * int unit_len; the length of the unit string
 *                * float val: the value of the measure
 *                * float default_err: the value of a non valid measure
 * =====================================================================================
 */
struct sensor_measure {
    char *label, *unit;
    int label_len, unit_len;
    float val, default_err;
};				/* ----------  end of struct sensor_measure  ---------- */

typedef struct sensor_measure sensor_measure_t;
/*
 * =====================================================================================
 *    Structure:  sensor_array
 *  Description:  An array of sensors 
 *      Members:  * sensor_signature_t *sensors; a pointer on the array
 *                * int nb_sensors: the number of tracked sensors
 *                * sensor_measure_t **measures: the bidimensional array of measure 
 *                      (one array per sensor)
 * =====================================================================================
 */
struct sensor_array {
    sensor_signature_t *sensors;
    sensor_measure_t **measures;
    int nb_sensors;
    int allocated_space;
};				/* ----------  end of struct sensor_array  ---------- */

typedef struct sensor_array sensor_array_t;
/* }}} */


LOLIN_IL3897 EPD(250, 122, EPD_MOSI, EPD_CLK, EPD_DC, EPD_RST, EPD_CS, EPD_BUSY);
int cur_nb_line = 0;
const int max_nb_line = 14;
sensor_array_t tracked_sensors;


#ifndef  STATUS_OFFSET
#define  STATUS_OFFSET      220
#endif   /* ----- #ifndef STATUS_OFFSET  ----- */


#ifndef  STATUS_SUCCESS
#define  STATUS_SUCCESS     "Ok"
#endif   /* ----- #ifndef STATUS_SUCCESS  ----- */


#ifndef  STATUS_ERROR
#define  STATUS_ERROR       "Fail"
#endif   /* ----- #ifndef STATUS_ERROR  ----- */

/*-----------------------------------------------------------------------------
 *  Internal Functions
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Internal Functions -------- */
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  printMessage
 *  Description:  Print a message on the screen 
 *   Parameters:  const char *messgae: the message to print
 *                int eol: a boolean that indicates whether the line should end or not
 *       Return:  
 * =====================================================================================
 */
/* --------- printMessage --------- {{{ */
    void
printMessage (const char *message, int eol, int clear, int refresh)
{
    if (cur_nb_line == max_nb_line || clear)
    {
        EPD.clearBuffer ();
        EPD.setCursor (0, 0);
        cur_nb_line = 0;
    }

    SCREEN_LOGI ("Writing %s at (%d, %d)", message, EPD.getCursorX (), EPD.getCursorY ());

    if (eol)
    {
        EPD.println (message);
    }
    else
    {
        EPD.print (message);
    }

    EPD.update ();
    if (refresh)
    {
        EPD.display ();
    }

    cur_nb_line += eol ? 1 : 0;
}		/* -----  end of function printMessage  ----- */
/* }}} */
/* }}} */


/*-----------------------------------------------------------------------------
 *  External Functions
 *-----------------------------------------------------------------------------*/
/* {{{ -------- External Functions -------- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  initScreen
 *  Description:  Initializes a screen 
 *   Parameters:  
 *       Return:  0 in case of error, non zero otherwise
 * =====================================================================================
 */
/* --------- initScreen --------- {{{ */
    int
initScreen ()
{
    EPD.begin ();
    EPD.clearBuffer ();

    EPD.setCursor (0, 0);
    EPD.setTextColor (EPD_BLACK);
    EPD.setTextSize (1);
    EPD.setTextWrap (false);

    EPD.display ();

    StartTracer ("Allocation structures pour affichage");
    tracked_sensors.sensors = (sensor_signature_t *) malloc (5 * sizeof (sensor_signature_t));
    if (! tracked_sensors.sensors)
    {
        StopTracer (0);
        SCREEN_LOGE ("Unable to allocate memory for sensors array");
        return 0;
    }

    tracked_sensors.measures = (sensor_measure_t **) malloc (5 * sizeof (sensor_measure_t *));
    if (! tracked_sensors.measures)
    {
        StopTracer (0);
        SCREEN_LOGE ("Unable to allocate memory for meausres array");
        free (tracked_sensors.sensors);
        return 0;
    }

    tracked_sensors.nb_sensors = 0;
    tracked_sensors.allocated_space = 5;

    StopTracer (1);
    return 1;
}		/* -----  end of function initScreen  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  clearScreen
 *  Description:  Start with a new white screen 
 *   Parameters:  
 *       Return:  
 * =====================================================================================
 */
/* --------- clearScreen --------- {{{ */
    void
clearScreen ()
{
    EPD.clearBuffer ();

    EPD.setCursor (0, 0);
    EPD.setTextColor (EPD_BLACK);
    EPD.setTextSize (1);
    EPD.setTextWrap (false);

    cur_nb_line = 0;

    EPD.display ();
}		/* -----  end of function clearScreen  ----- */
/* }}} */
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  initTracing
 *  Description:  Print the label of the current process 
 *   Parameters:  const char *label: the label of the task being performed
 *       Return:  
 * =====================================================================================
 */
/* --------- initTracing --------- {{{ */
    void
initTracing (const char *label)
{
    printMessage (label, 0, 0, 1);
}		/* -----  end of function initTracing  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  statusTracing
 *  Description:  Print the status of the terminated process 
 *   Parameters:  int status: 0 if fail anything else for success
 *       Return:  
 * =====================================================================================
 */
/* --------- statusTracing --------- {{{ */
    void
statusTracing (int status)
{
    int cur_y = EPD.getCursorY ();

    EPD.setCursor (STATUS_OFFSET, cur_y);

    printMessage (status ? STATUS_SUCCESS : STATUS_ERROR, 1, 0, 1);
}		/* -----  end of function statusTracing  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  registerSensor
 *  Description:  A function that register a new sensor 
 *   Parameters:  const char *name: the name of the sensor
 *                int len: the length of the string
 *                const char **measures_str: the labels and units stings of the measures.
 *                      The array contains 2 cases per measures tracked by the sensor.
 *                      They should be formatted in the following way:
 *                          neasures_str[0]: label of the first measure
 *                          neasures_str[1]: unit of the first measure
 *                          neasures_str[0]: label of the second measure
 *                          neasures_str[1]: unit of the second measure
 *                          ...
 *                int *lengths; the length of the previous strings, the array should be
 *                      formatted in the same way as the previous one
 *                float *error_vals: the error value for the measure
 *                int nb_meas: the number of measures performed by the sensor
 *       Return:  0 in case of error, 1 otherwise
 * =====================================================================================
 */
/* --------- registerSensor --------- {{{ */
    int
registerSensor (const char *name, 
                int len, 
                const char **measures_str,
                int *lengths,
                float *error_vals,
                int nb_meas)
{
    int i = 0, j = 0, ind = 0;
    int id = tracked_sensors.nb_sensors;
    sensor_signature_t *new_sensor = NULL;
    sensor_measure_t *new_measure = NULL;

    SCREEN_LOGW ("Registering %s", name);

    StartTracer ("Initialisation capteur: %s", name);
    
    // Realloc memory if needed
    if (id == tracked_sensors.allocated_space)
    {
        sensor_signature_t *tmp_sensor;
        sensor_measure_t **tmp_meas;
        int new_size = tracked_sensors.allocated_space * 2;

        SCREEN_LOGW ("Allocating new memory for sensors");
        tmp_sensor = (sensor_signature_t *) realloc (tracked_sensors.sensors, new_size * sizeof (sensor_signature_t));

        if (!tmp_sensor)
        {
            SCREEN_LOGE ("Unable to reallocate memory.");
            StopTracer (0);
            return 0;
        }

        tracked_sensors.sensors = tmp_sensor;

        tmp_meas = (sensor_measure_t **) realloc (tracked_sensors.measures, new_size * sizeof (sensor_measure_t *));
        if (!tmp_meas)
        {
            SCREEN_LOGE ("Unable to reallocate memory bis");
            StopTracer (0);
            return 0;
        }

        tracked_sensors.allocated_space = new_size;
    }

    // Register the sensor
    new_sensor = tracked_sensors.sensors + id;
    new_sensor->name = (char *) malloc ((len + 1) * sizeof(char));
    if (!new_sensor->name)
    {
        SCREEN_LOGE ("Unable to allocate memory for sensor name string");
        StopTracer (0);
        return 0;
    }
    strcpy (new_sensor->name, name);

    new_sensor->len = len;
    new_sensor->nb_measures = nb_meas;

    // Register the associated measures
    tracked_sensors.measures[id] = (sensor_measure_t *) malloc (nb_meas * sizeof (sensor_measure_t));
    new_measure = tracked_sensors.measures[id];
    if (!new_measure)
    {
        SCREEN_LOGE ("unable to allocate memory for sensor measures");
        free (new_sensor->name);
        StopTracer (0);
        return 0;
    }


    for (i = 0, ind = 0; i < nb_meas; i++, ind += 2)
    {
        sensor_measure_t *cur_meas = new_measure + i;

        cur_meas->label = (char *) malloc ((lengths[ind] + 1) * sizeof (char));
        if (!cur_meas->label)
        {
            SCREEN_LOGE ("Cannot allocate memory for measure %d name", i);
            for (j = 0; j < i; j++)
            {
                free (new_measure[j].label);
                free (new_measure[j].unit);
            }
            free (new_sensor->name);
            free (new_measure);
            StopTracer (0);
            return 0;
        }
        cur_meas->label_len = lengths[ind];
        strcpy (cur_meas->label, measures_str[ind]);

        cur_meas->unit	= (char *) malloc (sizeof(char) * (lengths[ind + 1] + 1));
        if (!cur_meas->unit) {
            SCREEN_LOGE ("Cannot allocate memory for measure %d unit", i);
            for (j = 0; j < i; j++)
            {
                free (new_measure[j].label);
                free (new_measure[j].unit);
            }
            free (new_measure[i].label);
            free (new_sensor->name);
            free (new_measure);
            StopTracer (0);
            return 0;
        }
        cur_meas->unit_len = lengths[ind + 1];
        strcpy (cur_meas->unit, measures_str[ind + 1]);

        cur_meas->default_err = error_vals[i];
        cur_meas->val = cur_meas->default_err;
    }

    tracked_sensors.nb_sensors++;

    SCREEN_LOGI ("New number of sensors: %d", tracked_sensors.nb_sensors);

    StopTracer (1);
    return 1;
}		/* -----  end of function registerSensor  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  updateScreenMeasure
 *  Description:  This function allows a sensor to update the value of one of its 
 *                  measure in the measure db of the screen library.
 *   Parameters:  const char *sensor_name: the name of the sensor
 *                const char *measure_name: the name of the measure
 *                float val: the new value of the masure
 *       Return:  0 in case of error, 1 otherwise
 * =====================================================================================
 */
/* --------- updateScreenMeasure --------- {{{ */
    int
updateScreenMeasure (const char *sensor_name, const char *measure_name, float val)
{
    int i = 0, j = 0;
    int keep_on_searching = 1;

    for (i = 0; keep_on_searching && i < tracked_sensors.nb_sensors; i++)
    {
        keep_on_searching = strcmp (sensor_name, tracked_sensors.sensors[i].name);
        if (!keep_on_searching)
        {
            int searching_measure = 1;
            for (j = 0; searching_measure && j < tracked_sensors.sensors[i].nb_measures; j++)
            {
                searching_measure = strcmp (measure_name, tracked_sensors.measures[i][j].label);
                if (!searching_measure)
                {
                    tracked_sensors.measures[i][j].val = val;
                    return 1;
                }
            }
        }
    }

    return 0;
}		/* -----  end of function updateScreenMeasure  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  printMeasures
 *  Description:  Print all the tracked measures on the screen 
 *   Parameters:  
 *       Return:  
 * =====================================================================================
 */
/* --------- printMeasures --------- {{{ */
    void
printMeasures ()
{
    int i = 0, j = 0;

    clearScreen ();

    for (i = 0; i < tracked_sensors.nb_sensors; i++)
    {
        char to_print[256] = {0}, buffer[256] = {0};
        char *str_p = to_print;
        size_t len;
        int space_left = 255;                       /* 256 minus the \0 */

        snprintf (buffer, 256, "%s =>", tracked_sensors.sensors[i].name);
        strncpy (str_p, buffer, space_left);

        len = strlen (buffer);
        str_p += len;
        space_left -= len;

        for (j = 0; j < tracked_sensors.sensors[i].nb_measures && space_left > 0; j++)
        {
            sensor_measure_t cur = tracked_sensors.measures[i][j];
            if (cur.val != cur.default_err)
            {
                snprintf (buffer, 256, " %s:%.1lf %s ", cur.label, cur.val, cur.unit);
            }
            else
            {
                snprintf (buffer, 256, " %s:NaN %s ", cur.label, cur.unit);
            }

            strncpy (str_p, buffer, space_left);
            len = strlen (buffer);
            str_p += len;
            space_left -= len;
            SCREEN_LOGI ("Mesures: %s", to_print);
        }

        printMessage (to_print, 1, 0, 0);
    }

    printMessage ("", 0, 0, 1);                 /* Refresh screen */
}		/* -----  end of function printMeasures  ----- */
/* }}} */
/* }}} */
