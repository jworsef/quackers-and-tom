#include "STM32F407xx.h" 
#include "Board_LED.h" 
#include "PB_LCD_Drivers.h"
#include <stdio.h> 

int LEDState;


void initialiseLED(){
	GPIOD -> MODER  = (GPIOD->MODER & 0x00FFFFFF)|0x55000000 ;//| GPIOD -> MODER;
	GPIOD -> OTYPER = (GPIOD->OTYPER & 0xFFFF0000) ;//| GPIOD -> OTYPER;
	GPIOD -> PUPDR  = (GPIOD->PUPDR & 0x00000000) ;//| GPIOD -> PUPDR;
	GPIOD -> OSPEEDR = (GPIOD->OSPEEDR & 0x00000000) ;
}

void EXTI15_10_IRQHandler (void) { 
	if (GPIOE -> IDR &= 0x00004000){
		EXTI -> PR = EXTI_PR_PR14;
		//enter here what you want to do when SW8 is pressed and remove LEDState if wanted
		LEDState = 0;
	}
	else if (GPIOE -> IDR &= 0x00008000){
		EXTI -> PR = EXTI_PR_PR15;
		//enter here what you want to do when SW9 is pressed and remove LEDState if wanted
		LEDState = 1;
	}
}

void greenLED_only (){
		GPIOD -> BSRR =   0xE0001000; //green LED
}

void orangeLED_only (){
	GPIOD -> BSRR =   0xD0002000; // orange LED
}

void redLED_only (){
	GPIOD -> BSRR =   0xB0004000; // red LED
}

void blueLED_only (){
	GPIOD -> BSRR =   0x70008000; // blue LED
}

int main (void) {
	RCC -> AHB1ENR = (RCC->APB1ENR & 0xFFFFFFE7) | 0x00000018;
		
	RCC -> APB2ENR = (RCC -> APB2ENR & 0xFFFFBFFF) | 0x00004000;

	GPIOE -> MODER = (GPIOE -> MODER & 0x0FFFFFFF);
		
	GPIOE -> PUPDR = (GPIOE -> PUPDR & 0x0FFFFFFF) | 0xA0000000;
		
	SYSCFG -> EXTICR[3] = (SYSCFG -> EXTICR[3] & 0xFFFF00FF) | 0x00004400;
	
	EXTI -> IMR = (EXTI -> IMR & 0xFFFF3FFF) |	0x0000C000;
		
	EXTI -> RTSR = (EXTI -> RTSR & 0xFFFF3FFF) |	0x0000C000;

	EXTI -> FTSR = (EXTI -> FTSR & 0xFFFF3FFF);
	
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	initialiseLED();
	EXTI15_10_IRQHandler ();
	
	while(1){
		// change this for other function that you want to happen dependant on 
		//what should happen when each button is pressed
		if (LEDState == 0) orangeLED_only();
		else if  (LEDState == 1) greenLED_only();
	}
	
}
