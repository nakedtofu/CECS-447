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

#define C1   2389   // 2093 Hz
#define B1   2531   // 1975.5 Hz
#define BF1  2681   // 1864.7 Hz
#define A1   2841   // 1760 Hz
#define AF1  3010   // 1661.2 Hz
#define G1   3189   // 1568 Hz
#define GF1  3378   // 1480 Hz
#define F1   3579   // 1396.9 Hz
#define E1   3792   // 1318.5 Hz
#define EF1  4018   // 1244.5 Hz
#define D1   4257   // 1174.7 Hz
#define DF1  4510   // 1108.7 Hz
#define C    4778   // 1046.5 Hz
#define B    5062   // 987.8 Hz
#define BF   5363   // 932.3 Hz
#define A    5682   // 880 Hz
#define AF   6020   // 830.6 Hz
#define G    6378   // 784 Hz
#define GF  6757   // 740 Hz
#define F   7159   // 698.5 Hz
#define E   7584   // 659.3 Hz
#define EF  8035   // 622.3 Hz
#define D   8513   // 587.3 Hz
#define DF  9019   // 554.4 Hz
#define C0  9556   // 523.3 Hz
#define B0  10124   // 493.9 Hz
#define BF0 10726   // 466.2 Hz
#define A0  11364   // 440 Hz
#define AF0 12039   // 415.3 Hz
#define G0  12755   // 392 Hz
#define GF0 13514   // 370 Hz
#define F0  14317   // 349.2 Hz
#define E0  15169   // 329.6 Hz
#define EF0 16071   // 311.1 Hz
#define D0  17026   // 293.7 Hz
#define DF0 18039   // 277.2 Hz
#define C7  19111   // 261.6 Hz
#define B7  20248   // 246.9 Hz
#define BF7 21452   // 233.1 Hz
#define A7  22727   // 220 Hz
#define AF7 24079   // 207.7 Hz
#define G7  25511   // 196 Hz
#define GF7 27027   // 185 Hz
#define F7  28635   // 174.6 Hz
#define E7  30337   // 164.8 Hz
#define EF7 32141   // 155.6 Hz
#define D7  34052   // 146.8 Hz
#define DF7 36077   // 138.6 Hz

#define SPEAKER (*((volatile unsigned long *)0x40005010))
#define Speaker_Toggle_Mask     0x00000004  // mask used to toggle the speaker output
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode

// indexes for notes used in music scores
#define C5 0+7
#define D5 1+7
#define E5 2+7
#define F5 3+7
#define G5 4+7
#define A5 5+7
#define B5 6+7
#define C6 0+2*7
#define D6 1+2*7
#define E6 2+2*7
#define F6 3+2*7
#define G6 4+2*7

// define music data structure 
struct Note {
  unsigned char note_index;
  unsigned char delay;
};
typedef const struct Note NTyp;

// initail values for piano major tones.
// Assume SysTick clock frequency is 80MHz.
const unsigned long Note_Tab[] =
// Notes:  C, D, E, F, G, A, B
// Offset: 0, 1, 2, 3, 4, 5, 6

{95420, 85034, 75758, 71633, 63776, 56818, 50607, //C4
47778, 42566, 37922, 35793, 31888, 28409, 25310, //C5
23889, 21283, 18961, 17897, 15944, 14205, 12655, //C6
11945, 10641, 9480, 8948, 7972, 7102, 6327}; //C7

// doe ray mi fa so la ti 
// C   D   E  F  G  A  B
NTyp mysong1[] =
 
// score table for Happy Birthday
{//so   so   la   so   doe' ti
   G5,2,G5,2,A5,4,G5,4,C6,4,B5,4,
// pause so   so   la   so   ray' doe'
   0,4,  G5,2,G5,2,A5,4,G5,4,D6,4,C6,4,
// pause so   so   so'  mi'  doe' ti   la
   0,4,  G5,2,G5,2,G6,4,E6,4,C6,4,B5,4,A5,8, 
// pause fa'  fa'   mi'  doe' ray' doe' stop
	 0,4,  F6,2,F6,2, E6,4,C6,4,D6,4,C6,8,0,0};

unsigned char Index = 0, key = 0, change_flag = 0, power = 0, hold = 0, menu = 0;
static unsigned long c_note;

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

// G,A,B key init
void PortC_Init(void){
	
  volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x04;            // 2) activate port C
  SYSCTL_RCGC2_R |= 0x00000004; // (a) activate clock for port C
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTC_LOCK_R = 0x4C4F434B; // unlock GPIO Port C
  GPIO_PORTC_CR_R |= 0x70;         // allow changes to PC4-6
  GPIO_PORTC_DIR_R &= ~0x70;    // (c) PC4-6 input
	GPIO_PORTC_PCTL_R &= ~0x0FFF0000;     // clear PC4-6
  GPIO_PORTC_AMSEL_R &= ~0x70;          // disable analog functionality on PC4-6
  GPIO_PORTC_DEN_R |= 0x70;             // enable digital I/O on PC4-6
	
}

