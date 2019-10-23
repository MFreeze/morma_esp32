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
#define SENSOR_LOGE(name, str_format, ...) ESP_LOGE (name, name " - " str_format, ##__VA_ARGS__);
#define SENSOR_LOGW(name, str_format, ...) ESP_LOGW (name, name " - " str_format, ##__VA_ARGS__);
#define SENSOR_LOGI(name, str_format, ...) ESP_LOGI (name, name " - " str_format, ##__VA_ARGS__);
#else
#define SENSOR_LOGE(name, str_format, ...)
#define SENSOR_LOGW(name, str_format, ...)
#define SENSOR_LOGI(name, str_format, ...)
#endif      /* SENSOR_DEBUG */


#ifdef  DEBUG_SCREEN
#define SCREEN_LOGE(str_format, ...) ESP_LOGE ("EDP", "EDP - " str_format, ##__VA_ARGS__);
#define SCREEN_LOGW(str_format, ...) ESP_LOGW ("EDP", "EDP - " str_format, ##__VA_ARGS__);
#define SCREEN_LOGI(str_format, ...) ESP_LOGI ("EDP", "EDP - " str_format, ##__VA_ARGS__);
#else      /* -----  not DEBUG_SCREEN  ----- */
#define SCREEN_LOGE(str_format, ...)
#define SCREEN_LOGW(str_format, ...)
#define SCREEN_LOGI(str_format, ...)
#endif     /* -----  not DEBUG_SCREEN  ----- */

#endif      /* __DEBUG_H__ */