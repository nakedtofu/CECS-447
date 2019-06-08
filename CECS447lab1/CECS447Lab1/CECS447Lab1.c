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

unsigned char Index = 0, menu = 0, change_flag = 0, power = 0;

const unsigned char TriangleWave[64] = {1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,
                                        31,33,35,37,39,41,43,45,47,49,51,53,55,57,59,
                                        61,63,63,63,61,59,57,55,53,51,49,47,45,43,41,
                                        39,37,35,33,31,29,27,25,23,21,19,17,15,13,11,
                                        9,7,5,3};

const unsigned char SquareWave[64] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                      63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
                                      63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63};

const unsigned char SineWave[64] = {32,35,38,41,44,47,49,52,54,56,58,59,61,62,62,
	                                  63,63,63,62,62,61,59,58,56,54,52,49,47,44,41,
	                                  38,35,32,29,26,23,20,17,15,12,10,8,6,5,3,2,2,
                                    1,1,1,2,2,3,5,6,8,10,12,15,17,20,23,26,29};

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
		menu = (menu + 1) % 3;
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
	
	if(power)
	{
	
	  switch(menu)
	  {

		  case 0:
			  DAC_Out(SineWave[Index]);
			  break;

		  case 1:
			  DAC_Out(SquareWave[Index]);
			  break;

		  case 2:
			  DAC_Out(TriangleWave[Index]);
			  break;

		  default:
			  DAC_Out(SineWave[Index]);
		  	break;

	  }
	
  }
	
}

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

/*
// need to generate a 100 Hz sine wave
// table size is 16, so need 100Hz*16=1.6 kHz interrupt
// bus is 80MHz, so SysTick period is 80000kHz/1.6kHz = 50000
int main(void){ unsigned long i,input,previous;     
  DisableInterrupts();    
  PLL_Init();          // bus clock at 80 MHz
  Switch_Init();       // Port F is onboard switches, LEDs, profiling
  Sound_Init(50000);   // initialize SysTick timer, 100 Hz
// Initial testing, law of superposition
  DAC_Out(1);
  DAC_Out(2);
  DAC_Out(4);
// static testing, single step and record Vout
  for(i=0;i<8;i++){
    DAC_Out(i);
  }
// dynamic testing, push SW1 for sound
  previous = Switch_In()&0x10;
  while(1){ 
    input = Switch_In()&0x10; // bit 4 means SW1 pressed
    if(input&&(previous==0)){ // just pressed     
      EnableInterrupts();
      Sound_Init(11364);      // Play 440 Hz wave
    }
    if(previous&&(input==0)){ // just released     
      DisableInterrupts();    // stop sound
    }
    previous = input; 
    Delay10ms();  // remove switch bounce    
  }  
}
*/

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
		
	}

}
