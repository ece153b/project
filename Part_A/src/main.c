/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Project
 */

#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "UART.h"
#include "motor.h"
#include <stdio.h>

void Init_USARTx(int x) {
	if(x == 1) {
		UART1_Init();
		UART1_GPIO_Init();
		USART_Init(USART1);
	} else if(x == 2) {
		UART2_Init();
		UART2_GPIO_Init();
		USART_Init(USART2);
	} else {
		// Do nothing...
	}
}

int main(void) {
	char ch;
	// Switch System Clock = 80 MHz
	System_Clock_Init(); 
	Motor_Init();
	SysTick_Init();
	/*
	UART2_GPIO_Init();
	UART2_Init();
	USART_Init(USART2);//TODO
	*/
	Init_USARTx(1);
	
	printf("Program Starts.\r\n");
	setDire(1);
	char rxByte;
	while(1)
	{
		printf("Enter 0 to turn counterclockwise or 1 to turn clockwise: "); 
		scanf("%c", &rxByte); 
		if(rxByte == '0')
		{
			setDire(0);
		}
		else if(rxByte == '1')
		{
			setDire(1);
		}
		else
		{
			printf("%c is an invalid input, try again\n", rxByte); 
		}
		Init_USARTx(1);
	}
}


