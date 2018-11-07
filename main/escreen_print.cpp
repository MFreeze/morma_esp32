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
#include <GxGDEP015OC1/GxGDEP015OC1.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

void initEInkScreen (GxEPD_Class display)
{
    display.init();

    printf("No problem with the initialization.\n");

    display.drawExampleBitmap(gImage_VBsplash, 0, 0, 200, 200, GxEPD_BLACK);
    printf("%s - %d : drawExampleBitmap Ok\n", __FILE__, __LINE__);
    display.update();
    printf("%s - %d : update Ok\n", __FILE__, __LINE__);
    delay(2000);

    display.drawExampleBitmap(gImage_measureBG, 0, 0, 200, 200, GxEPD_BLACK);
    display.update();
    delay(1000);

    display.drawExampleBitmap(gImage_measureBG, sizeof(gImage_measureBG), GxEPD::bm_default | GxEPD::bm_partial_update);
    display.update();
    delay(3000);
}

void showPartialUpdate (GxEPD_Class display, float tSubs, float hSubs, float tIn, float hIn, float tOut, float hOut)
{
    const char* name = "FreeSans12pt7b";
    const GFXfont* f = &FreeSans12pt7b;

    // print substrate temperature
    String tSubsString = String(tSubs,1);

    uint16_t vOffset = 18; //required so that text is written INSIDE the box

    uint16_t box_x = 10;
    uint16_t box_y = 29;
    uint16_t box_w = 50;
    uint16_t box_h = 25;
    uint16_t cursor_y = box_y + vOffset;

    display.setRotation(45);
    display.setFont(f);
    display.setTextColor(GxEPD_BLACK);

    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x, cursor_y);
    display.print(tSubsString); 
    display.updateWindow(box_x, box_y, box_w, box_h, true);

    // print substrate hygrometry
    String hSubsString = String(hSubs,1);
    //String humAndUnitsString = String("IN hygro " + humString + "%");

    uint16_t box_x2 = 110;
    uint16_t box_y2 = box_y;
    uint16_t cursor_y2 = box_y2 + vOffset;

    display.setRotation(45);
    display.setFont(f);
    display.setTextColor(GxEPD_BLACK);

    display.fillRect(box_x2, box_y2, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x2, cursor_y2);
    display.print(hSubsString); 
    display.updateWindow(box_x2, box_y2, box_w, box_h, true);

    // print internal temperature
    String tInString = String(tIn,1);

    uint16_t box_x3 = box_x;
    uint16_t box_y3 = 86;
    uint16_t cursor_y3 = box_y3 + vOffset;

    display.setRotation(45);
    display.setFont(f);
    display.setTextColor(GxEPD_BLACK);

    display.fillRect(box_x3, box_y3, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x3, cursor_y3);
    display.print(tInString); 
    display.updateWindow(box_x3, box_y3, box_w, box_h, true);

    // print internal hygrometry
    String hInString = String(hIn,1);

    uint16_t box_x4 = box_x2;
    uint16_t box_y4 = box_y3;
    uint16_t cursor_y4 = box_y4 + vOffset;

    display.setRotation(45);
    display.setFont(f);
    display.setTextColor(GxEPD_BLACK);

    display.fillRect(box_x4, box_y4, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x4, cursor_y4);
    display.print(hInString); 
    display.updateWindow(box_x4, box_y4, box_w, box_h, true);

    // print external temperature
    String tOutString = String(tOut,1);

    uint16_t box_x5 = box_x;
    uint16_t box_y5 = 141;
    uint16_t cursor_y5 = box_y5 + vOffset;

    display.setRotation(45);
    display.setFont(f);
    display.setTextColor(GxEPD_BLACK);

    display.fillRect(box_x5, box_y5, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x5, cursor_y5);
    display.print(tOutString); 
    display.updateWindow(box_x5, box_y5, box_w, box_h, true);

    // print external hygrometry
    String hOutString = String(hOut,1);

    uint16_t box_x6 = box_x2;
    uint16_t box_y6 = box_y5;
    uint16_t cursor_y6 = box_y6 + vOffset;

    display.setRotation(45);
    display.setFont(f);
    display.setTextColor(GxEPD_BLACK);

    display.fillRect(box_x6, box_y6, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x6, cursor_y6);
    display.print(hOutString); 
    display.updateWindow(box_x6, box_y6, box_w, box_h, true);
}
