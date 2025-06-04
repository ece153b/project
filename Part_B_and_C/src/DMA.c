/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Project
 */
 
#include "DMA.h"
#include "SysTimer.h"
#include "UART.h"

void DMA_Init_UARTx(DMA_Channel_TypeDef * tx, USART_TypeDef * uart) {
	//Enable the clock for DMA
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN; 
	//Wait 20us for DMA to finish setting up
	delay(20);  	
	//Disable Channel 6
	tx->CCR &= ~(DMA_CCR_EN);
	//Disable Memory-to-Memory mode
	tx->CCR &= ~(DMA_CCR_MEM2MEM); //CHANGED THIS
	//Set channel priority to high (10)
	tx->CCR &= ~(DMA_CCR_PL); 
	tx->CCR |= DMA_CCR_PL_1;
	//Set peripheral size to 8 bit (00)
	tx->CCR &= ~(DMA_CCR_PSIZE); //CHANGED THIS
	//Set memory size to 8-bit (00)
	tx->CCR &= ~(DMA_CCR_MSIZE); //CHANGED THIS
	//Disable peripheral increment mode
	tx->CCR &= ~(DMA_CCR_PINC); 
	//Enable memory increment mode
	tx->CCR |= DMA_CCR_MINC; 
	//Disable circular mode
	tx->CCR &= ~(DMA_CCR_CIRC); 
	//Set data transfer direction to Peripheral-to-Memory
	tx->CCR &= DMA_CCR_DIR; //CHANGED THIS


	//Disable half transfer interrupt
	tx->CCR &= ~(DMA_CCR_HTIE); 
	//Disable transfer error interrupt
	tx->CCR &= ~(DMA_CCR_TEIE); 
	//Enable transfer complete interrupt
	tx->CCR |= DMA_CCR_TCIE;
	//Set interrupt priority to 0 in NVIC
	NVIC_SetPriority(DMA1_Channel7_IRQn, 0); //Make sure this is the right channel
	//Enable interrupt in NVIC
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
	
	//Select USART2 on channel 7
	DMA1_CSELR->CSELR &= ~(DMA_CSELR_C7S);
	DMA1_CSELR->CSELR |= 1UL<<25; //double check this 
	
	tx->CCR |= DMA_CCR_EN;
}
