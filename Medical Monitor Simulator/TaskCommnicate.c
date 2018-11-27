#include "Globals.h"
#include "Interrupts.h"
#include "Data.h"
#include <stdio.h>

#define BUFFER_SIZE 40

char* text[] = { 
    "Temp:    %i C        ",
    "Sys BP:  %i mm Hg    ",
    "Dia BP:  %i mm Hg    ",
    "Pulase:  %i BPM      ",
    "Battery: %i          "
};


// *****************************************************************************
// Sends out the string to the UART Port
// NOTE: This Function was taken from the Stellaris boot_serial-Debug program
// *****************************************************************************
void UARTSend(const unsigned char *pucData, unsigned long ulSize)
{
    // Transmit the number of bytes requested on the UART port.
    while (ulSize--)
    {
        // Make sure that the transmit FIFO is not full.
        while ((HWREG(UART0_BASE + UART_O_FR) & UART_FR_TXFF))
        {
        }

        // Send out the next byte.
        HWREG(UART0_BASE + UART_O_DR) = *pucData++;
    }

    // Wait until the UART is done transmitting.
    UARTFlush();
}


// *****************************************************************************
// Pauses execution until all data has been transmitted by the UART port.
// NOTE: This function was taken from the Stellaris boot_serial-Debug program
// *****************************************************************************
void UARTFlush(void)
{
    // Wait for the UART FIFO to empty and then wait for the shifter to get the
    // bytes out the port.
    while (!(HWREG(UART0_BASE + UART_O_FR) & UART_FR_TXFE))
    {
    }

    // Wait for the FIFO to not be busy so that the shifter completes.
    while ((HWREG(UART0_BASE + UART_O_FR) & UART_FR_BUSY))
    {
    }
}


// *****************************************************************************
// Formats the corrected data (temp, BP, and pulse) into pretty-print strings
// and transmits the strings over UART to display in HyperTerminal on a PC.
// *****************************************************************************
void taskUSB(void* data)
{
    while(TRUE)
    {
        // Cast the raw data to an appropriate struct.
        CommunicationsData* cd = (CommunicationsData*)data;

        // Format the corrected data into strings and transmit over UART. 
        printfUSB(text[0], &cd->tmCorBuf[CURRENT]);
        printfUSB(text[1], &cd->syCorBuf[CURRENT]);
        printfUSB(text[2], &cd->diCorBuf[CURRENT]);
        printfUSB(text[3], &cd->prCorBuf[CURRENT]);
        printfUSB("\r\n", NULL);
        
        // Sleep until the next tick period.
        vTaskDelay(TICK_DELAY);
    }
}
