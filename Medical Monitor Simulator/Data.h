#ifndef DATA
#define DATA

#include "Globals.h"

// Enumeration with bitwise flags to represent each possible keypress.
typedef enum {            
    UP     = 1 << 0,        // GPIO_PIN_0           0000 0001
    DOWN   = 1 << 1,        // GPIO_PIN_1           0000 0010
    LEFT   = 1 << 2,        // GPIO_PIN_2           0000 0100
    RIGHT  = 1 << 3,        // GPIO_PIN_3           0000 1000
    SELECT = 1 << 4,        // Custom pattern       0001 0000
    CANCEL = 3 << 0         // UP & DOWN            0000 0011     
} Keypress;


// Enumeration for the current display mode.
typedef enum { MAIN, MEASURE, ANNUNCIATE, EKG } Mode;

// Enumeration for the current measurement.
typedef enum { NONE, PRESSURE, TEMP, PULSE, ALL } Measurement;

// Declare buffers for raw data.
extern Data        tmRawBuf[BUF_SIZE];
extern Data        prRawBuf[BUF_SIZE];
extern Data        syRawBuf[BUF_SIZE];
extern Data        diRawBuf[BUF_SIZE];

// Declare buffers for corrected data.
extern Data        tmCorBuf[BUF_SIZE];
extern Data        prCorBuf[BUF_SIZE];
extern Data        syCorBuf[BUF_SIZE];
extern Data        diCorBuf[BUF_SIZE];

// Declare buffers for EKG data
extern short     ekgRawBuf[256];
extern short     ekgFreqBuf[16];

// Declare global variables to hold task handles for on-demand tasks.
extern TaskHandle_t measureHandle;
extern TaskHandle_t computeHandle;
extern TaskHandle_t ekgProcHandle;
extern TaskHandle_t ekgCaptHandle;

// Declare global variables for user interaction
extern Mode         mode;
extern Measurement  measurement;
extern ushort       scroll;
extern Keypress     keypress;
extern ushort       alarmAcknowledge;

// Declare global variable for battery state
extern ushort       batteryState;

// Declare global variables for alarms
extern uchar        bpOutOfRange;
extern uchar        tempOutOfRange;
extern uchar        pulseOutOfRange;

// Declare global variables for warnings and alarms
extern bool         bpHigh;
extern bool         tempHigh;
extern bool         pulseLow;


extern short        ekgRawBuf[256];
extern short        ekgFreqBuf[16];

extern char         command;
extern char         response;
extern char         message[200];

// CommandData
typedef struct
{
    char*         command;
    char*         response;
    char*         message[200];
    Data*         tmCorBuf;
    Data*         diCorBuf;
    Data*         syCorBuf;
    Data*         prCorBuf;
} CommandData;


// MeasurementData
typedef struct
{
    Data*         tmRawBuf;
    Data*         prRawBuf;
    Data*         syRawBuf;
    Data*         diRawBuf;
    Measurement*  measurement;
    Keypress*     keypress;
} MeasurementData;

// ComputeData
typedef struct
{
    Data*         tmRawBuf;
    Data*         prRawBuf;
    Data*         syRawBuf;
    Data*         diRawBuf;
    Data*         tmCorBuf;
    Data*         prCorBuf;
    Data*         syCorBuf;
    Data*         diCorBuf;
    Measurement*  measurement;
    Keypress*     keypress;
} ComputeData;

// KeypadData
typedef struct
{
    Mode*         mode;
    Measurement*  measurement;
    ushort*       scroll;
    Keypress*     keypress;
    ushort*       alarmAcknowledge;
} KeypadData;

// DisplayData
typedef struct
{
    Mode*         mode;
    Measurement*  measurement;
    Data*         tmCorBuf;
    Data*         diCorBuf;
    Data*         syCorBuf;
    Data*         prCorBuf;
    ushort*       batteryState;
    ushort*       scroll; 
} DisplayData;

// WarningAlarmData
typedef struct
{
    Data*         tmCorBuf;
    Data*         diCorBuf;
    Data*         syCorBuf;
    Data*         prCorBuf;
    ushort*       batteryState;
} WarningAlarmData;

// StatusData
typedef struct
{
    ushort*       batteryState;
} StatusData;

// CommunicationsData
typedef struct
{
    Data*         tmCorBuf;
    Data*         diCorBuf;
    Data*         syCorBuf;
    Data*         prCorBuf;
} CommunicationsData;

// EKGCaptureData*struct
typedef struct
{
    short*  ekgRawBuf;
    short*  ekgFreqBuf;
} EkgData;

// Global instance members 
extern MeasurementData     measureData;
extern ComputeData         computeData;
extern KeypadData          keypadData;
extern DisplayData         displayData;
extern WarningAlarmData    alertData;
extern CommunicationsData  usbData;
extern StatusData          statusData;
extern EkgData             ekgData;
extern CommandData         commandData;

#endif