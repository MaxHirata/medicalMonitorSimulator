#include "Globals.h"
#include "Data.h"
#include "Interrupts.h"
#include "Interrupts.h"

// The number of items in each menu.
const int MAIN_SIZE = 2;
const int MEASURE_SIZE = 4;


// *****************************************************************************
// Helper function to change to state of the current display mode to a new mode.
// *****************************************************************************
void changeMode(Mode newMode, KeypadData* kd)
{
    // Switch to new display mode with cursor on first row (if appropriate).
    *kd->mode = newMode;              // Change to new display.
    *kd->scroll = 0;                  // Reset scroll position.
    *kd->measurement = NONE;          // Reset selected measurement.
    
    // If we're switching to EKGCapture or measure, then wake up that task.
    if (newMode == EKG)
    {
        vTaskResume(ekgCaptHandle);
        vTaskPrioritySet(ekgCaptHandle, P_TICK);        
    }
    else if (newMode == MEASURE)
    {
        vTaskResume(measureHandle);
        vTaskPrioritySet(measureHandle, P_TICK);        
    }
    
    // If we're switching to Anmumciate, then take all measurements.
    if (newMode == ANNUNCIATE)
    {
        *kd->measurement = ALL;
    }
}

// *****************************************************************************
// Task for processing keyboard data.
//
// Keypresses are handled in the following manner during each "tick" event:
//
//   1. The tick event handler polls the keypad GPIO to determine the latest
//      keypress bit pattern, then saves that bit pattern to the keypad 
//      flag global variable.
// 
//   2. TasKeypad updates our current model using a finite state machine in
//      response to any keypress.  The current state of our moel is represented 
//      by (a) the current display mode, (b) the current selexted row (if  any),
//      and (c) the current  measurement reading (if any).  The FSM determines
//      our current state and then transitions the model to the next appropriate
//      state when it recognizes a valid keypress.  
//
//   3. TaskDisplay reads those global variables to determine its current
//      state.  It then writes the appropriate output to the OLED.
// 
// This allows us to maintain a separation of concerns.  The ISR is responsible
// for reading (but not processing) the current keypress; the Keyboard task
// is responsible for updating our current model (but not not displaying
// any information), and the Display task is responsible for writing all 
// output to the OLED.
//  
// NB:  "left" and "right" keypresses to adjust BP are handled in TaskMeasure.
// *****************************************************************************
void taskKeypad(void* data)
{
    // Cast the raw data to a KeypadData struct.
    KeypadData* kd = (KeypadData*)data;

    // Loop forever for RTOS.
    while(TRUE)
    {
        // What is our current display mode?
        switch (*kd->mode)
        {
            case MAIN:
              
                // Main menu. Move cursor or select a menu item.
                switch (*kd->keypress)
                {
                    case UP:    
                        if (*kd->scroll > 0) (*kd->scroll)--; break;
                        
                    case DOWN:  
                        if (*kd->scroll < 2) (*kd->scroll)++; break;

                    case SELECT:
                      
                        // Change display mode to selected menu item.
                        switch (*kd->scroll)
                        {
                            case 0:  changeMode(MEASURE,    kd); break;
                            case 1:  changeMode(ANNUNCIATE, kd); break;
                            case 2:  changeMode(EKG,        kd); break;
                        }                      
                }
                break;

            case MEASURE:
            
                // Measure menu. Move cursor, select a menu item, or update BP.
                switch (*kd->keypress)
                {
                    case UP:    if (*kd->scroll > 0) (*kd->scroll)--; break;
                    case DOWN:  if (*kd->scroll < 3) (*kd->scroll)++; break;
                    case SELECT:
                      
                        // Change selected measurement to selected menu item.
                        switch (*kd->scroll)
                        {
                            case 0: *kd->measurement = PRESSURE; break;
                            case 1: *kd->measurement = TEMP;     break;
                            case 2: *kd->measurement = PULSE;    break;
                            case 3: changeMode(MAIN, kd);        break;
                        }
                        break;                        
                }
                break;
                  
                  
            case ANNUNCIATE:
            
                // Annunciate mode. Transition to Main Menu on any keypress.
                if (*kd->keypress != NULL)
                {
                    changeMode(MAIN, kd);
                }
                break;

            case EKG:
            
                // TODO:  Implrement appropriate logic.
                if (*kd->keypress != NULL)
                {
                    changeMode(MAIN, kd);
                }
                break;
                  
        }
        
        // Reset keypress flag unless "left" or "right" pressed (for BP).
        if (*kd->keypress == UP || *kd->keypress == DOWN || *kd->keypress == SELECT)
        {
            *kd->keypress = (Keypress)NULL;
        }
        
        // Sleep until the next tick period.
        vTaskDelay(TICK_DELAY);     
    }
}