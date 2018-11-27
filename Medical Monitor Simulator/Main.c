// *****************************************************************************
// Sources:
//   Timer-related code based on "timers" project from Stellaris examples.
//   UART-related code based on "uart_echo" project from Stellaris examples.
//   RTOS-related code based on RTOS demo app for Stellaris board.
//   Button-related code based on qs_ek-lm3s8962 from Stellaris examples.
// *****************************************************************************

#include "Globals.h"


// *****************************************************************************
// Error routine that is called if the driver library encounters an error.x
// *****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

// *****************************************************************************
// Initialize system and begin the main thread of execution.
// *****************************************************************************
 int main(void)
{
    // Make the startup API calls for the board and the interrupts.
    taskStartup(NULL);

    // Begin the main thread of execution.
    taskScheduler(NULL);
}
