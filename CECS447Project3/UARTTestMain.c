// UARTTestMain.c
// Runs on LM4F120/TM4C123
// Used to test the UART.c driver
// Daniel Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#include "PLL.h"
#include "UART.h"
#include "tm4c123gh6pm.h"
#include <string.h>

char data[30];
unsigned char char_data;

//button init
void PortF_Init(void)
{

	unsigned long volatile delay;
	//SYSCTL_RCGCGPIO_R |= 0x20;            // 2) activate port F
  SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
	delay = SYSCTL_RCGC2_R;   
  GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock GPIO Port F
  GPIO_PORTF_CR_R |= 0x0E;         // allow changes to PF4,0
  GPIO_PORTF_DIR_R &= ~0x00;    // (c) no input
	GPIO_PORTF_DIR_R |= 0x0E;    // (c) make PF3-1 out (built-in led)
	GPIO_PORTF_AFSEL_R &= ~0x0E;  //     disable alt funct on PF4,0
  GPIO_PORTF_PCTL_R &= ~0x0000FFF0; //  configure PF4-0 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x0E;          // disable analog functionality on PF4,0
  GPIO_PORTF_DEN_R |= 0x0E;             // enable digital I/O on PF4,0
}

// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06

/*
//setup code user input
int main(void){
	
  PLL_Init();
	UART0_Init();
  UART1_Init();              // initialize UART
	//PortF_Init();
	
  while(1)
	{
		
		UART0_OutString("Please type in AT command\r\n");
		UART0_InString(data, 30);
  	UART1_OutString(strcat(data, "\r\n"));
	  UART1_InString(data, 30);
  	UART0_OutString(data);
	  UART0_OutString("\r\n\r\n");
		
		UART0_OutString("Enter any string to continue");
		UART0_InString(data, 30);
		UART0_OutString("\r\n\r\n");

  }
	
}*/

/*

//setup code hardcoded
int main(void){
	
  PLL_Init();
	UART0_Init();
  UART1_Init();              // initialize UART
	PortF_Init();
	
	UART0_OutString("AT\r\n");
	UART1_OutString("AT\r\n");
	UART1_InString(data, 30);
	UART0_OutString(data);
	UART0_OutString("\r\n\r\n");
	
  while(1)
	{
		
		UART0_OutString("AT+NAME=NT");
		UART1_OutString("AT+NAME=NT\r\n");
		UART1_InString(data, 30);
		UART0_OutString(data);
		UART0_OutString("\r\n\r\n");
		
		UART0_OutString("AT+UART=57600,0,1");
		UART1_OutString("AT+UART=57600,0,1\r\n");
		UART1_InString(data, 30);
		UART0_OutString(data);
		UART0_OutString("\r\n\r\n");
		
		UART0_OutString("AT+PSWD=1192");
		UART1_OutString("AT+PSWD=1192\r\n");
		UART1_InString(data, 30);
		UART0_OutString(data);
		UART0_OutString("\r\n\r\n");
		
		UART0_OutString("AT+ROLE=0");
		UART1_OutString("AT+ROLE=0\r\n");
		UART1_InString(data, 30);
		UART0_OutString(data);
		UART0_OutString("\r\n\r\n");
		
		UART0_OutString("AT+CLASS=1");
		UART1_OutString("AT+CLASS=1\r\n");
		UART1_InString(data, 30);
		UART0_OutString(data);
		UART0_OutString("\r\n\r\n");
		
		UART0_OutString("AT+NAME?");
		UART1_OutString("AT+NAME?\r\n");
		UART1_InString(data, 30);
		UART0_OutString(data);
		UART0_OutString("\r\n\r\n");
		
		UART0_OutString("AT+UART?");
		UART1_OutString("AT+UART?\r\n");
		UART1_InString(data, 30);
		UART0_OutString(data);
		UART0_OutString("\r\n\r\n");
		
		UART0_OutString("AT+ PSWD?");
		UART1_OutString("AT+ PSWD?\r\n");
		UART1_InString(data, 30);
		UART0_OutString(data);
		UART0_OutString("\r\n\r\n");
		
		UART0_OutString("AT+ ROLE?");
		UART1_OutString("AT+ ROLE?\r\n");
		UART1_InString(data, 30);
		UART0_OutString(data);
		UART0_OutString("\r\n\r\n");
		
		UART0_OutString("Enter any string to continue");
		UART0_InString(data, 30);
		UART0_OutString("\r\n\r\n");

  }
	
}

*/

//communication code


int main(void){
	
  PLL_Init();
	UART0_Init();
  UART1_Init();              // initialize UART
	PortF_Init();
	
	while(1)
	{
		
  UART0_OutString("Waiting for input...\r\n");
	char_data = UART1_InChar();
		
	  switch(char_data)
	  {
		case 'r':
			    UART0_OutString("red\r\n");
			    GPIO_PORTF_DATA_R = 0x02;  // LED is red;
		      break;
		case 'y':
					UART0_OutString("yellow\r\n");
		      GPIO_PORTF_DATA_R = 0x0A;  // LED is yellow;
		      break;
	  case 'g':
					UART0_OutString("green\r\n");
					GPIO_PORTF_DATA_R = 0x08;  // LED is green;
		      break;
	  case 'b':
					UART0_OutString("blue\r\n");
					GPIO_PORTF_DATA_R = 0x04;  // LED is blue;
		      break;
	  case 'l':
					UART0_OutString("light blue\r\n");
					GPIO_PORTF_DATA_R = 0x0C;  // LED is light blue;
		      break;
	  case 'w':
					UART0_OutString("white\r\n");
					GPIO_PORTF_DATA_R = 0x0E;  // LED is white;
		      break;
	  case 'p':
					UART0_OutString("purple\r\n");
					GPIO_PORTF_DATA_R = 0x06;  // LED is purple;
		      break;
		default:
			    UART0_OutString("failed\r\n");
		      break;
	  }
	
  }
	
}

