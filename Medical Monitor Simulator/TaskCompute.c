#include "Globals.h"
#include "Data.h"


// *****************************************************************************
// Computes the corrected measurement values for each of the four signals.
//
// This task only runs when requested by TaskMeasure.  During each "tock" cycle,
// TaskMeasure reads the current measurements and determines if any have 
// changed.  If so, TaskMeasure calls vTaskResume to add TaskCompute to the 
// ready queue.  TaskCompute computes the new values, stores them, and then 
// suspends itself.
// *****************************************************************************
void taskCompute(void* data)
{
    // Cast the raw data to a struct.
    ComputeData* cd = (ComputeData*)data;
    
    // Loop forever for the RTOS scheduler.
    while(TRUE)
    {
        // Shift each buffer entry one position to the right.
        shiftRight(cd->tmCorBuf);
        shiftRight(cd->prCorBuf);
        shiftRight(cd->syCorBuf);
        shiftRight(cd->diCorBuf);
      
        // Calculate corrected measurement values and insert into buffers. 
        cd->tmCorBuf[CURRENT] = (uchar)(5 + 0.75 * cd->tmRawBuf[CURRENT]);
        cd->prCorBuf[CURRENT] = (uchar)(8 + 3.0  * cd->prRawBuf[CURRENT]);
        cd->syCorBuf[CURRENT] = (uchar)(9 + 2.0  * cd->syRawBuf[CURRENT]);
        cd->diCorBuf[CURRENT] = (uchar)(6 + 1.5  * cd->diRawBuf[CURRENT]);

        // Sleep until woken up again by TaskMeasure.
        vTaskSuspend(computeHandle);
    }
}
