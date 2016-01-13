/****************************************************************************************
* 
*   FILE        :    usb_terminal_commands.c
*   DATE CREATED:    
*   DESCRIPTION	:    
*
*   Author		:     
*  	Location	:    
*   Purpose		:
*   Copyright	:    
*                                                  
* UPDATED HISTORY:
*
* REV   YYYY.MM.DD  AUTHOR          	DESCRIPTION OF CHANGE
* ---   ----------  ------          	--------------------- 
* 1.0   2011.05.12  Gustavo Denardin	Initial version
*
****************************************************************************************/

/*
 * usb_terminal_commands.c
 *
 *  Created on: 12/05/2011
 *      Author: gustavo
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "usb_terminal.h"
#include "usb_terminal_commands.h"
#include "virtual_com.h"
#include "OSInfo.h"
#include "BRTOS.h"

INT8U entradas[CONSOLE_BUFFER_SIZE]; //vetor para a entrada de dados



// BRTOS version Command
void usb_cmd_ver(char *param)
{
  (void)*param;
  printf_usb("\n\r");
  printf_usb((CHAR8*)version);
  printf_usb("\n\r");
}

const command_t usb_ver_cmd = {
  "ver", usb_cmd_ver, "BRTOS Version"
};


char text[384];
// TOP Command (similar to the linux command)
void usb_cmd_top(char *param)
{
  (void)param;
  printf_usb("\n\r");
  OSCPULoad(text);
  printf_usb(text);
  printf_usb("\n\r");
  OSUptimeInfo(text);
  printf_usb(text);
  printf_usb("\n\r");
  OSAvailableMemory(text);
  printf_usb(text);
  printf_usb("\n\r");
  OSTaskList(text);
  printf_usb(text);
  printf_usb("\n\r");
}

const command_t usb_top_cmd = {
  "top", usb_cmd_top, "BRTOS TOP"
};




// Print a string in the terminal
void cmd_echo(char *param)
{
	  printf_usb("\n\r");
	  printf_usb((CHAR8*)param);
}

const command_t echo_cmd = {
  "echo", cmd_echo, "Print the string in the terminal"
};


// Function used to print a string in the terminal by other tasks
void echo (char *string, char Terminalbackup)
{
    INT8U backup_cnt = 0;
    INT8U i = 0;
    char backup[(CONSOLE_BUFFER_SIZE/2)];
	char *command_start = "echo ";
    char *command_end   = "\n\r";
    
    if (Terminalbackup == TRUE)
    {
    	backup_cnt = TerminalBackup(&backup[0]);
    }
    
    SetSilentMode(TRUE);
    while (*command_start)
    {
			if (OSQueuePost(USB, *command_start++) == BUFFER_UNDERRUN)
			{
			  // Problema: Estouro de buffer
			  OSQueueClean(USB);
			}
    }    
    
	while (*string)
    {
			if (OSQueuePost(USB, *string++) == BUFFER_UNDERRUN)
			{
			  // Problema: Estouro de buffer
			  OSQueueClean(USB);
			}
    }
    
    while (*command_end)
    {
			if (OSQueuePost(USB, *command_end++) == BUFFER_UNDERRUN)
			{
			  // Problema: Estouro de buffer
			  OSQueueClean(USB);
			}
    }
    
    if (Terminalbackup == TRUE)
    {
		for(i=0;i<backup_cnt;i++)
		{
			if (OSQueuePost(USB, backup[i]) == BUFFER_UNDERRUN)
			{
			  // Problema: Estouro de buffer
			  OSQueueClean(USB);
			}    	
		}
    }
}


void Reason_of_Reset(void)
{
  INT32U reason = 0;

  reason = ROM_SysCtlResetCauseGet();

  if (reason&1)  		printf_usb("Reset caused by reset pin.\n\r");
  if (reason&2)  		printf_usb("Power-on Reset (Cold reset).\n\r");
  if (reason&4)  		printf_usb("Brown-out reset.\n\r");
  if (reason&8)  		printf_usb("Reset caused by watchdog 0.\n\r");
  if (reason&16) 		printf_usb("Software reset.\n\r");
  if (reason&32) 		printf_usb("Reset caused by watchdog 1.\n\r");
  if (reason&0x10000)	printf_usb("Reset caused by Main Oscillator failure.\n\r");
}

// Reason of Reset Command
void usb_cmd_rst(char *param)
{
  (void)*param;
  printf_usb("\n\r");
  Reason_of_Reset();
}

const command_t usb_rst_cmd = {
  "rst", usb_cmd_rst, "CPU Reason of the Reset"
};

