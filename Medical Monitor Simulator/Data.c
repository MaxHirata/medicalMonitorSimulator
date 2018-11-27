#include "Data.h"
#include "Globals.h"

// Define buffers for raw data.
Data         tmRawBuf[BUF_SIZE];
Data         prRawBuf[BUF_SIZE];
Data         diRawBuf[BUF_SIZE];
Data         syRawBuf[BUF_SIZE];

// Define buffers for corrected data.
Data         tmCorBuf[BUF_SIZE];
Data         prCorBuf[BUF_SIZE];
Data         diCorBuf[BUF_SIZE];
Data         syCorBuf[BUF_SIZE];

// Define buffers for EKG processing.
short     ekgRawBuf[256];
short     ekgFreqBuf[16];

// Define global variable to hold Compute task handle.
TaskHandle_t measureHandle            = NULL;
TaskHandle_t computeHandle            = NULL;
TaskHandle_t ekgCaptHandle            = NULL;
TaskHandle_t ekgProcHandle            = NULL;

// Define global variables for user interaction
Mode          mode                    = MAIN;
Measurement   measurement             = NONE;
ushort        scroll                  = 0;
Keypress      keypress                = NULL;
ushort        alarmAcknowledge        = 0;

// Define global variable for battery state
ushort        batteryState            = 200;

// Define global variables for alarms (unsigned char)
uchar        bpOutOfRange            = 0;
uchar        tempOutOfRange          = 0;
uchar        pulseOutOfRange         = 0;

// Define global variables for warnings and alarms (bool)
bool          bpHigh                  = FALSE;
bool          tempHigh                = FALSE;
bool          pulseLow                = FALSE;

char          command;
char          response;
char          message[200];

// ComandData
CommandData commandData =
{
    &command,
    &response,
    &message[200],
    &tmCorBuf[BUF_SIZE],
    &diCorBuf[BUF_SIZE],
    &syCorBuf[BUF_SIZE],
    &prCorBuf[BUF_SIZE]
};

// MeasurementData struct.
MeasurementData measureData = {
    &tmRawBuf[BUF_SIZE],
    &syRawBuf[BUF_SIZE],
    &diRawBuf[BUF_SIZE],
    &prRawBuf[BUF_SIZE],
    &measurement,
    &keypress
};

// EkgData struct
EkgData ekgData = {
    &ekgRawBuf[256],
    &ekgFreqBuf[16]
};

// ComputeData struct
ComputeData computeData = {
    &tmRawBuf[BUF_SIZE],
    &syRawBuf[BUF_SIZE],
    &diRawBuf[BUF_SIZE],
    &prRawBuf[BUF_SIZE],
    &tmCorBuf[BUF_SIZE],
    &syCorBuf[BUF_SIZE],
    &diCorBuf[BUF_SIZE],
    &prCorBuf[BUF_SIZE],
    &measurement
};

// KeypadData struct.
KeypadData keypadData = {
    (Mode*)        &mode,
    (Measurement*) &measurement,
    (ushort*)      &scroll,
    (Keypress*)    &keypress,
    (ushort*)      &alarmAcknowledge
};

// DisplayData struct
DisplayData displayData = {
    &mode,
    &measurement,
    &tmCorBuf[BUF_SIZE],
    &diCorBuf[BUF_SIZE],
    &syCorBuf[BUF_SIZE],
    &prCorBuf[BUF_SIZE],
    &batteryState,
    &scroll
};

// WarningAlarmData struct
WarningAlarmData alertData = {
    &tmCorBuf[BUF_SIZE],
    &diCorBuf[BUF_SIZE],
    &syCorBuf[BUF_SIZE],
    &prCorBuf[BUF_SIZE],
    &batteryState
};

// StatusData struct
StatusData statusData = {
    &batteryState
};

// CommunicationsData struct
CommunicationsData usbData = {
    &tmCorBuf[BUF_SIZE],
    &diCorBuf[BUF_SIZE],
    &syCorBuf[BUF_SIZE],
    &prCorBuf[BUF_SIZE]
};
