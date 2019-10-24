    /*
    * =====================================================================================
    *
    *       Filename:  debug.h
    *
    *    Description:  Debug header
    *
    *        Version:  1.0
    *        Created:  15/11/2018 22:05:28
    *       Revision:  none
    *       Compiler:  gcc
    *
    *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
    *   Organization:  
    *
    * =====================================================================================
    */

#include "config.h"

#ifndef __DEBUG_H__
#define __DEBUG_H__

#if SENSOR_DEBUG
#define SENSOR_LOGE(name, str_format, ...) ESP_LOGE (name, name " - %s, %d: " str_format, __FILE__, __LINE__, ##__VA_ARGS__);
#define SENSOR_LOGW(name, str_format, ...) ESP_LOGW (name, name " - %s, %d: " str_format, __FILE__, __LINE__, ##__VA_ARGS__);
#define SENSOR_LOGI(name, str_format, ...) ESP_LOGI (name, name " - %s, %d: " str_format, __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define SENSOR_LOGE(name, str_format, ...)
#define SENSOR_LOGW(name, str_format, ...)
#define SENSOR_LOGI(name, str_format, ...)
#endif      /* SENSOR_DEBUG */


#ifdef  DEBUG_SCREEN
#define SCREEN_LOGE(str_format, ...) ESP_LOGE ("EDP", "EDP - %s, %d: " str_format, __FILE__, __LINE__, ##__VA_ARGS__);
#define SCREEN_LOGW(str_format, ...) ESP_LOGW ("EDP", "EDP - %s, %d: " str_format, __FILE__, __LINE__, ##__VA_ARGS__);
#define SCREEN_LOGI(str_format, ...) ESP_LOGI ("EDP", "EDP - %s, %d: " str_format, __FILE__, __LINE__, ##__VA_ARGS__);
#else      /* -----  not DEBUG_SCREEN  ----- */
#define SCREEN_LOGE(str_format, ...)
#define SCREEN_LOGW(str_format, ...)
#define SCREEN_LOGI(str_format, ...)
#endif     /* -----  not DEBUG_SCREEN  ----- */


#ifdef  WIFI
#define WIFI_LOGE(str_format, ...) ESP_LOGE ("WIFI", "WIFI - %s, %d: " str_format, __FILE__, __LINE__, ##__VA_ARGS__);
#define WIFI_LOGW(str_format, ...) ESP_LOGW ("WIFI", "WIFI - %s, %d: " str_format, __FILE__, __LINE__, ##__VA_ARGS__);
#define WIFI_LOGI(str_format, ...) ESP_LOGI ("WIFI", "WIFI - %s, %d: " str_format, __FILE__, __LINE__, ##__VA_ARGS__);
#else      /* -----  not WIFI  ----- */
#define WIFI_LOGE(str_format, ...)
#define WIFI_LOGW(str_format, ...)
#define WIFI_LOGI(str_format, ...)
#endif     /* -----  WIFI  ----- */
#endif      /* __DEBUG_H__ */
