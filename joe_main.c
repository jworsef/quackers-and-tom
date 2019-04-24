#include "STM32F407xx.h"
#include "Board_LED.h"
#include "PB_LCD_Drivers.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <math.h>

int sahOn = 0;	
int LEDState;
int refreshDisplay = 1;
int mode = 0;
float voltsSaHMax; //Max sample and hold value
float voltsSaHMin; //Min sample and hold value
float valueGivenByADC(void); //Predefinition
float systemTime;

void EXTI15_10_IRQHandler (void) { 
/* Interrupt request handler for SW8 and SW9. Must be called before switches can work*/
	if (GPIOE -> IDR &= 0x00004000){
		EXTI -> PR = EXTI_PR_PR14;
		
		//Rotate through the 3 modes, V, I and R
		mode++;	
		if (mode == 3)
		{
			mode = 0;
		}
	}
	else if (GPIOE -> IDR &= 0x00008000){
		EXTI -> PR = EXTI_PR_PR15;
		
		
		sahOn = 1 -sahOn;
		voltsSaHMax = valueGivenByADC();
		voltsSaHMin = valueGivenByADC();
	}
}

float compReturnHigh(float newValue, float oldValue){
/* Compares two float values. The highest of the two is returned. */
	if (newValue > oldValue)
	{
		return newValue;
	}
	else
	{
		return oldValue;
	}
}	

float compReturnLow(float newValue, float oldValue) {
/* Compares two float values. The lowest of the two is returned. */

	if (newValue < oldValue)
	{
		return newValue;
	}
	else
	{
		return oldValue;
	}
}

void ADC_Initialise (){
/* Initialises the ADC. Must be called before any ADC operations are called.*/
	RCC->APB2ENR = (RCC->APB2ENR & 0xFFFFFEFF) | 0x00000100;
	
	ADC1 -> SQR3  = (ADC1->SQR3 & 0xFFFFFFE0)|0x00000000E;
	
	ADC1 -> CR2  = (ADC1->CR2 & 0xFFFFFBFE)|0x00000401;
	
	ADC1 -> CR1  = (ADC1->CR1 & 0xFFFFF7FF)|0x000000800;
}

float valueGivenByADC(){ 
/* Reads the raw value from the ADC and calculates the value of the voltage across it. 	
 * Returns the value as a floating-point number. */
	ADC1 -> CR2  = (ADC1->CR2 & 0xBFFFFFFF)|0x40000000;
	
	if (ADC -> CSR &= 0x00000002){
		// I don't actually know why this is an if statement, but I'm scared to break it.
		float ADCValue;
		
		ADCValue = ADC1 -> DR;
		DAC -> DHR12R1 = ADCValue;
		return (ADCValue/4096)*3;
	}
	else
	{
		return valueGivenByADC(); //And hope it doesn't fail more than once
	}
}

char*	arvStringfromValue(float volts, char unit){
/* Takes a value in unit volts and auto-ranges it to mV. Then converts it to a string,
 * with the value to 4 s.f. followed by the correct unit. It is returned a pointer to  
 * a string. */
	float outputValue;
	char stringOut[10];
	char* ptrStringOut = stringOut;
	
	//Mode of operation checking
	if (unit == 'V') //Volts
	{
		outputValue = ((volts - 1.5f)/0.15f - 0.6f);
	}
	
	else if (unit == (char)222) //Ohms
	{
		outputValue = (volts*995.8f)/(5-volts);
	}
	
	//Autoranging
	if (fabs(outputValue) >= 1) //Fabs gives absolute value of float, polarity of number is irrelevant 
	{
		snprintf (stringOut, 10, "%.3f", outputValue);
	}
	else
	{
		if (fabs(outputValue) >= 0.1)
		{
			snprintf (stringOut, 10, "%.1fm", outputValue*1000);
		}
		else 
		{
			if (fabs(outputValue) >= 0.01)
			{
				snprintf (stringOut, 10, "%.2fm", outputValue*1000);
			}
			else
			{
					snprintf (stringOut, 10, "%.3fm", outputValue*1000);
			}
		}
	}
	//Append the string with the relevant unit
	strcat(stringOut, &unit);
	//Append a space
	strcat(stringOut, " ");
	
  return ptrStringOut;
}


