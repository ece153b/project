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
double x, y, z; 
uint8_t SecondaryAddress;
uint8_t Data_Send;
uint8_t Data_Receive;

void Init_USARTx(int x) {
	if(x == 1) {
		UART1_Init();
		UART1_GPIO_Init();
		USART_Init(USART1);
		NVIC_EnableIRQ(USART1_IRQn); 
		NVIC_SetPriority(USART1_IRQn, 0);
	} else if(x == 2) {
		UART2_Init();
		UART2_GPIO_Init();
		USART_Init(USART2);
		NVIC_EnableIRQ(USART2_IRQn); 
		NVIC_SetPriority(USART2_IRQn, 0);
	} else {
		// Do nothing...
	}
}

void UART_onInput(char* inputs, uint32_t size) {
	for(int i = 0; i < size; i++)
	{
		if(inputs[i] == '0')
		{
			setDire(0); 
			UART_print("Turning counterclockwise\n"); 
		}
		else if(inputs[i] == '1')
		{
			setDire(1); 
			UART_print("Turning clockwise\n"); 

		}
		else if(inputs[i] == '\n')
		{
			//Do nothing
		}
		else
		{
			UART_print(&inputs[i]); 
			UART_print(" is not a valid input\n"); 
		}
	}
}

int main(void) {
	// Switch System Clock = 80 MHz
	
	System_Clock_Init(); 
	Motor_Init();
	SysTick_Init();
	//UART2_Init();
	//LED_Init();	
	SPI1_GPIO_Init();
	SPI1_Init();
	initAcc();
	I2C_GPIO_Init();
	I2C_Initialization();
	
	DMA_Init_UARTx(DMA1_Channel4, USART1);
	Init_USARTx(1);

	sprintf(buffer, "This program starts\r\n");
	UART_print(buffer); 
	setDire(1);
	while(1) {
		//TODO
		/*
		LED_Toggle();
		delay(1000);
		*/
		SecondaryAddress = 0b1001000 << 1; // STUB - Fill in correct address 
		Data_Send = 0; 
		
		readValues(&x, &y, &z); 
		I2C_SendData(I2C1, SecondaryAddress, &Data_Send, 1); 
		I2C_ReceiveData(I2C1, SecondaryAddress, &Data_Receive, 1); 
		
		//sprintf(buffer, "Acceleration: %.2f, %.2f, %.2f\r\n", x, y, z); 
		sprintf(buffer, "Acceleration: %.2f, %.2f, %.2f\r\n, Temperature: %d\n", x, y, z, Data_Receive); 
		UART_print(buffer); 
		delay(2000); 
	}
}


