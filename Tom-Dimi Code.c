#include "STM32F407xx.h"
#include "Board_LED.h"

uint32_t LEDState = 0;

void SysTick_Handler (void) {
	buttonDetect();
		
}
	
//if (LEDState == 0) LEDState = 1;
//else if (LEDState == 1) LEDState = 2;
//else if (LEDState == 2) LEDState = 3;
//else LEDState = 0;
//}

void greenLED_only (){
		GPIOD -> BSRR =   0xE0001000; //green LED
}

void orangeLED_only (){
	GPIOD -> BSRR =   0xD0002000;
}

void redLED_only (){
	GPIOD -> BSRR =   0xB0004000;
}

void blueLED_only (){
	GPIOD -> BSRR =   0x70008000;
}

int buttonDetect () {
	if (GPIOA -> IDR &= 0x00000001)
		return 1;
	else 
		return 0;
	
	
}

void initialiseLEDandButtonPorts(){
	RCC->AHB1ENR = (RCC->AHB1ENR & 0x00000000) | 0x00000009;
	GPIOD -> MODER  = (GPIOD->MODER & 0x00FFFFFF)|0x55000000 ;//| GPIOD -> MODER;
	GPIOA -> MODER  = (GPIOA->MODER & 0x00000000) ;//| GPIOA -> MODER;
	GPIOD -> OTYPER = (GPIOD->OTYPER & 0x00000000) ;//| GPIOD -> OTYPER;
	GPIOD -> PUPDR  = (GPIOD->PUPDR & 0x00000000) ;//| GPIOD -> PUPDR;
	GPIOA -> PUPDR  = (GPIOA->PUPDR & 0x00000002) ;//| GPIOA -> PUPDR;
	GPIOD -> OSPEEDR = (GPIOD->OSPEEDR & 0x00000000) ;
}

int main (void) {
SystemCoreClockUpdate();
SysTick_Config(SystemCoreClock/4);
initialiseLEDandButtonPorts();
SysTick_Handler ();
while (1) {
	if (GPIOA -> IDR &= 0x00000001){
		if (LEDState == 0) GPIOD -> BSRR =   0xE0001000;
		else if  (LEDState == 1) GPIOD -> BSRR =   0xD0002000;
		else if  (LEDState == 2) GPIOD -> BSRR =   0xB0004000;
		else GPIOD -> BSRR =   0x70008000;
		}
//while (1) {
//	buttonDetect ();
	//GPIOD -> BSRR = 0x0000F000;
	//if (GPIOA -> IDR &= 0x00000001){
		//buttonDetect ();
	}
}
