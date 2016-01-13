
/* Standard includes. */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driverlib/rom.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"

/* BRTOS includes. */
#include "BRTOS.h"
#include "tarefas.h"

BRTOS_TH thst, th1, th2, th3, th4, th5, th6, th7;

/*
 * main.c
 */
int main(void) {
	  //
	  // Set the system clock to run at 80Mhz off PLL with external crystal as
	  // reference.
	  //
	  ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
	                     SYSCTL_OSC_MAIN);

	  // Enable stacking for interrupt handlers.  This allows floating-point
	  // instructions to be used within interrupt handlers, but at the expense of
	  // extra stack usage.
	  //
	  ROM_FPUEnable();
	  ROM_FPULazyStackingEnable();

	  // Initialize BRTOS
	  BRTOS_Init();

	  if(InstallTask(&System_Time,"System Time",384,31,&thst) != OK)
	  {
	    // Oh Oh
	    // Não deveria entrar aqui !!!
	    while(1){};
	  };

	  if(InstallTask(&exec,"Teste 1",384,18,&th1) != OK)
	  {
	    // Oh Oh
	    // Não deveria entrar aqui !!!
	    while(1){};
	  };


	  if(InstallTask(&exec2,"Teste 2",384,19,&th2) != OK)
	  {
	    // Oh Oh
	    // Não deveria entrar aqui !!!
	    while(1){};
	  };

	  if(InstallTask(&exec3,"Teste 3",384,20,&th3) != OK)
	  {
	    // Oh Oh
	    // Não deveria entrar aqui !!!
	    while(1){};
	  };

	  if(InstallTask(&Keyboard_Handler,"Keyb Handler",384,25,&th4) != OK)
	  {
	    // Oh Oh
	    // Não deveria entrar aqui !!!
	    while(1){};
	  };

	  if(InstallTask(&Keyb_Task,"Keyb Task",640,4,&th5) != OK)
	  {
	    // Oh Oh
	    // Não deveria entrar aqui !!!
	    while(1){};
	  };

	  if(InstallTask(&Terminal,"Terminal",384,5,&th6) != OK)
	  {
	    // Oh Oh
	    // Não deveria entrar aqui !!!
	    while(1){};
	  };

	  if(InstallTask(&USB_Task,"USB Terminal",1024,10,&th7) != OK)
	  {
	    // Oh Oh
	    // Não deveria entrar aqui !!!
	    while(1){};
	  };


	  // Start Task Scheduler
	  if(BRTOSStart() != OK)
	  {
	    // Oh Oh
	    // Não deveria entrar aqui !!!
	    for(;;)
	    {
	    }
	  }
	
	return 0;
}
