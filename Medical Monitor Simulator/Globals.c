#include "Globals.h"


// *****************************************************************************
// Write the specified text to the specified column/row of the OLED.
// *****************************************************************************
void printOLED(char* text, int col, int row)
{
    // Convert the column/row values to pixel values.
    int x = col * 12;
    int y = row * 12;

    // Print the text on the OLED at the specified coordinates.
    RIT128x96x4StringDraw(text, x, y, 15);
}

// *****************************************************************************
// Write the specified formatted text to the specified column/row of the OLED.
// *****************************************************************************
void printfOLED(char* input, int col, int row, void* data)
{
    // Create a static buffer initialized to NULL.
    static char buffer[TEXT_BUFFER_SIZE] = { 0 };
    
    // Fill buffer with formatted text.
    snprintf(buffer, TEXT_BUFFER_SIZE, input, data);

    // Send the contents of the buffer to the OLED.
    printOLED(buffer, col, row);
}

// *****************************************************************************
// Send the specified formatted text over USB to a terminal on a connected PC.
// *****************************************************************************
void printfUSB(char* input, uchar* data)
{
    // Create a static buffer initialized to NULL.
    static char buffer[TEXT_BUFFER_SIZE] = { 0 };

    // Fill buffer with formatted text.
    snprintf(buffer, TEXT_BUFFER_SIZE, input, data);

    // Send the contents of the buffer over USB.
    UARTSend((uchar const*)buffer, TEXT_BUFFER_SIZE);
}


// *****************************************************************************
// Shifts the current items in the buffer one slot to the right.
// *****************************************************************************
void shiftRight(Data* buffer)
{
    // Shift each item in the buffer to the right.
    for (int i = BUF_SIZE - 1; i > 0; i--)
    {
        buffer[i] = buffer[i - 1];
    }
    buffer[0] = NULL;
}

