/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Project
 */
 
#include "DMA.h"
#include "SysTimer.h"

void DMA_Init_UARTx(DMA_Channel_TypeDef * tx, USART_TypeDef * uart) {
	//Enable the clock for DMA
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN; 
	//Wait 20us for DMA to finish setting up
	delay(20);  	
	//Disable Channel 6
	DMA1_Channel6->CCR &= ~(DMA_CCR_EN);
	//Set Memory-to-Memory mode
	DMA1_Channel6->CCR |= DMA_CCR_MEM2MEM;
	//Set channel priority to high (10)
	DMA1_Channel6->CCR &= ~(DMA_CCR_PL); 
	DMA1_Channel6->CCR |= DMA_CCR_PL_1;
	//Set peripheral size to 32 bit (10)
	DMA1_Channel6->CCR &= ~(DMA_CCR_PSIZE); 
	DMA1_Channel6->CCR |= DMA_CCR_PSIZE_1; 
	//Set memory size to 32-bit (10)
	DMA1_Channel6->CCR &= ~(DMA_CCR_MSIZE); 
	DMA1_Channel6->CCR |= DMA_CCR_MSIZE_1; 
	//Disable peripheral increment mode
	DMA1_Channel6->CCR &= ~(DMA_CCR_PINC); 
	//Enable memory increment mode
	DMA1_Channel6->CCR |= DMA_CCR_MINC; 
	//Disable circular mode
	DMA1_Channel6->CCR &= ~(DMA_CCR_CIRC); 
	//Set data transfer direction to Memory-to-Peripheral
	DMA1_Channel6->CCR |= DMA_CCR_DIR; 
	//Set the data source to data buffer provided in CRC.h
	DMA1_Channel6->CMAR = (uint32_t)DataBuffer; //TODO - change data source
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
