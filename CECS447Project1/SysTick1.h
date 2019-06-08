// SysTick.h
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

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void);
void SysTick_start(void);
void SysTick_stop(void);
void SysTick_Set_Current_Note(unsigned long n_value);
