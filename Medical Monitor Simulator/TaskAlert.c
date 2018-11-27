#include "Globals.h"
#include "Data.h"
#include "Interrupts.h"

// Define value for "display solid LED."
const int SOLID = 0;

// Define number of ticks per period.
const int TICKS_PER_SEC = 20;

// Array of two floats for the current periods of LED0 and LED1, respectively.
float ledPeriod[2]  = { 0, 0 };

char* WARNING = "! ";
char* ALARM   = "!!";

// *****************************************************************************
// Toggles the current state of the specified LED (on or off) based on its 
// current period.  A period of SOLID (i.e., 0) represents an "always on" LED.
// *****************************************************************************
void updateLED(int led)
{
    // Start by assuming that the LED should be on solid (normal conditions).
    bool ledOn = TRUE;
    
    // If the LED should flash, keep on during the first half-period, then off.
    if (ledPeriod[led] > 0.0)
    {
        // Determine the number of ticks in the current period.
        int ticksPerPeriod = (int)(ledPeriod[led] * TICKS_PER_SEC);

        // Determine the index of this tick within the current period.
        int tickIndex = ticks % ticksPerPeriod;
        
        // Turn the LED on only if within the first half of this period.
        ledOn = (bool)(tickIndex < ticksPerPeriod / 2);                     
    }
  
    // Turn the relevant LED either on or off based on what we determined above.
    if (ledOn)
    {
        // Turn on the LED.
        GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);        
    }
    else
    {
        // Turn off the LED.
        GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 1);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3,
        GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3) ^ GPIO_PIN_3);
    }
}


// *****************************************************************************
// Either displays an alert icon (if "alert" is true) or erases any prior alerts.
// *****************************************************************************
void alertIf(bool alert, int led, float period, int row, char* alertText)
{
    // Should we display an alert?
    if (alert)
    {
        // Yes; display an alert on the OLED.
        printfOLED(alertText, 0, row, NULL);

        // Update the period of the appropriate LED.
        ledPeriod[led] = period;
    }
}

// *****************************************************************************
// Task for displaying warnings and alarms 
// *****************************************************************************
void taskAlert(void* data)
{

    // Cast the raw data to a struct.
    WarningAlarmData* wad = (WarningAlarmData*)data;
  
    // Loop forever for FreeRTOS scheduler.
    while(TRUE)
    {
        // Only display alerts in annunciate mode.
        if (mode == ANNUNCIATE)
        {
            // Set the current period of the two LEDs to "solid" by default.
            ledPeriod[0] = SOLID;
            ledPeriod[1] = SOLID;
            
            // Get the current readings for pulse, sys BP, dias BP, and battery.
            uchar temp  = wad->tmCorBuf[CURRENT];
            uchar diaBP = wad->diCorBuf[CURRENT];
            uchar sysBP = wad->syCorBuf[CURRENT]; 
            uchar pulse = wad->prCorBuf[CURRENT];
            uchar batt  = *wad->batteryState; 
            
            // Are any of these readings outside of their expected ranges?
            bool warnTemp   = (bool)(temp  < 60  || temp  > 100);
            bool alarmDiaBP = (bool)(diaBP < 56  || diaBP >  96);
            bool warnSysBP  = (bool)(sysBP < 120 || sysBP > 130);
            bool warnDiaBP  = (bool)(diaBP < 70  || diaBP >  80); 
            bool warnPulse  = (bool)(pulse < 60  || pulse > 100);
            bool warnBatt   = (bool)(batt < 40);
            
            // Display or erase each alert. If alerting, also update LED period.
            alertIf(warnTemp,   0, 1, 0, "! ");
            alertIf(alarmDiaBP, 1, 1, 1, "!!");
            if (!alarmDiaBP)
            { 
                // Only display BP warning if not already in BP alarm state.
                alertIf((bool)(warnSysBP || warnDiaBP),  0, 1, 2, "! ");
            }
            alertIf(warnPulse,  0, 1, 3, "! ");
            alertIf(warnBatt,   0, 1, 4, "! ");
        }
        
        // Update the current state of each LED (on or off).
        updateLED(0);
        updateLED(1);
            
    }
}