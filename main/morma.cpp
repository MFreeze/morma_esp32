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
 *  Fix multiple WiFi.h libraries
 *  Fix problem of coexisting WiFi + DS18B20 sensor
 *  Calibrate values of HL-69
 *  Integrate NA values (no value from sensor)
 *  Disconnect and reconnect WiFi only when necessary (send values)
 *  Get time each days
 */

#include "config.h"
#include "constants.h"

/*-----------------------------------------------------------------------------
 *  Libraries
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Libraries -------- */
/* Dépendances pour la connexion Wifi et upload vers InfluxDB */
#if WIFI || SEND_DATA_INFLUXDB || WEB_SERVER
#include <WiFi.h>
#endif

#if SEND_DATA_INFLUXDB
#include "InfluxArduino.hpp"
#include "InfluxCert.hpp"
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

#include <esp_log.h>
/* }}} */



/*-----------------------------------------------------------------------------
 *  Constants
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Constants -------- */

#if WIFI || SEND_DATA_INFLUXDB
const char WIFI_NAME[] = "Ze_Guitounz_Box";
const char WIFI_PASS[] = "j41m3tr0pl3tr3sm4uv41sp4t31";
#endif

#if SEND_DATA_INFLUXDB
// InfluxArduino influx;

//connection/ database stuff that needs configuring
const char INFLUX_DATABASE[] = "valuebugs_test";
const char INFLUX_IP[] = "influx.valuebugs.be";
const char INFLUX_USER[] = "etienne";
const char INFLUX_PASS[] = "6xQH6d0SDB6ttYJXGX04";
const char INFLUX_MEASUREMENT[] = "test_ESP32_20180713";
#endif

#if SOIL_MEASURES
//Inside HL-69
const int soilRead = 26;
#endif

#if WEB_SERVER
WebServer Server;
AutoConnect Portal(Server);
#endif

byte mac[6];
/* }}} */




#if SOIL_MEASURES
int hSubs = 0;
#endif


/*-----------------------------------------------------------------------------
 *  Functions
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Functions -------- */

#if WEB_SERVER
void rootPage() {
    char content[] = "Hello, world";
    Server.send(200, "text/plain", content);
}
#endif
/* }}} */

void setup()
{
    int nb_ds18b20;
    /* Initialisation du port série */
    Serial.begin(115200);

#if BME280_MEASURES
    initBmeSensors ();
#endif

#if  DS18B20_MEASURES
    nb_ds18b20 = discoverDsSensors ();
#endif     /* -----  DS18B20_MEASURES  ----- */

#if SOIL_MEASURES
    // Substrate hygrometry
    pinMode (soilRead, INPUT); //soilRead will input the value from pin 26
    hSubs = analogRead(soilRead);
#endif

#if WEB_SERVER
    Server.on("/", rootPage);
    if (Portal.begin()) {
        Serial.println("WiFi connected: " + WiFi.localIP().toString());
    }
#endif

#if WIFI || SEND_DATA_INFLUXDB
#if WEB_SERVER == 0
    WiFi.begin(WIFI_NAME, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected!");

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

#if SEND_DATA_INFLUXDB
    // try connection with influxdb
    influx.configure(INFLUX_DATABASE, INFLUX_IP); //third argument (port number) defaults to 8086
    influx.authorize(INFLUX_USER, INFLUX_PASS);   //if you have set the Influxdb .conf variable auth-enabled to true, uncomment this
    // influx.addCertificate(ROOT_CERT);             //uncomment if you have generated a CA cert and copied it into InfluxCert.hpp
    Serial.print("Using HTTPS: ");
    Serial.println(influx.isSecure()); //will be true if you've added the InfluxCert.hpp file.
    // WiFi.mode(WIFI_OFF);
#endif

#endif
    
#if  E_SCREEN
    initScreen ();
#endif     /* -----  E_SCREEN  ----- */

    Serial.println(String("SDK:") + String(ESP.getSdkVersion()));
}

void loop()
{
    char tags[16];
    char fields[256];
    char str_result[BUFFER_SIZE];
    int remaining_size = BUFFER_SIZE;
    int cur_written = 0;
    int first = 1;
    char *str_parser = str_result;
    char formatTags[] = "read_all=%s";
    char formatFields[] = ""
#if SOIL_MEASURES
        "substrateHumidity=%0.3f,"
#endif
        END_OF_MEASURE_LINE;

    char readingStatus[] = "true";

#if SOIL_MEASURES
    int hSubs = 0;
#endif

#if WEB_SERVER
    Portal.handleClient();
#endif


#if DS18B20_MEASURES
    // get values from DS18S20
    readOneWireTemp ();
    cur_written = printDsMeasures (str_parser, remaining_size, first);
    remaining_size -= cur_written;
    first = 0;
    str_parser += cur_written;

    printf("%s\n", str_result);

#ifdef  E_SCREEN
    printDsMeasuresOnScreen ();
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

#ifdef  E_SCREEN
    printBmeMeasuresOnScreen ();
#endif     /* -----  E_SCREEN  ----- */
#endif

    sprintf(tags, formatTags, readingStatus);
    sprintf(fields, formatFields
#if SOIL_MEASURES
            ,hSubs 
#endif
           );

#if SEND_DATA_INFLUXDB
    /* Write data to influxDB */
    if (!influx.write(INFLUX_MEASUREMENT, tags, fields))
    {
        Serial.print("error: ");
        Serial.println(influx.getResponse());
    }
#endif

    Serial.print (fields);

    delay(5000); //Delay 5 sec.
}