// C, D, E, F key init
void PortD_Init(void){
	
  volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x08;            // 2) activate port D
  SYSCTL_RCGC2_R |= 0x00000008; // (a) activate clock for port D
	delay = SYSCTL_RCGC2_R;
  GPIO_PORTD_LOCK_R = 0x4C4F434B; // unlock GPIO Port D
  GPIO_PORTD_CR_R |= 0x0F;         // allow changes to PD0-3
  GPIO_PORTD_DIR_R &= ~0x0F;    // (c) PD0-3 input
	GPIO_PORTD_PCTL_R &= ~0x0000FFFF;     // clear PD0-3
  GPIO_PORTD_AMSEL_R &= ~0x0F;          // disable analog functionality on PD0-3
  GPIO_PORTD_DEN_R |= 0x0F;             // enable digital I/O on PD0-3
	
}

void PortF_Init(void){
	
  volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x20;      // 2) activate port F
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
  NVIC_PRI7_R |= (NVIC_PRI7_R&0xFF00FFFF)|0x00400000; // (g) priority 2
  NVIC_EN0_R |= 0x40000000;      // (h) enable interrupt 30 in NVIC
	
}

//port F handler
void GPIOPortF_Handler() // called on touch of SW2
{ 
	
  if(GPIO_PORTF_RIS_R&0x01) // SW2 button
	{  

		Delay();
		Delay();
		GPIO_PORTF_ICR_R = 0x01;
		menu = (menu + 1) % 2;
		
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

	NVIC_ST_CTRL_R = 0;
	SPEAKER ^= Speaker_Toggle_Mask;
	NVIC_ST_RELOAD_R = c_note;// reload current initial value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
	NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+
	                 NVIC_ST_CTRL_CLK_SRC+NVIC_ST_CTRL_INTEN;
	
}

void SysTick_start(void)
{
  NVIC_ST_RELOAD_R = c_note;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
	NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+
	                 NVIC_ST_CTRL_CLK_SRC+NVIC_ST_CTRL_INTEN;
}
void SysTick_stop(void)
{
	  NVIC_ST_CTRL_R = 0;
}

void SysTick_Set_Current_Note(unsigned long n_value)
{
	  c_note = n_value-1;// reload value
}

void play_a_song(NTyp scoretab[])
{
	unsigned char i=0, j;

	while (scoretab[i].delay) {

		if((power == 0) || (menu == 1))
			break;
		
		if ( (!scoretab[i].note_index) ) 
			SysTick_stop(); // silence tone, turn off SysTick timer
		else {
			SysTick_Set_Current_Note(Note_Tab[scoretab[i].note_index]);
			SysTick_start();
		}
		
		// tempo control: 
		// play current note for duration 
		// specified in the music score table
		for (j=0;j<scoretab[i].delay;j++) 
			Delay();
		
		SysTick_stop();
		i++;  // move to the next note
	}

	// pause after each play
	for (j=0;j<5;j++) 
		Delay();
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
	PortC_Init();
	PortD_Init();
  Sound_Init(C7);   // initialize SysTick timer, 440 Hz
	EnableInterrupts();

  while(1)
	{ 
		
		if(menu && power)
		{
		
		if(GPIO_PORTD_DATA_R == 0x01)
		{
			Sound_Init(C7);   // initialize SysTick timer, 440 Hz
			while(GPIO_PORTD_DATA_R == 0x01)
			   DAC_Out(SineWave[Index]);
		}
		
		else if(GPIO_PORTD_DATA_R == 0x02)
		{
			Sound_Init(D0);   // initialize SysTick timer, 440 Hz
			while(GPIO_PORTD_DATA_R == 0x02)
			   DAC_Out(SineWave[Index]);
		}
		
		else if(GPIO_PORTD_DATA_R == 0x04)
		{
			Sound_Init(E0);   // initialize SysTick timer, 440 Hz
			while(GPIO_PORTD_DATA_R == 0x04)
			   DAC_Out(SineWave[Index]);
		}
		
		else if(GPIO_PORTD_DATA_R == 0x08)
		{
			Sound_Init(F0);   // initialize SysTick timer, 440 Hz
			while(GPIO_PORTD_DATA_R == 0x08)
			   DAC_Out(SineWave[Index]);
		}
		
		else if(GPIO_PORTC_DATA_R == 0x10)
		{
			Sound_Init(G0);   // initialize SysTick timer, 440 Hz
			while(GPIO_PORTC_DATA_R == 0x10)
			   DAC_Out(SineWave[Index]);
		}
		
		else if(GPIO_PORTC_DATA_R == 0x20)
		{
			Sound_Init(A0);   // initialize SysTick timer, 440 Hz
			while(GPIO_PORTC_DATA_R == 0x20)
			   DAC_Out(SineWave[Index]);
		}
		
		else if(GPIO_PORTC_DATA_R == 0x40)
		{
			Sound_Init(B0);   // initialize SysTick timer, 440 Hz
			while(GPIO_PORTC_DATA_R == 0x40)
			   DAC_Out(SineWave[Index]);
		}
		
		else
			Sound_Init(C7);

	}
	
	
	else if( (!menu) && power )
	{
		
		play_a_song(mysong1);
		
	}
	

}

}
