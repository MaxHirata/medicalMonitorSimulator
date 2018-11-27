#include "Globals.h"
#include "Data.h"
#include <stdio.h>

// Strings for formatting measurement or alert HTML strings.

char* measurementHTML = 
     "Temp:   %i C         <br>"
     "Sys BP: %i mm Hg     <br>"
     "Dia BP: %i mm Hg     <br>"
     "Pulse:  %i BPM       <br>"
     "Batt:   %i           <br>"
     "EKG:    %i           <br>";
       
char* alertHTML = 
     "Blood pressure alarm:           %i <br>"
     "Temperature out of range:       %i <br>"
     "Blood pressuree out of range:   %i <br>"
     "Pulse out of range:             %i <br>"
     "Low battery:                    %i <br>";
       
       

// *****************************************************************************
// Command task for responding to commands from web
// *****************************************************************************
void taskCommand(void* data)
{
     // Cast the raw data to a MeasurementData struct.
    CommandData* cd = (CommandData*)data;
    
    // Track whether the display is currently on or off.
    static int displayOn = 0;
    
    while(TRUE)
    {
        switch (*cd->command)
        {
          
        case 'I':  // Initialize network communications between systems.
            break;
            
        case 'S':  // Start mode; initiate measuremen tasks.
            vTaskResume(measureHandle);
            vTaskPrioritySet(computeHandle, P_TICK);
            break;
          
        case 'P':  // Stop moded; suspend measurement tasks.
            vTaskSuspend(measureHandle);
            break;
            
        case 'D':  // Enable/disable local displaty
            displayOn = (displayOn + 1) % 2;    // Toggle between 0 and 1
            GPIOPinWrite( GPIO_PORTF_BASE, GPIO_PIN_0, displayOn );
            break;
          
        case 'M':  // Send measurement data in HTML format
          
            // Fill message buffer with measurement data in HTML format
            snprintf(*cd->message, 200, measurementHTML, 
                cd->tmCorBuf[CURRENT],
                cd->syCorBuf[CURRENT],
                cd->diCorBuf[CURRENT],
                cd->prCorBuf[CURRENT]);

            // To-do:  Transmit to client.

            break;
          
        case 'W':  // Send warning/arlarms in HTML format.
          
            // Get the current readings for pulse, sys BP, dias BP, and battery.
            uchar temp  = tmCorBuf[CURRENT];
            uchar diaBP = diCorBuf[CURRENT];
            uchar sysBP = syCorBuf[CURRENT]; 
            uchar pulse = prCorBuf[CURRENT];
            uchar batt  = batteryState; 

            // Determine which alerts to send.
            bool alarmDiaBP = (bool)(diaBP < 56  || diaBP >  96);
            bool warnTemp   = (bool)(temp  < 60  || temp  > 100);
            bool warnSysBP  = (bool)(sysBP < 120 || sysBP > 130);
            bool warnDiaBP  = (bool)(diaBP < 70  || diaBP >  80); 
            bool warnPulse  = (bool)(pulse < 60  || pulse > 100);
            bool warnBatt   = (bool)(batt < 40);
            
            // Fill message buffer with message.
            snprintf(*cd->message, 200, alertHTML,
                 alarmDiaBP,
                 warnTemp,
                 warnSysBP || warnDiaBP,
                 warnPulse,
                 warnBatt);
            
            // To-do:  Transmit to client.
                 
            break;
          
        default:
            // To do: Issue an "invalid command" message.
            break;
          
        }
      
      
      
        // Sleep until the next tick period.
        vTaskDelay(TICK_DELAY);
    }
}
