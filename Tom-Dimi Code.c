#include "STM32F407xx.h"
#include "Board_LED.h"

uint32_t LEDState = 0;

int buttonDetect () {
	if (GPIOA -> IDR &= 0x00000001)
		return 1;
	else 
		return 0;
}

void SysTick_Handler (void) {
	if (buttonDetect()==1){
		LEDState = LEDState + 1;
		if (LEDState == 4){
			LEDState = 0;
		}
}
}

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
SysTick_Config(SystemCoreClock/6 );
initialiseLEDandButtonPorts();
SysTick_Handler ();
while (1) {
	if (GPIOA -> IDR &= 0x00000001){
		if (LEDState == 0) greenLED_only();
		else if  (LEDState == 1) orangeLED_only();
		else if  (LEDState == 2) redLED_only();
		else blueLED_only();
		}
	}
}
