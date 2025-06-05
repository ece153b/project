#include "SPI.h"
#include "SysTimer.h"

void SPI1_GPIO_Init(void) {
	// Enable the GPIO Clock
	// Set PA4, PB3, PB4, and PB5 to Alternative Functions, and configure their AFR to SPI1
	// Set GPIO Pins to: Very High Output speed, Output Type Push-Pull, and No Pull-Up/Down
	
	//Enable Clock for PA4, PB3, PB4, PB5
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	
	//Choose Alternative Mode
	//PA4
	GPIOA->MODER &= ~(GPIO_MODER_MODE4); 
	GPIOA->MODER |= GPIO_MODER_MODE4_1; 
	//PB3
	GPIOB->MODER &= ~(GPIO_MODER_MODE3);
	GPIOB->MODER |= GPIO_MODER_MODE3_1; 
	//PB4
	GPIOB->MODER &= ~(GPIO_MODER_MODE4);
	GPIOB->MODER |= GPIO_MODER_MODE4_1; 
	//PB5
	GPIOB->MODER &= ~(GPIO_MODER_MODE5);
	GPIOB->MODER |= GPIO_MODER_MODE5_1;
	
	//Choose alternate function (PA4: SPI1_NSS, PB3: SPIL_SCK, PB4: SPI1_MISO, PB5: SPI1_MOSI)
	//Set AF for PA4 to AF5 (0101)
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL4);
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL4_0;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL4_2;

	//Set AF for PB3 to AF5 (0101)
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL3);
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL3_0;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL3_2;
	
	//Set AF for PB4 to AF5 (0101)
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL4);
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL4_0;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL4_2;
	
	//Set AF for PB5 to AF5 (0101)
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL5);
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL5_0;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL5_2;
	
	//Set output type to push-pull (0)
	//PA4
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT4); 
	//PB3
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT3); 
	//PB4
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT4); 
	//PB5
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT5); 
	
	//Operate at very high speed
	//PA4
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED4;
	//PB3
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED3;
	//PB4
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED4;
	//PB5
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED5;

	//Pull up pull down
	//PA4
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD4); 
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD4_0;
	//PB3
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD3); 
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD3_0;
	//PB4
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD4); 
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD4_0;
	//PB5
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD5); 
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD5_0;
}


void SPI1_Init(void){
// Enable SPI clock and Reset SPI
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST; 
	RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI1RST); 

	// Disable SPI	
	SPI1->CR1 &= ~(SPI_CR1_SPE); 	
	// Configure for Full Duplex Communication
	SPI1->CR1 &= ~(SPI_CR1_RXONLY); 
	// Configure for 2-line Unidirectional Data Mode
	SPI1->CR1 &= ~(SPI_CR1_BIDIMODE); 
	// Disable Output in Bidirectional Mode
	SPI1->CR1 &= ~(SPI_CR1_BIDIOE); 
	// Set Frame Format: MSB First, 16-bit, Motorola Mode
	SPI1->CR1 &= ~(SPI_CR1_LSBFIRST);
	SPI1->CR2 |= SPI_CR2_DS; 
	SPI1->CR2 &= ~(SPI_CR2_FRF); 
	
	// Configure Clock. Read DataSheet for required value (DOUBLE CHECK THESE)
	SPI1->CR1 |= SPI_CR1_CPOL;
	SPI1->CR1 |= SPI_CR1_CPHA;  
	
	// Set Baud Rate Prescaler to 16
	SPI1->CR1 &= ~SPI_CR1_BR;
	SPI1->CR1 |= SPI_CR1_BR_0; 
	SPI1->CR1 |= SPI_CR1_BR_1; 


	// Disable Hardware CRC Calculation
	SPI1->CR1 &= ~(SPI_CR1_CRCEN); 
	
	// Set as Master
	SPI1->CR1 |= SPI_CR1_MSTR; 
	
	// Disable Software Slave Management
	SPI1->CR1 &= ~(SPI_CR1_SSM); 
	
	// Enable NSS Pulse Management
	SPI1->CR2 |= SPI_CR2_NSSP; 
	
	// Enable Output
	SPI1->CR2 |= SPI_CR2_SSOE;
	
	// Set FIFO Reception Threshold to 1/2
	SPI1->CR2 &= ~(SPI_CR2_FRXTH); 
	// Enable SPI
	SPI1->CR1 |= SPI_CR1_SPE; 	
}

uint16_t SPI_Transfer_Data(uint16_t write_data){
	// Wait for TXE (Transmit buffer empty)
	while(SPI1->SR & SPI_SR_TXE == 0){} 
	// Write data
	SPI1->DR = write_data; 
	// Wait for not busy
	while(SPI1->SR & SPI_SR_BSY){}
	// Wait for RXNE (Receive buffer not empty)
	while(SPI1->SR & SPI_SR_RXNE == 0){} 
	// Read data
	return SPI1->DR; 
}
