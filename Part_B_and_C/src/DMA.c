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
	DMA1_Channel6->CCR &= ~(DMA_CCR_EN);
	//Disable Memory-to-Memory mode
	DMA1_Channel6->CCR &= ~(DMA_CCR_MEM2MEM); //CHANGED THIS
	//Set channel priority to high (10)
	DMA1_Channel6->CCR &= ~(DMA_CCR_PL); 
	DMA1_Channel6->CCR |= DMA_CCR_PL_1;
	//Set peripheral size to 8 bit (00)
	DMA1_Channel6->CCR &= ~(DMA_CCR_PSIZE); //CHANGED THIS
	//Set memory size to 8-bit (00)
	DMA1_Channel6->CCR &= ~(DMA_CCR_MSIZE); //CHANGED THIS
	//Disable peripheral increment mode
	DMA1_Channel6->CCR &= ~(DMA_CCR_PINC); 
	//Enable memory increment mode
	DMA1_Channel6->CCR |= DMA_CCR_MINC; 
	//Disable circular mode
	DMA1_Channel6->CCR &= ~(DMA_CCR_CIRC); 
	//Set data transfer direction to Peripheral-to-Memory
	DMA1_Channel6->CCR &= ~(DMA_CCR_DIR); //CHANGED THIS
	//Set the data source to data buffer provided in CRC.h
	DMA1_Channel6->CMAR = (uint32_t)active; //CHANGED THIS, DOUBLE CHECK
	//Set the data destination to the data register of the CRC block
	DMA1_Channel6->CPAR = (uint32_t)&(CRC->DR); //TODO - change data destination
	//Disable half transfer interrupt
	DMA1_Channel6->CCR &= ~(DMA_CCR_HTIE); 
	//Disable transfer error interrupt
	DMA1_Channel6->CCR &= ~(DMA_CCR_TEIE); 
	//Enable transfer complete interrupt
	DMA1_Channel6->CCR |= DMA_CCR_TCIE;
	//Set interrupt priority to 0 in NVIC
	NVIC_SetPriority(DMA1_Channel6_IRQn, 0);
	//Enable interrupt in NVIC
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}
