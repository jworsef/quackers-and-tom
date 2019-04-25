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

void interruptInit(){
	RCC -> AHB1ENR = (RCC->APB1ENR & 0xFFFFFFE7) | 0x00000018;
		
	RCC -> APB2ENR = (RCC -> APB2ENR & 0xFFFFBFFF) | 0x00004000;

	GPIOE -> MODER = (GPIOE -> MODER & 0x0000FFFF);
		
	GPIOE -> PUPDR = (GPIOE -> PUPDR & 0x0000FFFF) | 0xAAAA0000;
		
	SYSCFG -> EXTICR[2] = (SYSCFG -> EXTICR[2] & 0xFFFF0000) | 0x00004444;
	
	SYSCFG -> EXTICR[3] = (SYSCFG -> EXTICR[3] & 0xFFFF0000) | 0x00004444;
	
	EXTI -> IMR = (EXTI -> IMR & 0xFFFF00FF) |	0x0000FF00;
		
	EXTI -> RTSR = (EXTI -> RTSR & 0xFFFF00FF) |	0x0000FF00;

	EXTI -> FTSR = (EXTI -> FTSR & 0xFFFF00FF);
}

void EXTI15_10_IRQHandler (void) { 
	if (GPIOE -> IDR &= 0x00008000){
		EXTI -> PR = EXTI_PR_PR15;
		//enter here what you want to do when SW9 is pressed
	}
	else if (GPIOE -> IDR &= 0x00004000){
		EXTI -> PR = EXTI_PR_PR14;
		//enter here what you want to do when SW8 is pressed
	}
	else if (GPIOE -> IDR &= 0x00002000){
		EXTI -> PR = EXTI_PR_PR13;
		////enter here what you want to do when SW7 is pressed
	}
	else if (GPIOE -> IDR &= 0x00001000){
		EXTI -> PR = EXTI_PR_PR12;
		//enter here what you want to do when SW6 is pressed
	}
	else if (GPIOE -> IDR &= 0x00000800){
		EXTI -> PR = EXTI_PR_PR11;
		//enter here what you want to do when SW5 is pressed
	}
	else if (GPIOE -> IDR &= 0x00000400){
		EXTI -> PR = EXTI_PR_PR10;
		//enter here what you want to do when SW4 is pressed
	}
}

void EXTI9_5_IRQHandler (void) { 
 if (GPIOE -> IDR &= 0x00000200){
		EXTI -> PR = EXTI_PR_PR9;
		//enter here what you want to do when SW3 is pressed
	}
	else if (GPIOE -> IDR &= 0x00000010){
		EXTI -> PR = EXTI_PR_PR8;
		//enter here what you want to do when SW2 is pressed
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
	
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	initialiseLED();
	interruptInit();
	EXTI15_10_IRQHandler ();
	EXTI9_5_IRQHandler ();
	
	while(1){
		// change this for other function that you want to happen dependant on 
		//what should happen when each button is pressed
		if (LEDState == 0) orangeLED_only();
		else if  (LEDState == 1) greenLED_only();
	}
	
}
