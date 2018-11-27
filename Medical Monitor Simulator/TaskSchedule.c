#include "Globals.h"
#include "Data.h"
#include "Interrupts.h"

// The period of the system clock in nano seconds.
#define mainNS_PER_CLOCK ( unsigned long ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )




void vApplicationTickHook(void)
{
}

void vAssertCalled(const char *pcFile, unsigned long ulLine)
{
    volatile unsigned long ulSetTo1InDebuggerToExit = 0;

    taskENTER_CRITICAL();
    {
        while (ulSetTo1InDebuggerToExit == 0)
        {
            /* Nothing do do here.  Set the loop variable to a non zero value in
            the debugger to step out of this function to the point that caused
            the assertion. */
            (void)pcFile;
            (void)ulLine;
        }
    }
    taskEXIT_CRITICAL();
}


// *****************************************************************************
// Establishes RTOS schedule and then initiates the main thread of execution.
// 
// The tasks havethe following schedule in RTOS:
//      Background:                  Alert
//      Every "tick" (50 ms):        Keypad, then Display, then Communicate
//      Every "tock" (five secs):    Measure, then Compute, then Status
// 
// During "tock" cycles, the measurement task has the hithest priority.  This
// ensures that we measure the latest data before running compute.
// *****************************************************************************
void taskScheduler(void* data)
{
    // Define background task.
    xTaskCreate(taskAlert,    NULL, 100, &alertData,  P_BKGD, NULL);
  
    // Define "tick" tasks (every 50 ms).
    xTaskCreate(taskKeypad,     NULL, 100, &keypadData,  P_TICK, NULL);
    xTaskCreate(taskDisplay,    NULL, 100, &displayData, P_TICK, NULL);
    xTaskCreate(taskUSB,        NULL, 100, &usbData,     P_TICK, NULL);
    xTaskCreate(taskCommand,    NULL, 100, &commandData, P_TICK, NULL);
  
    // Define "tock" tasks (every five seconds).
    xTaskCreate(taskStatus,     NULL, 100, &statusData,  P_TOCK, NULL);
 
    // Define "on demand" tasks (only scheduled when needed).
    xTaskCreate(taskMeasure,    NULL, 100, &measureData, P_TICK, &measureHandle);
    xTaskCreate(taskCompute,    NULL, 100, &computeData, P_TOCK, &computeHandle);
    xTaskCreate(taskEKGCapture, NULL, 100, &ekgData,     P_TICK, &ekgCaptHandle);
    xTaskCreate(taskEKGProcess, NULL, 2000, &ekgData,     P_TICK, &ekgProcHandle);
    
    // Put the last tasks immediately in suspended mode.
    vTaskSuspend(measureHandle);
    vTaskSuspend(computeHandle);
    vTaskSuspend(ekgCaptHandle);
    vTaskSuspend(ekgProcHandle);

    // Start the tasks.
    vTaskStartScheduler();

    // Should never get here.
    return;
}

