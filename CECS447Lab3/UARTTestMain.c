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

unsigned char qaFlag = 7, send = 0, data;

//button init
void PortF_Init(void){
	unsigned long volatile delay;
	SYSCTL_RCGCGPIO_R |= 0x20;            // 2) activate port F
  SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
	delay = SYSCTL_RCGC2_R;   
  GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock GPIO Port F
  GPIO_PORTF_CR_R |= 0x1F;         // allow changes to PF4,0
  GPIO_PORTF_DIR_R &= ~0x11;    // (c) PF4,0 input
	GPIO_PORTF_DIR_R |= 0x0E;    // (c) make PF3-1 out (built-in led)
	GPIO_PORTF_AFSEL_R &= ~0x1F;  //     disable alt funct on PF4,0
  GPIO_PORTF_PCTL_R &= ~0x000FFFFF; //  configure PF4-0 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x1F;          // disable analog functionality on PF4,0
  GPIO_PORTF_DEN_R |= 0x1F;             // enable digital I/O on PF4,0
  GPIO_PORTF_PUR_R |= 0x1F;     //     enable weak pull-up on PF4,0
  GPIO_PORTF_IS_R &= ~0x1F;     // (d) PF4-PF0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x1F;    //     PF4-PF0 is not both edges
  GPIO_PORTF_IEV_R &= ~0x1F;    //     PF4-PF0 falling edge event
  GPIO_PORTF_ICR_R = 0x1F;      // (e) clear flags 4,0
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4,0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000; // (g) priority 2
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
	
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

void DisableInterrupts(void);
void EnableInterrupts(void);

// Subroutine to wait 0.1 sec for 80MHz system clock
// Inputs: None
// Outputs: None
// Notes: ...
void Delay(void){
	unsigned long volatile time;
  time = 727240*100/91;  // 0.1sec
  while(time){
		time--;
  }
}

void GPIOPortF_Handler() // called on touch of SW2
{ 
	
  if(GPIO_PORTF_RIS_R&0x01) // SW2 button
	{  

		Delay();
		Delay();
		GPIO_PORTF_ICR_R = 0x01;
		qaFlag = (qaFlag + 1) % 8;

		switch(qaFlag)
  	{
		  	case 0:
					GPIO_PORTF_DATA_R = 0x02;  // LED is red;
				  break;
	  		case 1:
					GPIO_PORTF_DATA_R = 0x0A;  // LED is yellow;
				  break;
  			case 2:
					GPIO_PORTF_DATA_R = 0x08;  // LED is green;
				  break;
			  case 3:
					GPIO_PORTF_DATA_R = 0x04;  // LED is blue;
				  break;
		  	case 4:
					GPIO_PORTF_DATA_R = 0x0C;  // LED is light blue;
				  break;
				case 5:
					GPIO_PORTF_DATA_R = 0x0E;  // LED is white;
				  break;
			  case 6:
					GPIO_PORTF_DATA_R = 0x06;  // LED is purple;
				  break;
		  	case 7:
					GPIO_PORTF_DATA_R = 0x00;  // LED is dark;
				  break;
	  		default:;
				
		}
		
	}
	
	if(GPIO_PORTF_RIS_R&0x10) // SW1 button
	{  

		Delay();
		Delay();
		GPIO_PORTF_ICR_R = 0x10;
		UART_OutChar(GPIO_PORTF_DATA_R);
		
  }
	
}

//debug code
int main(void){
	
	DisableInterrupts();
  PLL_Init();
  UART_Init();              // initialize UART
	PortF_Init();
	EnableInterrupts();
	
  while(1)
	{
		
		data = UART_InChar();
		GPIO_PORTF_DATA_R = data;

  }
}
