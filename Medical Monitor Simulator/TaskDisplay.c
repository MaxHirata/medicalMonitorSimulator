#include "Globals.h"
#include "Data.h"
#include "Interrupts.h"
#include <string.h>
#include <stdio.h>


// Define the maximum number of rows for the OLED to display.
const int ROW_COUNT = 8;

// Define the number of columns from left for menu or annunciate items.
const int COL_START = 2;

// String arrays for the text/format codes to display for each mode.

char* menuMain[] = {
    "MAIN MENU            ", 
    "                     ",
    "Measurement Mode     ", 
    "Annunciation Mode    ",
    "EKG Mode             ",
    "                     ",
    "                     ",
    "                     "
};

char* menuMeasure[] = { 
    "MEASURE MENU         ", 
    "                     ",
    "Blood Pressure       ",
    "Temperature          ",
    "Pulse Rate           ",
    "(Main Menu)          ",
    "                     ",
    "                     "
};

char* textAnnunciate[] = { 
    "Temp:   %i C         ",
    "Sys BP: %i mm Hg     ",
    "Dia BP: %i mm Hg     ",
    "Pulse:  %i BPM       ",
    "Batt:   %i           ",
    "EKG:    %i           ",
    "                     ",
    "                     "
};

char* textEKG[] = { 
    "EKG READING          ",
    "                     ",
    "1. Reading           ",
    "2. Computing         ",
    "3. Results:          ",
    "                     ",
    "                     ",
    "                     "
};

// *****************************************************************************
// Helper function to print each menu item, with a cursor for the selected row.
// *****************************************************************************
void printMenu(char* menu[], int selected)
{
    // Remember our current state.
    static char** lastMenu     = NULL;
    static int    lastSelected = -1;

    // Has the display changed since the last tick?  (Only redraw if needed.)
    if (menu != lastMenu || selected != lastSelected)
    {
        // Yes; redraw each row in the menu.
        for (int row = 0; row < ROW_COUNT; row++)
        {
            // Print the text for the current row.
            printfOLED(menu[row], COL_START, row, NULL);

            // Either draw or erase the cursor on this row.
            char* cursor = (row == selected + 2) ? ">>" : "  ";
            printfOLED(cursor, 0, row, NULL);
        } 
    }
    
    // Save our current state.
    lastMenu = menu;
    lastSelected = selected;
}

// *****************************************************************************
// Helper function for displaying the currently-selected measurement on the
// last two rows of the Measure Menu.
// *****************************************************************************
void printDataLine(DisplayData* dd)
{
    switch (*dd->measurement)
    {
        case TEMP:
            // Print line showing temperature.
            printfOLED(textAnnunciate[0], COL_START, 7, (void*)dd->tmCorBuf[CURRENT]);
            break;

        case PRESSURE:
            // Print line showing both BP readings with format "BP: 120 / 80"
            printfOLED("BP: %i",  COL_START, 7, (void*)dd->syCorBuf[CURRENT]);
            printfOLED("/ %i   ", 6, 7, (void*)dd->diCorBuf[CURRENT]);
            break;
            
        case PULSE:
            // Print line showing temperature.
            printfOLED(textAnnunciate[3], COL_START, 7, (void*)dd->prCorBuf[CURRENT]);
            break;
            
        default:
            // Print blank line to erase last line.
            printfOLED("                     ", 0, 7, NULL);
            break;
    }
}

// *****************************************************************************
// Task for printing the current display information to the OLED.
//
// This task reads the current display mode and state (as set by TaskKeyboard), 
// then prints the appropriate text and data for this mode to the OLED.
//
// Each line of each display mode is stored in an array of fixed-width strings.
// Blank lines are represented by spaces of the same width.  This way, the task 
// will automatically overwrite or erase any prior text on the screen.
// 
// Additionally, this task displays a cursor to the left of the currently-
// selected menu item when displaying a menu.
// *****************************************************************************
void taskDisplay(void* data)
{
    // Cast the raw data to a DisplayData struct.
    DisplayData* dd = (DisplayData*)data;

    // Loop forever for RTOS scheduler.
    while(TRUE)
    {
        // What is our current display mode?
        switch (*dd->mode)
        {
              
            case MAIN:
                // Print main menu with cursor on selected row.
                printMenu(menuMain, *dd->scroll);
                break;
                
            case MEASURE:
                // Print measure menu with cursor on selected row.
                printMenu(menuMeasure, *dd->scroll);
                
                // Print the selected measurement on last row of display.
                printDataLine(dd);
                break;
              
            case ANNUNCIATE:
                // Print each row of annunciate screen with formatted data).
                printfOLED(textAnnunciate[0], COL_START, 0, (void*)dd->tmCorBuf[CURRENT]);     // Temperature
                printfOLED(textAnnunciate[1], COL_START, 1, (void*)dd->syCorBuf[CURRENT]);     // Systolic
                printfOLED(textAnnunciate[2], COL_START, 2, (void*)dd->diCorBuf[CURRENT]);     // Diastolic
                printfOLED(textAnnunciate[3], COL_START, 3, (void*)dd->prCorBuf[CURRENT]);     // Pulse
                printfOLED(textAnnunciate[4], COL_START, 4, (void*)dd->batteryState[CURRENT]); // Battery
                printfOLED(textAnnunciate[5], COL_START, 5, NULL);                             // Blank
                printfOLED(textAnnunciate[6], COL_START, 6, NULL);                             // Blank
                break;

            case EKG:
                // TODO:  Implement EKG display
                printfOLED(textEKG[0], COL_START, 0, NULL);
                printfOLED(textEKG[1], COL_START, 1, NULL);
                printfOLED(textEKG[2], COL_START, 2, NULL);
                printfOLED(textEKG[3], COL_START, 3, NULL);
                printfOLED(textEKG[4], COL_START, 4, NULL);
                printfOLED(textEKG[5], COL_START, 5, NULL);
                printfOLED(textEKG[6], COL_START, 6, NULL);
                
            default:
                // Should not happen.
                break;
        }

        // Sleep until the next tick period.
        vTaskDelay(TICK_DELAY);
    }
}
