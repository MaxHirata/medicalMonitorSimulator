#include "Globals.h"
#include "Data.h"
#include "optfft.h"
#include <string.h>


short imaginaryBuffer[256];

// *****************************************************************************
// Task to calculate the peak EKG amplitude from the sampled sine wave.
// *****************************************************************************
void taskEKGProcess(void* data)
{
    // Cast the raw data to an EKG struct.
    EkgData* ed = (EkgData*)data;
    
    // Loop forever for RTOS scheduler.
    while(TRUE)
    {
        // Zero out the imaginary buffer.
        memset(imaginaryBuffer, NULL, 256);

        // Call the FFT algorithm
        short result = optfft(ed->ekgRawBuf, imaginaryBuffer);
        
        // Print the result to the display.
        printfOLED("%i ", 2, 6, (void*)result);
        
        // Sleep until woken up again by TaskEKGCapture.
        vTaskSuspend(ekgProcHandle);
    }
}
