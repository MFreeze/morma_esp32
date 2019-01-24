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


/*-----------------------------------------------------------------------------
 *  Macro processing
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Macro processing -------- */
// If USE_AUTOCONNECT has been set, we force the use of Wifi
#if USE_AUTOCONNECT
#define WIFI 1
#endif
/* }}} */



/*-----------------------------------------------------------------------------
 *  Libraries
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Libraries -------- */
/* Dépendances pour la connexion Wifi et upload vers InfluxDB */
#if WIFI 
#include "wifi_connect.h"
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
#include "escreen_print.h"
#endif

#if WEB_SERVER
#include <WebServer.h>
#include <AutoConnect.h>
#endif


#if  USE_SPIFFS
#include <esp_spiffs.h>
#define FILE_TAG "SPIFFS"
#endif     /* -----  USE_SPIFFS  ----- */

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


#if  USE_SPIFFS
#ifndef  NB_MEASURES_BEFORE_FILE_ROTATION
#define  NB_MEASURES_BEFORE_FILE_ROTATION    2000
#endif   /* ----- #ifndef NB_MEASURES_BEFORE_FILE_ROTATION  ----- */

static int write_counter = 0;
static int file_counter = 0;
char file_name[64];
#endif     /* -----  USE_SPIFFS  ----- */

byte mac[6];
/* }}} */

static int loop_counter = 0;


/*-----------------------------------------------------------------------------
 *  Functions
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Functions -------- */

#if WEB_SERVER
void handleDownload(){
    char *str, *str_parser;
    int buffer_size = 0, current_size = 0;

    FILE* f = fopen ("/spiffs/measures.txt", "a");
    if (!f) 
    {
        Serial.println ("Can't open SPIFFS file !\r\n");         
    }
    else {
        char buf[1024];

        str = (char *) malloc (1024 * sizeof (char));
        if (!str)
        {
            fclose (f);
            return Server.send (200, "text/plain", "Memory error");
        }
        buffer_size += 1024;
        str_parser = str;

        while (fgets (buf, 1024, f)) 
        {
            current_size += strlen (buf);
            if (current_size >= buffer_size)
            {
                char *tmp_str = str;
                tmp_str = (char *) realloc (str, buffer_size = 1024);
                if (!tmp_str)
                {
                    free (str);
                    fclose (f);
                    return Server.send (200, "text/plain", "Memory error");
                }

                buffer_size += 1024;
            }

            strncpy (str_parser, buf, 1024);
            str_parser += strlen (buf);
        }

        fclose (f);
        Server.send(200, "text/plain", str);
    }
}
#endif
/* }}} */

void setup()
{
    /* Initialisation du port série */
    Serial.begin(115200);

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

    /* 
#if WIFI || SEND_DATA_INFLUXDB
    // Store mac address for further use
    WiFi.macAddress(mac);
    Serial.print("MAC ");
    for (int i=0; i<6;  i++)
    {
        Serial.print(" : ");
        Serial.print(mac[i], HEX);
    }
    Serial.println();
#endif
 */

# if SEND_DATA_INFLUXDB
    initInfluxConnection ();
#endif
    
#if  E_SCREEN
    initScreen ();
#endif     /* -----  E_SCREEN  ----- */


#if  USE_SPIFFS
    ESP_LOGI(FILE_TAG, "Initializing SPIFFS");
    
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 50,
      .format_if_mount_failed = true
    };
    
    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(FILE_TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(FILE_TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(FILE_TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    snprintf (file_name, 64, "/spiffs/measures_%d.txt", write_counter);
#endif     /* -----  USE_SPIFFS  ----- */
    
    Serial.println(String("SDK:") + String(ESP.getSdkVersion()));
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

#if  E_SCREEN
    updateDsScreenValue ();
#endif     /* -----  E_SCREEN  ----- */
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

#if  E_SCREEN
    updateBmeScreenValues ();
#endif     /* -----  E_SCREEN  ----- */
#endif     /* -----  BME280_MEASURES  ----- */

#if SEND_DATA_INFLUXDB
    /* Write data to influxDB */
    sendInfluxData ();
#endif


    if (++loop_counter == SAVED_MEASURE_PERIOD)
    {
#if  USE_SPIFFS
        // Use POSIX and C standard library functions to work with files.
        // First create a file.
        ESP_LOGI(FILE_TAG, "Opening file <%s>.", file_name);
        FILE* f = fopen(file_name, "a");
        if (f == NULL) {
            ESP_LOGE(FILE_TAG, "Failed to open file for appending");
            return;
        }
        fprintf(f, "%s\n", str_result);
        fclose(f);
        ESP_LOGI(FILE_TAG, "File written");

        if (++write_counter >= NB_MEASURES_BEFORE_FILE_ROTATION)
        {
            write_counter = 0;
            snprintf (file_name, 64, "/spiffs/measures_%d.txt", ++file_counter);
            ESP_LOGI(FILE_TAG, "File rotation, new file name: %s.", file_name);
        }

#endif     /* -----  USE_SPIFFS  ----- */

        loop_counter = 0;
    }


#if  E_SCREEN
    updateScreen ();
#endif   /* ----- E_SCREEN  ----- */

    delay(MEASURE_DELAY * 1000); 
}