char unitOfMode(){
/* Reads the current mode of operation, returns the correct unit of measurement */
	switch(mode)
	{
		case 0	:
			return 'V';
		case 1	:
			return 'A';
		case 2	:
			return (char)222;
		}
	//If mode is ever equal to or greater than 3, you've broken something
	return 'X';
}

void displayValue(float voltsADC){
/* Takes a volt value from ADC and displays it on the LCD 
 * Only performs string and LCD operations, all convertions and calculates happen in arvStringfromValue*/

	char LCD_out[15];
	char LCD_minmax[15];
	char unit = unitOfMode();
	//Sample and hold
	
	if (sahOn == 1)
	{
		voltsSaHMax = compReturnHigh(voltsADC, voltsSaHMax);
		voltsSaHMin = compReturnLow(voltsADC, voltsSaHMin);
	}
	
	//Pass the volt value to arvStringfromValue() and collect the string it returns.
	strcpy(LCD_out, arvStringfromValue(voltsADC, unit));
	

	PB_LCD_Clear();
	PB_LCD_WriteString(LCD_out, 16);
	
	if (sahOn == 1)
	{
		strcpy(LCD_minmax, arvStringfromValue(voltsSaHMin, unit));
		strncat(LCD_minmax, arvStringfromValue(voltsSaHMax, unit), 16);
		PB_LCD_GoToXY(0, 1);
		PB_LCD_WriteString(LCD_minmax, 16);
	}


}
 
 void SysTick_Handler (void){
/*Many times a second, this is called*/
	refreshDisplay = 1; //Claims that the display is ready to be refreshed
	systemTime++;
}
 
void menuModeSelect (void){
	/*This isn't a function that's ever called, I just wanted to remember the numbers of the following characters:*/
			PB_LCD_WriteChar((char)197); //Up arrow
			PB_LCD_WriteChar((char)198); //Down arrow
}

void initialiseAF(){
	/*Something to do with initiliasing the ADC*/
	//enabling the clock
	RCC->AHB1ENR = (RCC->AHB1ENR & 0xFFFFFFF9) | 0x00000005;
	//Timer 1 external triger and DAC out 1 enable
	GPIOA -> MODER  = (GPIOA->MODER & 0xFCFFFBFF)|0x02000300 ;
	//Input #14 ADC 1 enable
	GPIOC -> MODER  = (GPIOC->MODER & 0xFFFFFBFF)|0x00000300 ;
	//setting DAC 1 as an output
	GPIOA -> OTYPER = (GPIOA->OTYPER & 0xFFFFFFEF);
	//setting PA12 to AF1
	GPIOA -> AFR[1] = (GPIOA->AFR[1] & 0xFFF0FFFF)| 0x00010000;
	//enabling the clock for the DAC
	RCC->APB1ENR = (RCC->AHB1ENR & 0xCFFFFFFF) | 0x20000000;
	//enabling DAC channel 1
	DAC -> CR = (DAC->CR & 0xFFFFFFFE)|0x00000001;
	
}

int main (void) {
	//Something to do with interrupts, ask Tom, not me.
	RCC -> AHB1ENR = (RCC->APB1ENR & 0xFFFFFFE7) | 0x00000018;
		
	RCC -> APB2ENR = (RCC -> APB2ENR & 0xFFFFBFFF) | 0x00004000;

	GPIOE -> MODER = (GPIOE -> MODER & 0x0FFFFFFF);
		
	GPIOE -> PUPDR = (GPIOE -> PUPDR & 0x0FFFFFFF) | 0xA0000000;
		
	SYSCFG -> EXTICR[3] = (SYSCFG -> EXTICR[3] & 0xFFFF00FF) | 0x00004400;
	
	EXTI -> IMR = (EXTI -> IMR & 0xFFFF3FFF) |	0x0000C000;
		
	EXTI -> RTSR = (EXTI -> RTSR & 0xFFFF3FFF) |	0x0000C000;

	EXTI -> FTSR = (EXTI -> FTSR & 0xFFFF3FFF);
	
	// My code starts here
	SystemCoreClockUpdate();
	initialiseAF();
	ADC_Initialise ();
	PB_LCD_Init();
	LED_Initialize();
	SysTick_Config(SystemCoreClock/2); 
	
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	EXTI15_10_IRQHandler ();
	while(1)
	{
		if (refreshDisplay == 1)
		{
			displayValue(valueGivenByADC());
			refreshDisplay = 0;
		}
	}
} 	 	
