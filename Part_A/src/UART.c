#include "UART.h"

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

void USART_Init(USART_TypeDef* USARTx) {
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
	
	// Enable USART
	USARTx->CR1 |= USART_CR1_UE;
}

uint8_t USART_Read (USART_TypeDef * USARTx) {
	// SR_RXNE (Read data register not empty) bit is set by hardware
	while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
	// USART resets the RXNE flag automatically after reading DR
	return ((uint8_t)(USARTx->RDR & 0xFF));
	// Reading USART_DR automatically clears the RXNE flag 
}

void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	int i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR 
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));   	// wait until TXE (TX empty) bit is set
		// Writing USART_DR automatically clears the TXE flag 	
		USARTx->TDR = buffer[i] & 0xFF;
		USART_Delay(300);
	}
	while (!(USARTx->ISR & USART_ISR_TC));   		  // wait until TC bit is set
	USARTx->ISR &= ~USART_ISR_TC;
}   

void USART_Delay(uint32_t us) {
	uint32_t time = 100*us/7;    
	while(--time);   
}
