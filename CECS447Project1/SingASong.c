// This is an example program for music programming.
// You will run this program without modification.
// The program will repeatedly play a specified song.

// The program will continuously play one of the song  
// defined in mysong on a speaker connted to pin PA2.
// Authors: Min He
// Date: August 26, 2018

// Header files 
#include "tm4c123gh6pm.h"
#include "SysTick1.h"
#include "PLL.h"

// 2. Declarations Section

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

/*
{152672,136054,121212,114613,102041,90909,80972, // C4 Major notes
 76445,68104,60675,57269,51021,45455,40495, // C5 Major notes
 38223,34052,30337,28635,25510,22727,20248};// C6 Major notes
*/

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

NTyp mysong2[] =
// score table for Mary Had A Little Lamb
{E5, 4, D5, 4, C5, 4, D5, 4, E5, 4, E5, 4, E5, 8, 
 D5, 4, D5, 4, D5, 8, E5, 4, G5, 4, G5, 8,
 E5, 4, D5, 4, C5, 4, D5, 4, E5, 4, E5, 4, E5, 8, 
 D5, 4, D5, 4, E5, 4, D5, 4, C5, 8, 0, 0 };

NTyp mysong3[] =
 // score table for Twinkle Twinkle Little Stars
{C5,4,C5,4,G5,4,G5,4,A5,4,A5,4,G5,8,F5,4,F5,4,E5,4,E5,4,D5,4,D5,4,C5,8, 
 G5,4,G5,4,F5,4,F5,4,E5,4,E5,4,D5,8,G5,4,G5,4,F5,4,F5,4,E5,4,E5,4,D5,8, 
 C5,4,C5,4,G5,4,G5,4,A5,4,A5,4,G5,8,F5,4,F5,4,E5,4,E5,4,D5,4,D5,4,C5,8,0,0};

unsigned char track_number = 0;
unsigned char change_flag = 0;
unsigned char power = 0;
	
	// Function Prototypes
void Speaker_Init(void);
void Delay(void);
void EnableInterrupts(void);
void play_a_song(NTyp scoretab[]);
void GPIOPortF_Handler(void);
	
//button init
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

// 3. Subroutines Section
// MAIN: Mandatory for a C Program to be executable
int main(void){
  Speaker_Init();
  SysTick_Init();
	PLL_Init();
	PortF_Init();
	
  EnableInterrupts();  // SysTick uses interrupts
	
  while(1){
		
		switch(track_number)
			{
			
				case 0:
					play_a_song(mysong1);
					break;
				case 1:
					play_a_song(mysong2);
					break;
				case 2:
					play_a_song(mysong3);
					break;
				default:
					play_a_song(mysong1);
					break;
			
			}
		
  }

}

void play_a_song(NTyp scoretab[])
{
	unsigned char i=0, j;

	while (scoretab[i].delay) {
		
		if((power == 0) || (change_flag == 1))
		{
			change_flag = 0;
			break;
		}
		
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
// Make PA2 an output, enable digital I/O, ensure alt. functions off

//port F handler
void GPIOPortF_Handler() // called on touch of SW2
{ 
	
  if(GPIO_PORTF_RIS_R&0x01) // SW2 button
	{  

		Delay();
		Delay();
		GPIO_PORTF_ICR_R = 0x01;
		track_number = (track_number + 1) % 3;
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

void Speaker_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x02;           // 1) activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
                                    // 2) no need to unlock PA2
  GPIO_PORTB_PCTL_R &= ~0x00000F00; // 3) regular GPIO
  GPIO_PORTB_AMSEL_R &= ~0x04;      // 4) disable analog function on PA2
  GPIO_PORTB_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTB_AFSEL_R &= ~0x04;      // 6) regular port function
  GPIO_PORTB_DEN_R |= 0x04;         // 7) enable digital port
}
