// CECS447Lab1.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a single 440Hz sine wave
// Daniel Valvano, Jonathan Valvano
// September 15, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
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
// Port B bits 2-0 have the 3-bit DAC
// Port F is onboard LaunchPad switches and LED
// Port F bit 4 is negative logic switch to play sound, SW1
// SysTick ISR: PF3 ISR heartbeat


#include "./tm4c123gh6pm.h"
#include "PLL.h"

unsigned char Index = 0, menu = 0, change_flag = 0, power = 0, t =0;

const unsigned char bits[64] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
	                                      21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,
	                                      38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,
                                        55,56,57,58,59,60,61,62,63};

void DAC_Init(void){unsigned long volatile delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // activate port B
  delay = SYSCTL_RCGC2_R;    // allow time to finish activating
  GPIO_PORTB_AMSEL_R &= ~0x3F;      // no analog 
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // regular function
  GPIO_PORTB_DIR_R |= 0x3F;      // make PB5-0 out
  GPIO_PORTB_AFSEL_R &= ~0x3F;   // disable alt funct on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;      // enable digital I/O on PB5-0
}

void Sound_Init(unsigned long period){
  DAC_Init();          // Port B is DAC
  Index = 0;
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1      
  NVIC_ST_CTRL_R = 0x0007;  // enable SysTick with core clock and interrupts
}

void Delay(void){
	unsigned long volatile time;
  time = 727240*100/91;  // 0.1sec
  while(time){
		time--;
  }
}

void PortF_Init(void){
	
  volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x20;            // 2) activate port F
  SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
	delay = SYSCTL_RCGC2_R;
  GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock GPIO Port F
  GPIO_PORTF_CR_R |= 0x11;         // allow changes to PF0,4
  GPIO_PORTF_DIR_R &= ~0x11;    // (c) PF0,4 input
	GPIO_PORTF_PCTL_R &= ~0x000F000F;     // clear PF0,4
  GPIO_PORTF_AMSEL_R &= ~0x11;          // disable analog functionality on PF0,4
  GPIO_PORTF_DEN_R |= 0x11;             // enable digital I/O on PF0,4
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF0,4
  GPIO_PORTF_IS_R &= ~0x11;     // (d) PF0,4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    //     PF0,4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;    //     PF0,4 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flags 0,4
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF0,4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000; // (g) priority 2
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
	
}
	
//port F handler
void GPIOPortF_Handler() // called on touch of SW2
{ 
	
  if(GPIO_PORTF_RIS_R&0x01) // SW2 button
	{  

		Delay();
		Delay();
		GPIO_PORTF_ICR_R = 0x01;
		t = (t + 1) % 64;
		change_flag = 1;
		
	}
	
	if(GPIO_PORTF_RIS_R&0x10) // SW1 button
	{  

		Delay();
		Delay();
		GPIO_PORTF_ICR_R = 0x10;
		power = (power + 1) % 2;
		
	}
	
}

void DAC_Out(unsigned long data)
{
  GPIO_PORTB_DATA_R = data;
}

void SysTick_Handler(void)
{

  //GPIO_PORTF_DATA_R ^= 0x08;     // toggle PF3, debugging
  Index = (Index+1)&0x3F;  

	if( (change_flag == 1) || (power == 0) )
	{
		
		change_flag = 0;
		Index = 0;
		
	}
	
}

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

int main(void)
{ 
	//unsigned long i,input,previous;     
  DisableInterrupts();    
  PLL_Init();          // bus clock at 80 MHz
  PortF_Init();       // Port F is onboard switches
  Sound_Init(11364);   // initialize SysTick timer, 440 Hz
	EnableInterrupts();

  while(1)
	{ 
		
		DAC_Out(bits[t]);
		
	}

}
