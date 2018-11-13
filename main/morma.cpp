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


#define NA_VAL -1000.0
#define USE_GxGDEP015OC1


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

#if BME280_MEASURES
/* Dépendances pour le bus I2C -> sonde BME280*/
#include <Wire.h>
#include <Adafruit_BME280.h>
#endif

#if E_SCREEN
/* Dépendances pour l'écran e-ink 1.54inch WaveShare*/
#include "escreen_print.h"

GxIO_Class io(SPI, TFT_CS, TFT_DC, TFT_RST);
GxEPD_Class display(io, TFT_RST, MISO_BUSY);
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

#if BME280_MEASURES
// Inside and outside DME260
/* Define pins for I2C bus*/
const int sclPin = 22;
const int sdaPin = 21;
#endif

#if DS18B20_MEASURES
/* DS18B20 - Broche du bus 1-Wire */
const byte BROCHE_ONEWIRE = 25;      // what pin DS18B20 is connected to

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




/*-----------------------------------------------------------------------------
 *  Sensors
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Sensors -------- */

#if BME280_MEASURES
Adafruit_BME280 bmeIn; // I2C BME 620 Inside 
Adafruit_BME280 bmeOut; // I2C BME 620 Outside
#endif
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
    /* Initialisation du port série */
    Serial.begin(115200);

#if BME280_MEASURES
    Wire.begin(sdaPin, sclPin);
    bmeIn.begin(0x76);
    bmeOut.begin(0x77);

    if (!bmeIn.begin(0x76)) {
        Serial.println("Could not find a valid BME280 IN sensor, check wiring!");
    }

    if (!bmeOut.begin(0x77)) {
        Serial.println("Could not find a valid BME280 OUT sensor, check wiring!");
    }
#endif

#if DS18B20_MEASURES
    //XXX is this code useful?
    /* delay(5000);
       OneWire ds(BROCHE_ONEWIRE);
       delay(5000);
       */
    /* Lit la température ambiante à ~1Hz */
    /*  if (getTemperature(&tSubs, true) != READ_OK) {
        Serial.println(F("Erreur de lecture de la sonde substrat"));
        return;
        }*/
#endif

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

    Serial.println(String("SDK:") + String(ESP.getSdkVersion()));
#if E_SCREEN
    initEInkScreen(display);
#endif
}

void loop()
{
    char tags[16];
    char fields[256];
    char formatTags[] = "read_all=%s";
    char formatFields[] = ""
#if DS18B20_MEASURES
        "substrateTemperature=%0.3f,"
#endif
#if SOIL_MEASURES
        "substrateHumidity=%0.3f,"
#endif
#if BME280_MEASURES
        "insideTemperature=%0.3f,"
        "insideHumidity=%0.3f,"
        "insidePressure=%0.3f,"
        "outsideTemperature=%0.3f,"
        "outsideHumidity=%0.3f,"
        "outsidePressure=%0.3f"
#endif
        END_OF_MEASURE_LINE;

    char readingStatus[] = "true";

#if DS18B20_MEASURES
    float tSubs = 0.0;
#endif
#if SOIL_MEASURES
    int hSubs = 0;
#endif
#if BME280_MEASURES
    float tIn = 0.0;
    float hIn = 0.0;
    float pIn = 0.0;
    float tOut = 0.0;
    float hOut = 0.0;
    float pOut = 0.0;
#endif

#if WEB_SERVER
    Portal.handleClient();
#endif


#if DS18B20_MEASURES
    // get values from DS18S20
    if (readOneWireTemp (&tSubs) != READ_OK)
        tSubs = DS_ERROR_VALUE;
#endif


#if SOIL_MEASURES
    /* Substrate hygrometry */
    hSubs = analogRead(soilRead); //read pin 26 then assign hSubs to that value
#endif


#if BME280_MEASURES
    if (bmeIn.begin(0x76))
    {
        tIn = bmeIn.readTemperature();
        hIn = bmeIn.readHumidity();
        pIn = bmeIn.readPressure();
    } else
    {
        tIn = -1000.0;
        hIn = -1000.0;
        pIn = -1000.0;
        Serial.println("Failed to perform reading inside BME280 sensor :(");
    }

    if (bmeOut.begin(0x77))
    {
        tOut = bmeOut.readTemperature();
        hOut = bmeOut.readHumidity();
        pOut = bmeOut.readPressure();
    } else
    {
        tOut = -1000.0;
        hOut = -1000.0;
        pOut = -1000.0;
        Serial.println("Failed to perform reading outside BME280 sensor :(");
    }
#endif

    sprintf(tags, formatTags, readingStatus);
    sprintf(fields, formatFields
#if DS18B20_MEASURES
            ,tSubs
#endif
#if SOIL_MEASURES
            ,hSubs 
#endif
#if BME280_MEASURES
            ,tIn, hIn, pIn, tOut, hOut, pOut
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


#if E_SCREEN
    showPartialUpdate (display
#if DS18B20_MEASURES
            ,tSubs
#endif
#if SOIL_MEASURES
            ,hSubs
#endif
#if BME280_MEASURES
            ,tIn
            ,hIn
            ,tOut
            ,hOut
#endif
            );
#else
    Serial.print (fields);
#endif

    delay(5000); //Delay 5 sec.
}

