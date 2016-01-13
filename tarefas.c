#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "BRTOS.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/watchdog.h"
#include "driverlib/gpio.h"
#include "rgb.h"
#include "buttons.h"
#include "driverlib/uart.h"

#include "rgb.h"
#include "buttons.h"
#include "OSInfo.h"

#include "virtual_com.h"
#include "usb_terminal.h"
#include "usb_terminal_commands.h"


void System_Time(void)
{
   // task setup
   INT16U i = 0;

   OSResetTime();
   Init_Calendar();

   // task main loop
   for (;;)
   {
      #if (WATCHDOG == 1)
        __RESET_WATCHDOG();
      #endif

      DelayTask(10);
      i++;

      if (i >= 100)
      {
        i = 0;
        OSUpdateUptime();
        OSUpdateCalendar();
      }
   }
}

void exec(void)
{
  while(1)
  {
	  GPIOPinWrite(GREEN_GPIO_BASE, GREEN_GPIO_PIN, GREEN_GPIO_PIN);
	  DelayTask(200);
	  GPIOPinWrite(GREEN_GPIO_BASE, GREEN_GPIO_PIN, 0);
	  DelayTask(200);
  }
}



void exec2(void)
{
  while(1)
  {
	  GPIOPinWrite(RED_GPIO_BASE, RED_GPIO_PIN, RED_GPIO_PIN);
	  DelayTask(150);
	  GPIOPinWrite(RED_GPIO_BASE, RED_GPIO_PIN, 0);
	  DelayTask(150);
  }
}



