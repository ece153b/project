/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Project
 */

#include "stm32l476xx.h"
#include "motor.h"

static const uint32_t MASK = 0xFFFFFFFF;
static const uint32_t HalfStep[8] = {0x220,0x20,0x120,0x100,0x140,0x40,0x240,0x200};

static volatile int8_t dire = 0;
static volatile uint8_t step = 0;
int stepIndex = 0; 


void Motor_Init(void) {	
	//Enable clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	
	//Configure pins as output
	GPIOC->MODER  &= ~(GPIO_MODER_MODE5); //Clear Pin 5 Bits
	GPIOC->MODER  |= GPIO_MODER_MODE5_0; //Pin 5 output
	GPIOC->MODER  &= ~(GPIO_MODER_MODE6); //Clear Pin 5 Bits
	GPIOC->MODER  |= GPIO_MODER_MODE6_0; //Pin 6 output
	GPIOC->MODER  &= ~(GPIO_MODER_MODE8); //Clear Pin 5 Bits
	GPIOC->MODER  |= GPIO_MODER_MODE8_0; //Pin 8 output
	GPIOC->MODER  &= ~(GPIO_MODER_MODE9); //Clear Pin 5 Bits
	GPIOC->MODER  |= GPIO_MODER_MODE9_0; //Pin 9 output

	//Set output speed to fast
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5); 
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_1; //Pin 5 fast
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED6); 
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED6_1; //Pin 6 fast
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED8); 
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1; //Pin 8 fast
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED9); 
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED9_1; //Pin 9 fast	
	
	//Set output type to push pull
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT5); //Pin 5
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT6); //Pin 6
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT8); //Pin 8
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT9); //Pin 9

	//Set pins to no pull-up, no pull-down
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD5); //Pin 5
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD6); //Pin 6
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD8); //Pin 8
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD9); //Pin 9
}

void rotate(void) {
	GPIOC->ODR &= ~(MASK); 
	GPIOC->ODR |= HalfStep[stepIndex];
	if(dire == 1)
	{
			stepIndex++; 
			if(stepIndex > 7)
			{
				stepIndex = 0;
			}
	}
	else
	{
			stepIndex--;
			if(stepIndex < 0)
			{
				stepIndex = 7;
			}
	}
}

void setDire(int8_t direction) {
	dire = direction;
}
	


