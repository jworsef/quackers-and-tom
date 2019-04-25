#include "STM32F407xx.h" 
#include "Board_LED.h" 
#include "PB_LCD_Drivers.h"
#include <stdio.h> 

int beepState;


void beepInit()
{
	RCC -> AHB1ENR = (RCC->APB1ENR & 0xFFFFFFFD) | 0x00000002;
	GPIOB -> MODER = (GPIOB -> MODER & 0xFFFFFF3F) | 0x00000040;
	GPIOB -> OTYPER = (GPIOB -> OTYPER & 0xFFFFFFF7);
	GPIOB -> PUPDR = (GPIOB -> PUPDR & 0xFFFFFF3F) | 0x00000080;
}

void SysTick_Handler(){
	if (beepState == 0){
		beepState = 1;
		GPIOB -> BSRR = (GPIOB -> BSRR & 0xFFF7FFFF) | 0x00080000;
	}
	else if (beepState == 1){
		beepState = 0;
		GPIOB -> BSRR = (GPIOB -> BSRR & 0xFFFFFFF7) | 0x00000008;
	}
}

int main (void) {
	beepInit();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock);
	SysTick_Handler();
	while (1){
		
	}
}
