#include "Globals.h"
#include "Data.h"
#include "Interrupts.h"


// Global variables for this interrupt handler.
ulong ticks              = 0;      // Count of total ticks (20 per second)
ulong heartbeats         = 0;	   // Count of total heartbeats (variable rate)


// *****************************************************************************
// Reads the current keypress during each tick interrupt by reading and 
// combining the bit patterns for pad buttons and for the select key.
//
// Each bit pattern is stored in a custom "Keypress" enum type. The enum
// corresponds to the following bit patterns for each key:
//     UP     = 1 << 0,        // GPIO_PIN_0           0000 0001
//     DOWN   = 1 << 1,        // GPIO_PIN_1           0000 0010
//     LEFT   = 1 << 2,        // GPIO_PIN_2           0000 0100
//     RIGHT  = 1 << 3,        // GPIO_PIN_3           0000 1000
//     SELECT = 1 << 4,        // Custom pattern       0001 0000
//     CANCEL = 1 << 5         // Custom pattern       0010 0000     
//
// *****************************************************************************
void readKeypress(void)
{
    // Store the last keypress for debounce filter.
    static Keypress keyLast;
    
    // Read bit pattern for the four direction keys (bits 0, 1, 2, and 3).
    uint key = (Keypress)(GPIOPinRead(GPIO_PORTE_BASE, UP | DOWN | LEFT | RIGHT) ^ 0x0f);
    
    // If select is pressed, toggle "select" (bit 4) of the keypress flag.
    bool sel = (bool)(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) == 0);
    if (sel)
    {
        key = key | 1 << 4;
    }
    
    // Record the last keypress once the user has released the key (debounce).
    if (key == NULL && keyLast != NULL)
    {
        keypress = keyLast;
    }
    keyLast = (Keypress)key;
}

// *****************************************************************************
// Interrupt handler for the tick timer (every 50 ms).
// *****************************************************************************
void tickHandler(void)
{
    // Clear the timer interrupt.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Increment the tick count.
    ticks++;
    
    // Read the current keypress and save to a global variable.
    readKeypress();
}

// *****************************************************************************
// Interrupt handler for the heartbeat timer (with variable pulse rate).
// *****************************************************************************
void heartbeatHandler(void)
{
    // Clear the timer interrupt.
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Increment the heartbeat count.
    heartbeats++;

    // Have the beats-per-minute slowly rise from 20 to 200, then repeat.
    uint elapsedSeconds = ticks / 10;
    uint bpm = 20 + elapsedSeconds % 180;
    TimerLoadSet(TIMER1_BASE, TIMER_A, 60 * SysCtlClockGet() / bpm);
}


// *****************************************************************************
// The UART interrupt handler.
// *****************************************************************************
void UARTIntHandler(void)
{
    unsigned long ulStatus;

    // Get the interrrupt status.
    ulStatus = UARTIntStatus(UART0_BASE, true);

    // Clear the asserted interrupts.
    UARTIntClear(UART0_BASE, ulStatus);

    // Loop while there are characters in the receive FIFO buffer.
    while (UARTCharsAvail(UART0_BASE))
    {
        // Read the next character from the UART and write it back to the UART.
        UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE));
    }
}


// *****************************************************************************
// Sets the specified timer/interrupt to fire at the specified frequency.
// *****************************************************************************
void setTimer(ulong timerBase, ulong interrupt, double ticksPerSec)
{
    // Configure a 32-bit periodic timer using TIMER_A.
    TimerConfigure(timerBase, TIMER_CFG_PERIODIC);
    TimerLoadSet(timerBase, TIMER_A, (int)(SysCtlClockGet() / ticksPerSec));

    // Setup the interrupt for the timer timeouts.
    IntEnable(interrupt);
    TimerIntEnable(timerBase, TIMER_TIMA_TIMEOUT);

    // Enable the timer.
    TimerEnable(timerBase, TIMER_A);
}
