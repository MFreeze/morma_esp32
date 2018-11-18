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

#ifndef __SENSOR_DEBUG_H__
#define __SENSOR_DEBUG_H__

#if SENSOR_DEBUG
#define SENSOR_LOGE(name, str_format, ...) esp_log_write (ESP_LOG_ERROR, name, name " - " str_format, ##__VA_ARGS__);
#define SENSOR_LOGW(name, str_format, ...) esp_log_write (ESP_LOG_WARN,  name, name " - " str_format, ##__VA_ARGS__);
#define SENSOR_LOGI(name, str_format, ...) esp_log_write (ESP_LOG_INFO,  name, name " - " str_format, ##__VA_ARGS__);
#else
#define SENSOR_LOGE(name, str_format, ...)
#define SENSOR_LOGW(name, str_format, ...)
#define SENSOR_LOGI(name, str_format, ...)
#endif      /* SENSOR_DEBUG */

#endif      /* __SENSOR_DEBUG_H__ */
