#include "STM32F407xx.h"
#include "Board_LED.h"

uint32_t LEDState = 0;

void SysTick_Handler (void) {
	if (LEDState > 3) 
		LEDState = 0;
	else LEDState = LEDState+1;
} 

void initialiseLEDandButtonPorts()
{
	//Enable clock to GPIO Port D for LEDs
	RCC->AHB1ENR = (RCC->AHB1ENR & RCC_AHB1ENR_GPIODEN_Msk) | (0x1 << RCC_AHB1ENR_GPIODEN_Pos); 
	
	//Set LED 1-4 to General Purpose Output Mode
	GPIOD->MODER = (GPIOD->MODER & ~GPIO_MODER_MODER12_Msk) | (0x01 << GPIO_MODER_MODER12_Pos); 
	GPIOD->MODER = (GPIOD->MODER & ~GPIO_MODER_MODER13_Msk) | (0x01 << GPIO_MODER_MODER13_Pos); 
	GPIOD->MODER = (GPIOD->MODER & ~GPIO_MODER_MODER14_Msk) | (0x01 << GPIO_MODER_MODER14_Pos); 
	GPIOD->MODER = (GPIOD->MODER & ~GPIO_MODER_MODER15_Msk) | (0x01 << GPIO_MODER_MODER15_Pos); 
	//Sety LED 1-4 to push-pull mode
	GPIOD->OTYPER = (GPIOD->OTYPER & ~GPIO_OTYPER_OT12_Msk) | (0x0 << GPIO_OTYPER_OT12_Pos); 
	GPIOD->OTYPER = (GPIOD->OTYPER & ~GPIO_OTYPER_OT13_Msk) | (0x0 << GPIO_OTYPER_OT13_Pos); 
	GPIOD->OTYPER = (GPIOD->OTYPER & ~GPIO_OTYPER_OT14_Msk) | (0x0 << GPIO_OTYPER_OT14_Pos); 
	GPIOD->OTYPER = (GPIOD->OTYPER & ~GPIO_OTYPER_OT15_Msk) | (0x0 << GPIO_OTYPER_OT15_Pos); 
}

int main (void) 
{
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock);
	initialiseLEDandButtonPorts();
	//LED_Initialize();
	while (1)
	{ 
		SysTick_Handler();
		/*if (LEDState==0)
		{
			LED_On(0);
			LED_Off(3);
		}
		else if (LEDState==1)
		{
			LED_On(1);
			LED_Off(0);
		}
		else if (LEDState==2)
		{
			LED_On(2);
			LED_Off(1);
		}
		else if (LEDState==3)
		{
			LED_On(3);
			LED_Off(2);
			

		} */
	}
} 
