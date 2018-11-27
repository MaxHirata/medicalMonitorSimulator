#include "Globals.h"
#include "Data.h"

// *****************************************************************************
// Task to decrement the battery counter during each "tock".
// *****************************************************************************
void taskStatus(void* data)
{
     // Cast the raw data to a MeasurementData struct.
    StatusData* sd = (StatusData*)data;

    while(TRUE)
    {
        // Decrement the battery counter.
        (*sd->batteryState)--;
                
        // Sleep until the next tock period.
        vTaskDelay(TOCK_DELAY);
    }
}
