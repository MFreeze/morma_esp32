/*
 * =====================================================================================
 *
 *       Filename:  ds18b20.h
 *
 *    Description:  Header file for DS18B20 sensor
 *
 *        Version:  1.0
 *        Created:  06/11/2018 09:50:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef __DS18B20_H__
#define __DS18B20_H__
#include <OneWire.h>

// default values
#define DS_ERROR_VALUE -100.0


/* DS18B20 - Code de retour de la fonction getTemperature() */
enum DS18B20_RCODES {
    READ_OK,  // Lecture ok
    NO_SENSOR_FOUND,  // Pas de capteur
    INVALID_ADDRESS,  // Adresse reçue invalide
    INVALID_SENSOR  // Capteur invalide (pas un DS18B20)
};

DS18B20_RCODES readOneWireTemp (float *temperatures, byte reset_search=1);

#endif     /* -----  not __DS18B20_H__  ----- */
