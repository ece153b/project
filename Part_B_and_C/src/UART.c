/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Project
 */


#include "UART.h"
#include "DMA.h"
#include <stdio.h>
#include <string.h>
static volatile DMA_Channel_TypeDef * tx;
static volatile char inputs[IO_SIZE];
static volatile uint8_t data_t_0[IO_SIZE];
static volatile uint8_t data_t_1[IO_SIZE];
static volatile uint8_t input_size = 0;
static volatile uint8_t pending_size = 0;
static volatile uint8_t * active = data_t_0;
static volatile uint8_t * pending = data_t_1;

static int inputIndex = 0;

#define SEL_0 1
#define BUF_0_EMPTY 2
#define BUF_1_EMPTY 4
#define BUF_0_PENDING 8
#define BUF_1_PENDING 16

void transfer_data(char ch);
void on_complete_transfer(void);

void UART1_Init(void) {
	//Enable clock in peripheral clock register
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	//Select the system clock as USART1 clock source in peripheral independent clock config register
	RCC->CCIPR &= ~(RCC_CCIPR_USART1SEL);
	RCC->CCIPR |= RCC_CCIPR_USART1SEL_0;
}

void UART2_Init(void) {
	//Enable clock in peripheral clock register
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	//Select the system clock as USART2 clock source in peripheral independent clock config register
	RCC->CCIPR &= ~(RCC_CCIPR_USART2SEL);
	RCC->CCIPR |= RCC_CCIPR_USART2SEL_0;
	
	//Set the data source to data buffer provided in CRC.h
	DMA1_Channel6->CMAR = (uint32_t)active; //CHANGED THIS
	//Set the data destination to the data register of the CRC block
	DMA1_Channel6->CPAR = (uint32_t)&(USART1->TDR); //CHANGED THIS
}

void UART1_GPIO_Init(void) {
	//Enable Clock for PA 6 (tx/transmitter) and Pa 7 (RX/reciever)
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	//Clear mode reg
	GPIOB->MODER &= ~(GPIO_MODER_MODE6);
	GPIOB->MODER |= GPIO_MODER_MODE6_1; //double check
	GPIOB->MODER &= ~(GPIO_MODER_MODE7);
	GPIOB->MODER |= GPIO_MODER_MODE7_1; //double check
	
	//Choose alternate function
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6; 
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6_3); //Set AF for PB6 to AF7 (0111)
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7; 
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL7_3); //Set AF for PB7 to AF7 (0111)
	
	//Operate at high speed
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED6);
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6_1;
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED7;
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED7_1;
	//Push-pull output type
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT6);
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT7);
	//Configure both GPIO pins to use pull-up registors for I/O
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6);
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0;
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD7);
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD7_0;
}

void UART2_GPIO_Init(void) {
	//TODO//Enable Clock for PA 2 (tx/transmitter) and Pa 3 (RX/reciever)
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	//Clear mode reg
	GPIOA->MODER &= ~(GPIO_MODER_MODE2);
	GPIOA->MODER |= GPIO_MODER_MODE2_1; //double check
	GPIOA->MODER &= ~(GPIO_MODER_MODE3);
	GPIOA->MODER |= GPIO_MODER_MODE3_1; //double check
	//Choose alternate function
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2; 
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_3); //Set AF for PA2 to AF7 (0111)
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3; 
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL3_3); //Set AF for PA3 to AF7 (0111)
	
	
		
	//Operate at high speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED2;
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED3;
	//Push-pull output type
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT3);
	//Configure both GPIO pins to use pull-up registors for I/O
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD2_0;
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD3);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD3_0;
}

void USART_Init(USART_TypeDef * USARTx) {
	// Disable USART before configuring settings
	USARTx->CR1 &= ~USART_CR1_UE;
	
	// Set Communication Parameters
	USARTx->CR1 &= ~(USART_CR1_M);     // 00 -> 8 Data Bits
	USARTx->CR1 &= ~(USART_CR1_OVER8); // 0 -> Oversampling by 16
	USARTx->CR2 &= ~(USART_CR2_STOP);  // 00 -> 1 Stop Bit
	
	// Set Baud Rate
	// f_CLK = 80 MHz, Baud Rate = 9600 = 80 MHz / DIV -> DIV = 8333 = 0x208D
	USARTx->BRR = 0x208D;
	
	// Enable Transmitter/Receiver
	USARTx->CR1 |= USART_CR1_TE | USART_CR1_RE;
	
	//Enable needed interrupts (ADDED THIS)
	USART2->CR1 |= USART_CR1_TCIE; 	//Transmission Complete
	USART2->CR1 |= USART_CR1_RXNEIE;	//Data Recieved
	if(USARTx == USART1)
	{
		NVIC_EnableIRQ(USART1_IRQn); 
	}
	else if(USARTx == USART2)
	{
		NVIC_EnableIRQ(USART2_IRQn); 
	}
	
	// Enable USART
	USARTx->CR1 |= USART_CR1_UE;
	
	USARTx->CR3 |= USART_CR3_DMAT; 
}

/**
 * This function accepts a string that should be sent through UART
*/
void UART_print(char* data) {
	//TODO

	
	//Check DMA status. If DMA is ready, send data
	if(DMA1_Channel7->CNDTR == 0) //Not printing
	{
		uint32_t data_size = sprintf((char*)active, "%s", data); //Transfer char array to buffer

		DMA1_Channel7->CMAR = (uint32_t)active;
		DMA1_Channel7->CNDTR = data_size;		
	}
	else 	//If DMA is not ready, put the data aside (just leave it in pending)
	{
				pending_size += sprintf((char*)pending[pending_size], "%s", data);
	}
}

/**
 * This function should be invoked when a character is accepted through UART
*/
void transfer_data(char ch) {
	//TODO
	// Append character to input buffer.
	if (inputIndex < IO_SIZE - 1) {
		inputs[inputIndex] = ch;
		inputIndex++;
	}
	// If the character is end-of-line, invoke UART_onInput
	if(ch == '\n')
	{
		UART_onInput((char *)inputs, IO_SIZE);
		inputIndex  = 0;
	}
}

/**
 * This function should be invoked when DMA transaction is completed
*/
void on_complete_transfer(void) {
	//TODO
	// If there are pending data to send, switch active and pending buffer, and send data
	uint32_t* temp = active;
	active = pending; 
	pending = temp; 
	
	DMA1_Channel7->CMAR = (uint32_t)active;
	DMA1_Channel7->CNDTR = pending_size;
	pending_size = 0;
	
}

void USART1_IRQHandler(void){
	//TODO
	// When receive a character, invoke transfer_data
	
	// When complete sending data, invoke on_complete_transfer
}

void USART2_IRQHandler(void){
	//TODO
	// When receive a character, invoke transfer_data
	if(USART2->ISR & USART_ISR_RXNE)
	{
		transfer_data((char)USART2->RDR); 
		USART2->ISR &= ~(USART_ISR_RXNE);
	}
	else if(USART2->ISR & USART_ISR_TC) // When complete sending data, invoke on_complete_transfer
	{
		on_complete_transfer(); 
		USART2->ISR &= ~(USART_ISR_TC);

	}
}
