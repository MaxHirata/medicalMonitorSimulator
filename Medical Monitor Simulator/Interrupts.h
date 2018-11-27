#ifndef INTERRUPTS
#define INTERRUPTS

#include "Globals.h"


//*****************************************************************************
// Counters for tracking the total count of elapsed ticks and heartbeats
//*****************************************************************************
extern ulong ticks;	        // Count of total ticks (1/20 second)
extern ulong heartbeats;	// Count of total heartbeats (variable rate)


//*****************************************************************************
// Sets the specified timer/interrupt to fire at the given frequency.
//*****************************************************************************
extern void setTimer(ulong timerBase, ulong interrupt, double ticksPerSec);



#endif