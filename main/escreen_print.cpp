/*
 * =====================================================================================
 *
 *       Filename:  escreen_print.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/11/2018 23:53:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */

#include "escreen_print.h"
/* TODO Check whether this is required... */
#include <GxGDEP015OC1/GxGDEP015OC1.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

#define PRINT_STRING_ON_SCREEN(display, font, string, offset, x, y, w, h, fgcol, bgcol) do {\
    display.setRotation(45); \
    display.setFont(font); \
    display.setTextColor(fgcol); \
    display.fillRect(x, y, w, h, bgcol); \
    display.setCursor(x, y + offset); \
    display.print(string); \
    display.updateWindow(x, y, w, h, true); \
} while (0);

void initEInkScreen (GxEPD_Class display)
{
    display.init();

    display.drawExampleBitmap(gImage_VBsplash, 0, 0, 200, 200, GxEPD_BLACK);
    display.update();
    delay(2000);

    display.drawExampleBitmap(gImage_measureBG, 0, 0, 200, 200, GxEPD_BLACK);
    display.update();
    delay(1000);

    display.drawExampleBitmap(gImage_measureBG, sizeof(gImage_measureBG), GxEPD::bm_default | GxEPD::bm_partial_update);
    display.update();
    delay(3000);
}

void showPartialUpdate (GxEPD_Class display
#if DS18B20_MEASURES
        ,float tSubs
#endif
#if SOIL_MEASURES
        ,float hSubs
#endif
#if BME280_MEASURES
        ,float tIn
        ,float hIn
        ,float tOut
        ,float hOut
#endif
        )
{
//    const char* name = "FreeSans12pt7b";
    const GFXfont* f = &FreeSans12pt7b;

    /*
     * .________________________________.
     * |                |               |
     * |    (x1,y1)     |    (x2,y1)    |
     * |                |               |
     * |----------------+---------------|
     * |                |               |
     * |    (x1,y2)     |    (x2,y2)    |
     * |                |               |
     * |----------------+---------------|
     * |                |               |
     * |    (x1,y3)     |    (x2,y3)    |
     * |                |               |
     * |________________|_______________|
     *
     */

    uint16_t x1 = 10;
    uint16_t x2 = 110;
    uint16_t y1 = 29;
    uint16_t y2 = 86;
    uint16_t y3 = 141;
    uint16_t width = 50;
    uint16_t height = 25;



#if DS18B20_MEASURES
    // print substrate temperature
    String tSubsString = String(tSubs,1);
    PRINT_STRING_ON_SCREEN(display, f, tSubsString, 18, x1, y1, width, height, GxEPD_BLACK, GxEPD_WHITE);
#endif

#if SOIL_MEASURES
    // print substrate hygrometry
    String hSubsString = String(hSubs,1);
    PRINT_STRING_ON_SCREEN(display, f, hSubsString, 18, x2, y1, width, height, GxEPD_BLACK, GxEPD_WHITE);
#endif

#if BME280_MEASURES
    // print internal temperature
    String tInString = String(tIn,1);
    PRINT_STRING_ON_SCREEN(display, f, tInString, 18, x1, y2, width, height, GxEPD_BLACK, GxEPD_WHITE);

    // print internal hygrometry
    String hInString = String(hIn,1);
    PRINT_STRING_ON_SCREEN(display, f, hInString, 18, x2, y2, width, height, GxEPD_BLACK, GxEPD_WHITE);

    // print external temperature
    String tOutString = String(tOut,1);
    PRINT_STRING_ON_SCREEN(display, f, tOutString, 18, x1, y3, width, height, GxEPD_BLACK, GxEPD_WHITE);

    // print external hygrometry
    String hOutString = String(hOut,1);
    PRINT_STRING_ON_SCREEN(display, f, hOutString, 18, x2, y3, width, height, GxEPD_BLACK, GxEPD_WHITE);
#endif
}
