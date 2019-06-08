// SysTick.c
// Runs on TM4C123
// This is an example program to show music programming on the LaunchPad.
// You can run this program without modification.
// The program will play three songs: Happy Birthday, 
// Mary Had a Little Lamb, and Twinkle Twinkle Little Stars,
// in that order and repeat forever.
// Hardware connection: 
// Positive logic Speaker/Headset is connected to PA2.
// Authors: Min He
// Date: August 28, 2018

#include "tm4c123gh6pm.h"
#define SPEAKER (*((volatile unsigned long *)0x40005010))
#define Speaker_Toggle_Mask     0x00000004  // mask used to toggle the speaker output
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode

static unsigned long c_note;

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
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

// Interrupt service routine, 
// frequency is determined by current tone being played
void SysTick_Handler(void){
	NVIC_ST_CTRL_R = 0;
	SPEAKER ^= Speaker_Toggle_Mask;
	NVIC_ST_RELOAD_R = c_note;// reload current initial value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
	NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+
	                 NVIC_ST_CTRL_CLK_SRC+NVIC_ST_CTRL_INTEN;
}

