/* WeatherStation for ValueBugs Project
 *  Written to be run on ESP32 board
 *  Connect to WiFi
 *  Get measures from sensors : 2 BME260 sensor (temperature/humidity/pressure) + 1 DS18B20 (temperature) + 1 HL-69 (moisture)
 *  Write values on influxdb
 *  Print values on screen
 *  
 *  
 */

/* TODO
 *  Use mac address as unique id for the weather station
 *  Get certificate for influxdb (see InfluxCert.hpp)
 *  Fix problem of coexisting WiFi + DS18B20 sensor
 *  Calibrate values of HL-69
 *  Integrate NA values (no value from sensor)
 *  Disconnect and reconnect WiFi only when necessary (send values)
 *  Get time each days
 */

#include "config.h"
#include "constants.h"
#include "screen_macros.h"
#include "debug.h"



/*-----------------------------------------------------------------------------
 *  Libraries
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Libraries -------- */
/* Dépendances pour la connexion Wifi et upload vers InfluxDB */
#if WIFI 
#include "wifi_connect.h"
#include <WebServer.h>
#endif

/* No need to explicitly force use of Wifi when using influxdb since the library 
 * already forces it.
 * */
#if SEND_DATA_INFLUXDB
#include "influx_data.h"
#endif

#if DS18B20_MEASURES
#include "ds18b20.h"
#endif


#if  BME280_MEASURES
#include "local_bme280.h"
#endif     /* -----  BME280_MEASURES  ----- */

#if E_SCREEN
/* Dépendances pour l'écran e-ink 1.54inch WaveShare*/
#include "lolin_screen.h"
#endif

#include <esp_err.h>
#include <esp_log.h>
#include <time.h>
/* }}} */



/*-----------------------------------------------------------------------------
 *  Constants
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Constants -------- */
#if SOIL_MEASURES
//Inside HL-69
const int soilRead = 26;
int hSubs = 0;
#endif


#ifndef  MEASURE_DELAY
#define  MEASURE_DELAY  60
#endif   /* ----- #ifndef MEASURE_DELAY  ----- */


#ifndef  SAVED_MEASURE_PERIOD
#define  SAVED_MEASURE_PERIOD  60
#endif   /* ----- #ifndef SAVED_MEASURE_PERIOD  ----- */


byte mac[6];
/* }}} */

/*-----------------------------------------------------------------------------
 *  Functions
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Functions -------- */

void setup()
{
    int ret_val;
    /* Initialisation du port série */
    Serial.begin(115200);

    InitScreen (ret_val);
    if (ret_val)
    {
        StartTracer ("Affichage");                  /* The display cannot be started before screen initialization */
        StopTracer (1);
    }
    else
    {
        // TODO Handle correctly the error
        SCREEN_LOGE ("Error while initializing the screen.");
    }

#if BME280_MEASURES
    initBmeSensors ();
#endif

#if  DS18B20_MEASURES
    discoverDsSensors ();
#endif     /* -----  DS18B20_MEASURES  ----- */

#if SOIL_MEASURES
    // Substrate hygrometry
    pinMode (soilRead, INPUT); //soilRead will input the value from pin 26
    hSubs = analogRead(soilRead);
#endif

#if WIFI
    initWifiConnection ();
#endif

# if SEND_DATA_INFLUXDB
    initInfluxConnection ();
#endif
    
    ESP_LOGI ("Serial", "SDK: %s", ESP.getSdkVersion ());
}

void loop()
{
    char str_result[BUFFER_SIZE];
    int remaining_size = BUFFER_SIZE;
    int cur_written = 0;
    int first = 1;
    char *str_parser = str_result;

#if SOIL_MEASURES
    int hSubs = 0;
#endif

#if WIFI
    wifiLoopRoutine ();
#endif

    cur_written = snprintf (str_parser, remaining_size, ESP_ID ",%u,", (unsigned int) time (NULL));
    str_parser += cur_written;
    if (cur_written >= remaining_size)
    {
        ESP_LOGW ("Main", "Buffer has insufficient space to store user and timestamp. Consider resizing.");
        *(str_parser - 1) = '\0';
    }
    remaining_size -= cur_written;
    first = 0;

#if DS18B20_MEASURES
    // get values from DS18S20
    readOneWireTemp ();
    cur_written = printDsMeasures (str_parser, remaining_size, first);
    remaining_size -= cur_written;
    first = 0;
    str_parser += cur_written;

    printf("%s\n", str_result);
#endif


#if SOIL_MEASURES
    /* Substrate hygrometry */
    hSubs = analogRead(soilRead); //read pin 26 then assign hSubs to that value
#endif

#if BME280_MEASURES
    readBmeMeasures ();
    cur_written = printBmeMeasures (str_parser, remaining_size, first);
    remaining_size -= cur_written;
    first = 0;
    str_parser += cur_written;

    printf("%s\n", str_result);

    delay (1000);
#endif     /* -----  BME280_MEASURES  ----- */

#if SEND_DATA_INFLUXDB
    /* Write data to influxDB */
    sendInfluxData ();
#endif

    ESP_LOGI ("Time", "%s", getFormattedNTPTime ());

    PrintMeasure ();

    delay(MEASURE_DELAY * 1000); 
}

