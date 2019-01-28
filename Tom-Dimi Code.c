#include "STM32F407xx.h"
#include "Board_LED.h"

uint32_t LEDState = 0;

void SysTick_Handler (void) {
if (LEDState == 0) LEDState = 1;
else if (LEDState == 1) LEDState = 2;
else if (LEDState == 2) LEDState = 3;
else LEDState = 0;
}

void greenLED_invert (){
	if (GPIOD -> ODR == 0x0000F000) {
		GPIOD -> ODR =   0x00000000; }//green LED
	else{ GPIOD -> ODR =   0x0000F000;}
}

void buttonDetect () {
	if (GPIOA -> IDR == 0x00000001)
	{greenLED_invert ();}
}

void initialiseLEDandButtonPorts(){
	//GPIOA -> MODER = (GPIOA -> MODER & 0x000) | (0x01 << GPIO_MODER_MODER1_Pos);
	GPIOD -> MODER  = 0x55000000 ;//| GPIOD -> MODER;
	GPIOA -> MODER  = 0x00000000 ;//| GPIOA -> MODER;
	GPIOD -> OTYPER = 0x00000000 ;//| GPIOD -> OTYPER;
	GPIOA -> OTYPER = 0x00000000 ;//| GPIOA -> OTYPER;
	GPIOD -> PUPDR  = 0x00000000 ;//| GPIOD -> PUPDR;
	GPIOA -> PUPDR  = 0x00000000 ;//| GPIOA -> PUPDR;
	
//	GPIOD -> BSRR = 0X0000000C; //green LED
//	GPIOD -> BSRR = 0X0000000D; //orange LED
//	GPIOD -> BSRR = 0X0000000E; //red LED
//	GPIOD -> BSRR = 0X0000000F; //blue LED
}

int main (void) {
SystemCoreClockUpdate();
SysTick_Config(SystemCoreClock/4);
//LED_Initialize();
initialiseLEDandButtonPorts();
//while (1) {
//if (LEDState == 0) LED_On(0), LED_Off(3);
//else if  (LEDState == 1) LED_On(1), LED_Off(0);
//else if  (LEDState == 2) LED_On(2), LED_Off(1);
//else LED_On(3), LED_Off(2);
//}
//while (1) {
	//buttonDetect ();
	greenLED_invert () ;
//}

}
