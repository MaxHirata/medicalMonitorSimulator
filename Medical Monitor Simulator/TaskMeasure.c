#include "Globals.h"
#include "Data.h"
#include "Interrupts.h"

#define MEASUREMENTS_PER_MINUTE 12

// Define an enumeration to track whether we're measuring Sys BP or Dia BP.
static enum BpMeasurement { SYS, DIA } bpMeasurement = SYS;

const int HUMAN_TEMPERATURE_OFFSET = 70;

// *****************************************************************************
// Simulates a measurement of the raw temperature data.
// *****************************************************************************
uchar measureTemp()
{
    unsigned long ulADC0_Value[1];

    ADCIntClear(ADC0_BASE, 3);
    ADCProcessorTrigger(ADC0_BASE, 3);
    while (!ADCIntStatus(ADC0_BASE, 3, false))
    {}

    ADCSequenceDataGet(ADC0_BASE, 3, ulADC0_Value);
    return (uint)((1475 * 1023 - 2250 * ulADC0_Value[0]) / 10230 + HUMAN_TEMPERATURE_OFFSET);
}

// *****************************************************************************
// Simulates a measurement of the pulse using the pulse rate transducer.
// *****************************************************************************
uchar measurePulse(uchar pulseLast)
{
    // Maintain the state of the last heartbeat count.
    static uint  heartbeatsLast = 0;
    static float pulseSmoothed = 0.0;

    // Get the total number of heartbeats for the current "tock" period.
    uchar pulseCurrent = (heartbeats - heartbeatsLast) * MEASUREMENTS_PER_MINUTE;
    heartbeatsLast = heartbeats;

    // Apply a four-period (20 sec) moving average filter to the measured pulse.
    pulseSmoothed = 0.25 * pulseCurrent + 0.75 * pulseSmoothed;

    // Is the smoothed value more than 15% different than last measured value?
    if (pulseSmoothed / pulseLast > 1.15 || pulseSmoothed / pulseLast < 0.85)
    {
        // Yes. Return smoothed pulse count.
        return (uchar)pulseCurrent;
    }
    else
    {
        // No. Return last value.
        return pulseLast;
    }
    
}


Data measureBP(Data* bpBuffer, Keypress keypress)
{        
    // Create a variable to determine which multiplier to use.
    float mult = 1.0;
  
    // Update the sys/dia readings if "left" or "right" key is pressed.
    if (keypress == LEFT || keypress == RIGHT)
    {
        // Increase or decrease the multiplier to simulate adjusting BP cuff.
        mult = (keypress == LEFT) ? (1/1.1) : 1.1;        
    }

    // Return the updated measurement.
    return (Data)(bpBuffer[LAST] * mult);
}

// *****************************************************************************
// Task to simulate a raw measurement of the selected measurement data point.
// *****************************************************************************
void taskMeasure(void* data)
{
    // Cast the raw data to a MeasurementData struct.
    MeasurementData* md = (MeasurementData*)data;
    
    // Loop forever for RTOS scheduler.
    while(TRUE)
    {
        // Should we measure pulse rate?
        if (*md->measurement == PULSE || *md->measurement == ALL)
        {
            shiftRight(md->prRawBuf);
            md->prRawBuf[CURRENT] = measurePulse(md->prRawBuf[LAST]);
        }
      
        // Should we measure temperature?
        if (*md->measurement == TEMP || *md->measurement == ALL)
        {
            shiftRight(md->tmRawBuf);
            md->tmRawBuf[CURRENT] = measureTemp();
        }

        // Should we measure blood pressure?
        if (*md->measurement == PRESSURE || *md->measurement == ALL)
        {
            switch (bpMeasurement)
            {
                case SYS:
                    shiftRight(md->syRawBuf);
                    md->syRawBuf[CURRENT] = measureBP(md->syRawBuf, *md->keypress);
                    break;
                  
                case DIA:
                    shiftRight(md->diRawBuf);
                    md->diRawBuf[CURRENT] = measureBP(md->diRawBuf, *md->keypress);
                    break;
            }
                      
        }

        // Reset any existing keypress to indicate that we've handled it.
        *md->keypress = (Keypress)NULL;

        // Determine whether we have new raw data for temp, BP or pulse.
        bool newTemp = (bool)(md->tmRawBuf[CURRENT] != md->tmRawBuf[LAST]);
        bool newPR   = (bool)(md->prRawBuf[CURRENT] != md->prRawBuf[LAST]);
        bool newSys  = (bool)(md->syRawBuf[CURRENT] != md->syRawBuf[LAST]);
        bool newDia  = (bool)(md->diRawBuf[CURRENT] != md->diRawBuf[LAST]);
        
        // If we have new raw data, wake up the compute task with high priority.
        if (newTemp || newSys || newDia || newPR)
        {
            vTaskResume(computeHandle);
            vTaskPrioritySet(computeHandle, P_TICK);
        }
                
        // Sleep until the next tock period.
        vTaskDelay(TOCK_DELAY);   
    }
}
