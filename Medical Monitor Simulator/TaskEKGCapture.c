#include "Globals.h"
#include "Data.h"
#include "Interrupts.h"

#include <time.h>
#include <stdlib.h>
#include <math.h>

#define AMPLITUDE_PEAK    3.3 
#define FREQUENCY         35
#define SAMPLE_RATE       (FREQUENCY / 3.14159) * 2.5 
#define xDelay250ms       pdMS_TO_TICKS(250)

// *****************************************************************************
// *****************************************************************************
void taskEKGCapture(void* data)
{
    // Cast the struct to EKGData.
    EkgData* ed = (EkgData*)data;
  
    // Loop forever for RTOS scheduler.
    while(TRUE)
    {
        // Capture each of the samples.
        for (int i = 0; i < 256; ++i)
        {
            float time = i * SAMPLE_RATE / 256.0;
            float ekg  = (AMPLITUDE_PEAK) / 2 * sin(time);
            ed->ekgRawBuf[i] = (short)ekg;
        }
        
        // Wake up TaskEKGProcess
        vTaskResume(ekgProcHandle);
        vTaskPrioritySet(ekgProcHandle, P_TICK);
        
        // Sleep until woken up again by TaskKeypad.
        vTaskSuspend(ekgCaptHandle);
    }
}
