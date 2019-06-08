// main.c
// Runs on LM4F120/TM4C123
// Test main for Lab 11
// January 15, 2016

// this connection occurs in the USB debugging cable
// U0Rx (PA0) connected to serial port on PC
// U0Tx (PA1) connected to serial port on PC
// Ground connected ground in the USB cable

#include "UART.h"

//char Strings[20];

void EnableInterrupts(void);  // Enable interrupts
unsigned char RunLab1[15] = "Running Lab 11";
unsigned char input[9] = "\n\rInput:";
unsigned char OutUDec[17] = " UART_OutUDec = ";
unsigned char OutDist[23] = ",  UART_OutDistance ~ ";
// do not edit this main
// your job is to implement the UART_OutUDec UART_OutDistance functions 
int main(void){ unsigned int n;
  UART_Init();              // initialize UART
  UART_OutString(RunLab1);
  while(1){
    UART_OutString(input);
    n = UART_InUDec();
		//UART_InString(Strings, 20);
    UART_OutString(OutUDec);
    UART_OutUDec(n);     // your function
    UART_OutString(OutDist);
    UART_OutDistance(n); // your function
  }
}

