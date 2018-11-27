#include "Globals.h"
#include "Data.h"
#include "Interrupts.h"


// *****************************************************************************
// Initializes the GPIO pins for the specified module, port and pin.
// *****************************************************************************
void configGPIO(ulong peripheral, ulong port, ulong pin)
{
    // Enable the GPIO module.
    SysCtlPeripheralEnable(peripheral);
    
    // Enable the GPIO pin to read the specified button.  
    GPIODirModeSet(port, pin, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(port, pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

// *****************************************************************************
// Makes the system calls to initialize the RTOS scheduler.
// *****************************************************************************
void initRTOS(void)
{
    /* If running on Rev A2 silicon, turn the LDO voltage up to 2.75V.  This is
    a workaround to allow the PLL to operate reliably. */
    if (DEVICE_IS_REVA2)
    {
        SysCtlLDOSet(SYSCTL_LDO_2_75V);
    }

    /* Set the clocking to run from the PLL at 50 MHz */
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);
//    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

    /*     Enable Port F for Ethernet LEDs
        LED0        Bit 3   Output
        LED1        Bit 2   Output */
    configGPIO(SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, (GPIO_PIN_2 | GPIO_PIN_3));
}

// *****************************************************************************
// Makes the system calls to initialize the OLED timings.
// *****************************************************************************
void initOLED(void)
{
    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

    // Initialize the OLED printOLED and display current status.
    RIT128x96x4Init(1000000);

}

// *****************************************************************************
// Makes the system calls to initialize the hardware timers.
// *****************************************************************************
void initTimers(void)
{
    // Initialize the timers used for counting ticks and heartbeats.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    // Enable processor interrupts.
    IntMasterEnable();

    // Set the initial periods for the ticks timer and the heartbeats timer.
    setTimer(TIMER0_BASE, INT_TIMER0A, 20);
    setTimer(TIMER1_BASE, INT_TIMER1A, 1 / 5.0);
}


// *****************************************************************************
// Makes the system calls to initialize each of the five buttons on the board.
// *****************************************************************************
void initButtons(void)
{
    configGPIO(SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_0); // Up
    configGPIO(SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_1); // Down
    configGPIO(SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_2); // Left
    configGPIO(SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, GPIO_PIN_3); // Right
    configGPIO(SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_1); // Select
}

// *****************************************************************************
// Makes the system calls to initialize the on-board speaker.
// *****************************************************************************
void initSpeaker()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);

    // Set GPIO as a PWM pin. It is used to output the PWM1 signal.
    GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);

    // Compute the PWM period based on the system clock.
    unsigned long ulPeriod = SysCtlClockGet() / 440;

    // Set the PWM period to 440 (A) Hz.
    PWMGenConfigure(PWM_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM_BASE, PWM_GEN_0, ulPeriod);

    // Set PWM0 to a duty cycle of 25% and PWM1 to a duty cycle of 75%.
    PWMPulseWidthSet(PWM_BASE, PWM_OUT_1, ulPeriod * 3 / 4);

    // Enable the PWM generator.
    PWMGenEnable(PWM_BASE, PWM_GEN_0);
}

// *****************************************************************************
// Makes the system calls to initialize the ADC for the temperature sensor.
// *****************************************************************************
void initTemp()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCSequenceDisable(ADC0_BASE, 3);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
}

// *****************************************************************************
// Makes the system calls to enable serial communication over UART.
//
// NB: Use the following settings with Hyper Terminal:
//	Port:   Uart
//      Baud:   9600 bps
//      Data:   8 Bit
//      Parity: None
//      Stop:   1 Bit
// *****************************************************************************
void initUART() 
{
    // Enable the peripherals 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable processor interrupts.
    IntMasterEnable();

    // Set GPIO A0 and A1 as UART pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Enable the UART interrupt.
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

// *****************************************************************************
// Makes the system calls to initialize the board, RTOS and interrupts.
// *****************************************************************************
void taskStartup(void* data)
{
    // Perform initialization steps.
    initRTOS();         // RTOS
    initOLED();         // OLED display timings
    initTimers();       // Interrupts for hardware timers
    initButtons();      // Interrupts for buttons on board
    initSpeaker();      // Interrupts for PWM calls for speaker
    initTemp();         // ADC for the temperature sensor
    initUART();         // Interrupts for serial communications
    
    // Assign default values to raw measurement global variables.
    prRawBuf[CURRENT]     = 20;
    tmRawBuf[CURRENT]     = 20;
    syRawBuf[CURRENT]     = 20;
    diRawBuf[CURRENT]     = 20;
}
