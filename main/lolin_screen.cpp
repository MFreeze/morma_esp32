/*
 * =====================================================================================
 *
 *       Filename:  lolin_screen.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21/10/2019 14:30:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include "lolin_screen.h"
#include "debug.h"

/*-----------------------------------------------------------------------------
 *  Pin Declaration
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Pin Declaration -------- */
#ifndef  EPD_CS
#define  EPD_CS      14
#endif   /* ----- #ifndef EPD_CS  ----- */

#ifndef  EPD_CLK
#define  EPD_CLK     18
#endif   /* ----- #ifndef EPD_CLK  ----- */

#ifndef  EPD_BUSY
#define  EPD_BUSY   -1
#endif   /* ----- #ifndef EPD_BUSY  ----- */

#ifndef  EPD_MOSI
#define  EPD_MOSI    23
#endif   /* ----- #ifndef EPD_MOSI  ----- */

#ifndef  EPD_DC
#define  EPD_DC      27
#endif   /* ----- #ifndef EPD_DC  ----- */

#ifndef  EPD_RST
#define  EPD_RST     33
#endif   /* ----- #ifndef EPD_RST  ----- */
/* }}} */


LOLIN_IL3897 EPD(250, 122, EPD_MOSI, EPD_CLK, EPD_DC, EPD_RST, EPD_CS, EPD_BUSY);
int cur_nb_line = 0;
const int max_nb_line = 14;


#ifndef  STATUS_OFFSET
#define  STATUS_OFFSET      200
#endif   /* ----- #ifndef STATUS_OFFSET  ----- */


#ifndef  STATUS_SUCCESS
#define  STATUS_SUCCESS     "Ok"
#endif   /* ----- #ifndef STATUS_SUCCESS  ----- */


#ifndef  STATUS_ERROR
#define  STATUS_ERROR       "Fail"
#endif   /* ----- #ifndef STATUS_ERROR  ----- */

/*-----------------------------------------------------------------------------
 *  Internal Functions
 *-----------------------------------------------------------------------------*/
/* {{{ -------- Internal Functions -------- */
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  printMessage
 *  Description:  Print a message on the screen 
 *   Parameters:  const char *messgae: the message to print
 *                int eol: a boolean that indicates whether the line should end or not
 *       Return:  
 * =====================================================================================
 */
/* --------- printMessage --------- {{{ */
    void
printMessage (const char *message, int eol)
{
    if (cur_nb_line == max_nb_line)
    {
        EPD.clearBuffer ();
        EPD.setCursor (0, 0);
        cur_nb_line = 0;
    }


    if (eol)
    {
        EPD.println (message);
    }
    else
    {
        EPD.print (message);
    }

    EPD.update ();
    EPD.display ();

    cur_nb_line += eol ? 1 : 0;
}		/* -----  end of function printMessage  ----- */
/* }}} */
/* }}} */


/*-----------------------------------------------------------------------------
 *  External Functions
 *-----------------------------------------------------------------------------*/
/* {{{ -------- External Functions -------- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  initScreen
 *  Description: Initializes a screen 
 *   Parameters:  
 *       Return:  
 * =====================================================================================
 */
/* --------- initScreen --------- {{{ */
    void
initScreen ()
{
    EPD.begin ();
    EPD.clearBuffer ();

    EPD.setCursor (0, 0);
    EPD.setTextColor (EPD_BLACK);
    EPD.setTextSize (1);
    EPD.setTextWrap (false);

    EPD.display ();
}		/* -----  end of function initScreen  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  initTracing
 *  Description:  Print the label of the current process 
 *   Parameters:  const char *label: the label of the task being performed
 *       Return:  
 * =====================================================================================
 */
/* --------- initTracing --------- {{{ */
    void
initTracing (const char *label)
{
    printMessage (label, 0);
}		/* -----  end of function initTracing  ----- */
/* }}} */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  statusTracing
 *  Description:  Print the status of the terminated process 
 *   Parameters:  int status: 0 if fail anything else for success
 *       Return:  
 * =====================================================================================
 */
/* --------- statusTracing --------- {{{ */
    void
statusTracing (int status)
{
    int cur_y = EPD.getCursorY ();

    EPD.setCursor (STATUS_OFFSET, cur_y);

    printMessage (status ? STATUS_SUCCESS : STATUS_ERROR, 1);
}		/* -----  end of function statusTracing  ----- */
/* }}} */
/* }}} */
