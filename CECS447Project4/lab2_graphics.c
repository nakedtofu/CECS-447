// Lab1.c
// Runs on TM4C123
// Uses ST7735.c LCD.
// Jonathan Valvano
// August 5, 2015
// Possible main program to test the lab
// Feel free to edit this to match your specifications

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "ST7735.h"
#include "SysTick.h"
#include "PLL.h"
#include "walking.h"
#include "tm4c123gh6pm.h"
#include "texas.h"

int main(void){
  int32_t x = 40;// ht = 0;	
//	int32_t l = 15, h = 15;
	
	SysTick_Init();
  PLL_Init(12);
  ST7735_InitR(INITR_REDTAB);
	
	// DRAW THE WALL
	ST7735_FillScreen(ST7735_GREEN);
	
	// DRAW THE BORDER
	ST7735_FillRect(0, 0,128, 5, ST7735_RED);
	ST7735_FillRect(0, 155,128, 5, ST7735_RED);
	ST7735_FillRect(0, 0,5, 160, ST7735_RED);
	ST7735_FillRect(123, 0,5, 160, ST7735_RED);
	  
	// DRAW THE CORNER
	ST7735_DrawLine(0, 0, 20, 25, ST7735_RED);
	ST7735_DrawLine(0, 160, 20, 135, ST7735_RED);
	ST7735_DrawLine(108, 25, 128, 0, ST7735_RED);
	ST7735_DrawLine(108, 135, 128, 160, ST7735_RED);
	
	// DRAW THE FLOOR
	ST7735_FillRect(20, 25, 88, 110, ST7735_RED);
	ST7735_FillRect(21, 26, 86, 108, ST7735_CYAN);
	
	// DRAW THE TARGET CIRCLE
	ST7735_FillCircle(64, 80, 30,ST7735_RED);
	ST7735_FillCircle(64, 80, 29,ST7735_CYAN);
	
	// DRAW THE TARGET CROSS
	ST7735_DrawLine(64, 40, 64, 120, ST7735_RED);
	ST7735_DrawLine(24, 80, 104, 80, ST7735_RED);
	
	// DRAW THE THREE CHARACTERS
	ST7735_DrawChar(24, 30, 'A', ST7735_YELLOW, ST7735_CYAN, 1);
	ST7735_DrawChar(34, 30, 'B', ST7735_WHITE, ST7735_CYAN, 2);
	ST7735_DrawChar(84, 30, 'C', ST7735_BLUE, ST7735_CYAN, 3);
	
  while(1){
	
	// WHILE PUCK OVERLAPS WITH CROSS, CHANGE COLOR
	if(x>62 && x<66)
		ST7735_FillCircle(x, 80, 5,ST7735_YELLOW);
	else
		// DRAW THE HOCKEY PUCK		
	  ST7735_FillCircle(x, 80, 5,ST7735_BLACK);
		
  SysTick_Wait(1600000);
	
	// ERASE PUCK
  ST7735_FillCircle(x, 80, 5,ST7735_CYAN);
	
	// REDRAW THE TARGET CROSS
	ST7735_DrawLine(64, 40, 64, 120, ST7735_RED);
	ST7735_DrawLine(24, 80, 104, 80, ST7735_RED);
 
	// MOVE THE COORDINATES OF THE PUCK
	x++;
 
	//Reset parameters
	  if(x>88){
			x = 40;
    }
	}
  
} 


// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}
