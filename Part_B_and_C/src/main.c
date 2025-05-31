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
#include "LED.h"
#include "DMA.h"
#include "UART.h"
#include "motor.h"
#include "SPI.h"
#include "I2C.h"
#include "accelerometer.h"
#include <stdio.h>

static char buffer[IO_SIZE];

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

void UART_onInput(char* inputs, uint32_t size) {
	//TODO
}

int main(void) {
	// Switch System Clock = 80 MHz
	/*
	System_Clock_Init(); 
	Motor_Init();
	SysTick_Init();
	UART2_Init();
	LED_Init();	
	SPI1_GPIO_Init();
	SPI1_Init();
	initAcc();
	I2C_GPIO_Init();
	I2C_Initialization();
	*/
	System_Clock_Init(); 
	Motor_Init();
	SysTick_Init();
	//Init_USARTx(2);
	
	
	sprintf(buffer, "Program Starts.\r\n");
	setDire(1);

	//UART_print(buffer);
	while(1) {
		//TODO
		/*
		LED_Toggle();
		delay(1000);
		*/
	}
}


