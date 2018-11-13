/*
 * =====================================================================================
 *
 *       Filename:  ds18b20.cpp
 *
 *    Description:  Body file for ds18b20 component
 *
 *        Version:  1.0
 *        Created:  13/11/2018 17:56:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <esp_log.h>

#include "ds18b20.h"

// Log macros
#define DS_LOGE(...) esp_log_write (ESP_LOG_ERROR, "DS18B20", ##__VA_ARGS__);
#define DS_LOGI(...) esp_log_write (ESP_LOG_INFO, "DS18B20", ##__VA_ARGS__);


const byte ONEWIRE_PIN = 25;

// Initialize the OneWire that will be the interface to the OneWire Sensors (DS18B20)
OneWire ds(ONEWIRE_PIN);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  readOneWireTemp
 *  Description:  Read temperature on a DS18B20 sensor
 *   Parameters:  float *temperatures: a float that will contain the value
 *                byte reset_search: indicates whether a the search for new 
 *       Return:  DS18B20 that indicates whether everything ran fine
 * =====================================================================================
 */
/* --------- readOneWireTemp --------- {{{ */
    DS18B20_RCODES
readOneWireTemp (float *temperatures, byte reset_search)
{
    byte data[9];       // Data read from the scratchpad
    byte addr[8];       // Address of the detected 1-Wire component

    /* Reset le bus 1-Wire ci nécessaire (requis pour la lecture du premier capteur) */
    if (reset_search)
        ds.reset_search ();

    /* Recherche le prochain capteur 1-Wire disponible */
    if (!ds.search (addr)) 
    {
        DS_LOGE ("No sensor found.\n");
        return NO_SENSOR_FOUND;
    }

    /* Vérifie que l'adresse a été correctement reçue */
    if (OneWire::crc8 (addr, 7) != addr[7])
    {
        DS_LOGE ("Unknown address for given sensor.\n");
        return INVALID_ADDRESS;
    }

    /* Vérifie qu'il s'agit bien d'un DS18B20 */
    if (addr[0] != 0x28)
    {
        DS_LOGE ("Unkown sensor.\n");
        return INVALID_SENSOR;
    }

    /* Reset le bus 1-Wire et sélectionne le capteur */
    ds.reset ();
    ds.select (addr);

    /* Lance une prise de mesure de température et attend la fin de la mesure */
    ds.write (0x44, 1);
    delay (800);

    /* Reset le bus 1-Wire, sélectionne le capteur et envoie une demande de lecture du scratchpad */
    ds.reset ();
    ds.select (addr);
    ds.write (0xBE);

    /* Lecture du scratchpad */
    for (byte i = 0; i < 9; i++)
        data[i] = ds.read ();

    /* Calcul de la température en degré Celsius */
    *temperatures = (int16_t) ((data[1] << 8) | data[0]) * 0.0625; 
    DS_LOGI ("Read temperature: %lf.\n", *temperatures);

    // Pas d'erreur
    return READ_OK;
}		/* -----  end of function readOneWireTemp  ----- */
/* }}} */