void exec3(void)
{
	//RGBInit(TRUE);
    // Enable the GPIO Port and Timer for each LED
    SysCtlPeripheralEnable(RED_GPIO_PERIPH);
    SysCtlPeripheralEnable(GREEN_GPIO_PERIPH);
    SysCtlPeripheralEnable(BLUE_GPIO_PERIPH);

    // Reconfigure each LED's GPIO pad for timer control
    GPIOPadConfigSet(GREEN_GPIO_BASE, GREEN_GPIO_PIN, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
    GPIOPinTypeGPIOOutput(GREEN_GPIO_BASE, GREEN_GPIO_PIN);

    GPIOPadConfigSet(BLUE_GPIO_BASE, BLUE_GPIO_PIN, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
    GPIOPinTypeGPIOOutput(BLUE_GPIO_BASE, BLUE_GPIO_PIN);

    GPIOPadConfigSet(RED_GPIO_BASE, RED_GPIO_PIN, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
    GPIOPinTypeGPIOOutput(RED_GPIO_BASE, RED_GPIO_PIN);

	while(1)
	{
		GPIOPinWrite(BLUE_GPIO_BASE, BLUE_GPIO_PIN, BLUE_GPIO_PIN);
		DelayTask(100);
		GPIOPinWrite(BLUE_GPIO_BASE, BLUE_GPIO_PIN, 0);
		DelayTask(100);
	}
}

extern BRTOS_TH th1, th2, th3, th4, th5,th6;
// Declares a queue structure for the keyboard
BRTOS_Queue  *qKeyboard;

// Declares a semaphore structure for the keyboard
BRTOS_Sem 	 *sKeyboard;

void Keyboard_Handler(void)
{
   // task setup
   INT8U  key      = NO_KEY;
   INT32U read = 0;

   ButtonsInit();

   if (OSSemCreate(0,&sKeyboard) != ALLOC_EVENT_OK)
   {
     // Oh Oh
     // Não deveria entrar aqui !!!
     while(1){};
   };

   if (OSQueueCreate(64, &qKeyboard) != ALLOC_EVENT_OK)
   {
     // Oh Oh
     // Não deveria entrar aqui !!!
     while(1){};
   };

   // task main loop
   for (;;)
   {
      // Wait for a keyboard interrupt
      OSSemPend (sKeyboard,0);
      DelayTask(50);

      read = GPIOPinRead(BUTTONS_GPIO_BASE, ALL_BUTTONS);

      // Find out which key was pressed
      key = (INT8U)read;

      // Copy the key to the keyboard buffer
      if(key != NO_KEY)
      {
    	if (OSQueuePost(qKeyboard, key) == BUFFER_UNDERRUN)
        {
          // Buffer overflow
          OSQueueClean(qKeyboard);
        }
      }

      key = NO_KEY;
      DelayTask(200);
      // Enable interrupt to the next key detection
      GPIOIntEnable(BUTTONS_GPIO_BASE, ALL_BUTTONS);
   }
}



void UARTPutChar(uint32_t ui32Base, char ucData);
void UARTPutString(uint32_t ui32Base, char *string);

void Keyb_Task(void)
{
	char text[320];
	uint8_t key = 0;

	while(1)
	{
		if(!OSQueuePend(qKeyboard, &key, 0))
		{
			switch(key)
			{
				case LEFT_KEY:
					// Block LED RGB task
					UARTPutString(UART0_BASE, "Botão Esquerdo pressionado!\n\r");
					OSAvailableMemory(text);
					UARTPutString(UART0_BASE, text);
					OSCPULoad(text);
					UARTPutString(UART0_BASE, text);
					UARTPutString(UART0_BASE, "\n\r");
					break;

				case RIGHT_KEY:
					// UnBlock LED RGB task
					UARTPutString(UART0_BASE, "Botão Direito pressionado!\n\r");
					OSUptimeInfo(text);
					UARTPutString(UART0_BASE, text);
					UARTPutString(UART0_BASE, "\n\r");
					break;

				case BOTH_KEY:
					UARTPutString(UART0_BASE, "Ambos os botões pressionados!\n\r");
					OSTaskList(text);
					UARTPutString(UART0_BASE, text);
					UARTPutString(UART0_BASE, "\n\r");
					break;

				default:
					break;
			}
		}
	}
}



// Declares a queue structure for the UART
BRTOS_Queue  *qUART;

// Declares a semaphore structure for the UART
BRTOS_Sem 	*sUART;

// Declares a mutex structure for the UART
BRTOS_Mutex *mutexTx;

void Terminal(void)
{
	char data;

   if (OSSemCreate(0,&sUART) != ALLOC_EVENT_OK)
   {
     // Oh Oh
     // Não deveria entrar aqui !!!
     BlockTask(th6);
   };

   if (OSMutexCreate(&mutexTx,6) != ALLOC_EVENT_OK)
   {
     // Oh Oh
     // Não deveria entrar aqui !!!
     BlockTask(th6);
   };

   if (OSQueueCreate(64, &qUART) != ALLOC_EVENT_OK)
   {
     // Oh Oh
     // Não deveria entrar aqui !!!
	 BlockTask(th6);
   };

	//
	// Enable the peripherals used by this example.
	// The UART itself needs to be enabled, as well as the GPIO port
	// containing the pins that will be used.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	//
	// Configure the GPIO pin muxing for the UART function.
	// This is only necessary if your part supports GPIO pin function muxing.
	// Study the data sheet to see which functions are allocated per pin.
	// TODO: change this to select the port/pin you are using
	//
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	//
	// Since GPIO A0 and A1 are used for the UART function, they must be
	// configured for use as a peripheral function (instead of GPIO).
	// TODO: change this to match the port/pin you are using
	//
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//
	// Configure the UART for 115,200, 8-N-1 operation.
	// This function uses SysCtlClockGet() to get the system clock
	// frequency.  This could be also be a variable or hard coded value
	// instead of a function call.
	//
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
						(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
						 UART_CONFIG_PAR_NONE));

	UARTFIFODisable(UART0_BASE);

	//
	// Enable the UART interrupt.
	//
	ROM_IntEnable(INT_UART0);
	ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    //
    // Put a character to show start of example.  This will display on the
    // terminal.
    //
    UARTPutString(UART0_BASE, "Iniciou!\n\r\n\r");

    while(1)
    {
    	if(!OSQueuePend(qUART, (INT8U*)&data, 0))
    	{
    		if (data != 13)
    		{
    			UARTPutChar(UART0_BASE, data);
    		}else
    		{
    			UARTPutString(UART0_BASE, "\n\r");
    		}
    	}
    }
}



//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void UARTIntHandler(void)
{
    uint32_t ui32Status;
    char data;

    //
    // Get the interrrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);

	UARTIntClear(UART0_BASE, ui32Status);

    if ((ui32Status&UART_INT_RX) == UART_INT_RX)
    {
		//
		// Loop while there are characters in the receive FIFO.
		//
		while(ROM_UARTCharsAvail(UART0_BASE))
		{
			//
			// Read the next character from the UART and write it back to the UART.
			//
			data = (char)ROM_UARTCharGetNonBlocking(UART0_BASE);
			OSQueuePost(qUART, data);
		}
    }

    if ((ui32Status&UART_INT_TX) == UART_INT_TX)
    {
    	ROM_UARTIntDisable(UART0_BASE, UART_INT_TX);

    	// Call the keyboard analysis task
        OSSemPost(sUART);
    }

    // ************************
    // Interrupt Exit
    // ************************
    OS_INT_EXIT_EXT();
    // ************************
}


void UARTPutChar(uint32_t ui32Base, char ucData)
{
    OSMutexAcquire(mutexTx,0);
	//
	// Send the char.
	//
	HWREG(ui32Base + UART_O_DR) = ucData;

	//
	// Wait until space is available.
	ROM_UARTIntEnable(UART0_BASE, UART_INT_TX);
	// Wait indefinitely for a UART interrupt
	OSSemPend(sUART,0);
	OSMutexRelease(mutexTx);
}


void UARTPutString(uint32_t ui32Base, char *string)
{
    OSMutexAcquire(mutexTx,0);
	while(*string)
	{
		//
		// Send the char.
		//
		HWREG(ui32Base + UART_O_DR) = 0x72;// *string;

		ROM_UARTIntEnable(UART0_BASE, UART_INT_TX);
		// Wait indefinitely for a UART interrupt
		OSSemPend(sUART,0);

		string++;
	}

	OSMutexRelease(mutexTx);
}





void USB_Task(void)
{

    //
    // Configure the required pins for USB operation.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    ROM_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_4);

    //
    // Not configured initially.
    //
    g_bUSBConfigured = false;

    //
    // Initialize the transmit and receive buffers.
    //
    USBBufferInit(&g_sTxBuffer);
    USBBufferInit(&g_sRxBuffer);

    //
    // Set the USB stack mode to Device mode with VBUS monitoring.
    //
    USBStackModeSet(0, eUSBModeForceDevice, 0);

    //
    // Pass our device information to the USB library and place the device
    // on the bus.
    //
    USBDCDCInit(0, &g_sCDCDevice);
    Virtual_Comm_Init();

	(void)usb_terminal_add_cmd((command_t*)&usb_ver_cmd);
	(void)usb_terminal_add_cmd((command_t*)&usb_top_cmd);
	(void)usb_terminal_add_cmd((command_t*)&usb_rst_cmd);
	(void)usb_terminal_add_cmd((command_t*)&echo_cmd);

	while(1)
	{
		/* Call the application task */
		usb_terminal_process();
	}
}
