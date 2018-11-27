#ifndef GLOBALS
#define GLOBALS

// Standard includes.
#include <stdio.h>              // Printf
#include <string.h>             // Memset

// RTOS kernel includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// Stellaris includes
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_uart.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "drivers/rit128x96x4.h"
#include "driverlib/uart.h"
#include "boot_loader/bl_uart.h"
#include "inc/lm3s8962.h"
#include "driverlib/adc.h"
#include <stdio.h>

#define BUF_SIZE 7

// Define types for boolean and function pointer, and alias "unsigned" types.
typedef enum { FALSE = 0, TRUE = 1 } bool;
typedef void(*TaskFunction)(void*);
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;

// Define enumeration with four task priority levels for RTOS.
typedef enum { 
    P_IDLE = 0, 
    P_BKGD = 1,
    P_TICK = 2,
    P_TOCK = 3
} Priority;



// Define type for a uchar buffer with eight members.
typedef uchar Data;

typedef uint FftData;

// Define the size of a text buffer for text-related functions.
#define TEXT_BUFFER_SIZE 40

// Prototypes for executing each TCB with its associated data.
void taskUSB(void* data);
void taskCompute(void* data);
void taskDisplay(void* data);
void taskKeypad(void* data);
void taskMeasure(void* data);
void taskScheduler(void* data);
void taskStartup(void* data);
void taskStatus(void* data);
void taskAlert(void* data);
void taskEKGCapture(void* data);
void taskEKGProcess(void* data);
void taskCommand(void* data);

// Position of "current" and "last" members in data queue.
#define CURRENT 0
#define LAST 1

// Writes the specified text to the specified column/row of the OLED.
void printOLED(char* input, int col, int row);

// Writes the specified formatted text to the specified column/row of the OLED.
void printfOLED(char* input, int col, int row, void* format);

// Sends the specified formatted text over USB to a terminal on a connected PC.
void printfUSB(char* input, Data* data);

void shiftRight(Data* data);

#define TICKS_PER_SECOND = 20;

// Declarations for "tick" and "tock" delays
#define TICK_DELAY pdMS_TO_TICKS(50)
#define TOCK_DELAY pdMS_TO_TICKS(5000)


#endif
