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
static int8_t doorStatus = 0; //0-closing, 1-opening, 2-closed, 3-opened
int tempState = 0; //0-cold, 1-hot
int tempPrev = 0;
int messageDelay = 0; 
int commandDelay = 0;

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
		else if(inputs[0] == 'o' && inputs[1] == 'p' && inputs[2] == 'e' && inputs[3] == 'n')
		{
			doorStatus = 1; 
			UART_print("Opening door\n");
			commandDelay = 0;
			break;
		}
		else if(inputs[0] == 'c' && inputs[1] == 'l' && inputs[2] == 'o' && inputs[3] == 's' && inputs[4] == 'e')
		{
			doorStatus = 0; 
			UART_print("Closing door\n"); 
			commandDelay = 1; 
			break;
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
		

		
		if(commandDelay < 20)
		{
			commandDelay++;
		}
		else
		{
			//Temperature cross threshold value 25C, open garage door
			if (Data_Receive > 24 && doorStatus != 3 && doorStatus != 1) {
				sprintf(buffer, "Temperature too high. Door opening.\n");
				UART_print(buffer);
				doorStatus = 1;			
				tempState = 1; 
			}
			else if (Data_Receive < 22 && doorStatus != 2 && doorStatus != 0) { 		//When temp drop below 15C, close garage door
				sprintf(buffer, "Temperature too low. Door closing. \n");
				UART_print(buffer);
				doorStatus = 0; 
				tempState = 0;
			}
			
			if(tempState == 0 && (doorStatus == 1 || doorStatus == 3))
			{
				doorStatus = 0; 
			}
			
			if(tempState == 1 && (doorStatus == 0 || doorStatus == 2))
			{
				doorStatus = 1; 
			}
		}

			if(x < -1.05 && doorStatus != 1 && doorStatus != 2)
			{
				//sprintf(buffer, "Closed, halting\n");
				UART_print(buffer);
				doorStatus = 2;
			}
			else if(x > -0.6 && doorStatus != 0 && doorStatus != 3)
			{
				//sprintf(buffer, "Open, halting\n");
				UART_print(buffer);
				doorStatus = 3; 
			}
		
		setDire(doorStatus); 
		if(messageDelay == 5)
		{
			messageDelay = 0; 
			//sprintf(buffer, "Acceleration: %.2f, %.2f, %.2f\r\n, Temperature: %d\n", x, y, z, Data_Receive); 
			UART_print(buffer); 
			if(Data_Receive != tempPrev)
			{
				sprintf(buffer, "Temperature: %d\n", Data_Receive); 
				UART_print(buffer);
			}
			tempPrev = Data_Receive;
		}
		else
		{
			messageDelay++; 
		}
		//sprintf(buffer, "Acceleration: %.2f, %.2f, %.2f\r\n", x, y, z); 

		delay(2000); 
	}
}


