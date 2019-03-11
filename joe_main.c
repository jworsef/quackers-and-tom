#include "STM32F407xx.h"
#include "Board_LED.h"
#include "PB_LCD_Drivers.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

int sahHighOn = 1;
int sahLowOn = 1 ;	
float voltsSaHMax; //Max sample and hold value
float voltsSaHMin; //Min sample and hold value


void EXTI15_10_IRQHandler (void) { 
	if (GPIOE -> IDR &= 0x00004000){
		EXTI -> PR = EXTI_PR_PR14;
		//enter here what you want to do when SW8 is pressed and remove LEDState if wanted
		sahHighOn = 1 - sahHighOn; //Toggle whether you're sample and holding high values.
	}
		
	else if (GPIOE -> IDR &= 0x00008000){
		EXTI -> PR = EXTI_PR_PR15;
		//enter here what you want to do when SW9 is pressed and remove LEDState if wanted
		sahLowOn = 1 - sahLowOn; //Toggle whether you're sample and holding low values.
	}
}

void initialiseAF(){
	//enabling the clock
	RCC->AHB1ENR = (RCC->AHB1ENR & 0x00000000) | 0x00000005;
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

float compReturnHigh(float newValue, float oldValue) //Compares values, returns highest
{
	if (newValue > oldValue)
	{
		return newValue;
	}
	else
	{
		return oldValue;
	}
	}	
	
float compReturnLow(float newValue, float oldValue) //Compares values, returns lowest
{
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
	RCC->APB2ENR = (RCC->APB2ENR & 0xFFFFFEFF) | 0x00000100;
	
	ADC1 -> SQR3  = (ADC1->SQR3 & 0xFFFFFFE0)|0x00000000E;
	
	ADC1 -> CR2  = (ADC1->CR2 & 0xFFFFFBFE)|0x00000401;
	
	ADC1 -> CR1  = (ADC1->CR1 & 0xFFFFF7FF)|0x000000800;
}

float voltsFromADC(){ //Reads the value from the ADC and returns a voltage	
	
	ADC1 -> CR2  = (ADC1->CR2 & 0xBFFFFFFF)|0x40000000;
	
	if (ADC -> CSR &= 0x00000002){
		float ADCconv;
		
		ADCconv = ADC1 -> DR;
		DAC -> DHR12R1 = ADCconv;
		ADCconv = (ADCconv/4096)*3;
		return ADCconv;
	}
}

char* arvStringfromVolt(float volts) //returns an autoranged value as a string from a given voltage
{
	char stringOut[10];
	char* ptrStringOut = stringOut;
	if (volts < 1) 
	{
		
		// always 4 s.f.
		if (volts >= 0.1)
		{
			snprintf (stringOut, 10, "%.1f mV", volts*1000);
		}
		else
		{
			snprintf (stringOut, 10, "%.2f mV", volts*1000);
		}
	}
	else {
		snprintf (stringOut, 10, "%.3f V", volts);
	}
	// TEST: PB_LCD_WriteString(stringOut, 10);
	
  return ptrStringOut;
}

void displayVoltage(float voltsADC) //Writes a voltage to the LCD
{
	char LCD_out[7];
	char LCD_minmax[15];

	
		
	//Sample and hold
	
	if (sahHighOn == 1)
	{
		voltsSaHMax = compReturnHigh(voltsADC, voltsSaHMax);
	}
	if (sahLowOn == 1)
	{
		voltsSaHMin = compReturnLow(voltsADC, voltsSaHMin);
	}
	
	//Auto-ranging 
	strcpy(LCD_out, arvStringfromVolt(voltsADC));
	strcpy(LCD_minmax, arvStringfromVolt(voltsSaHMin));
	strncat(LCD_minmax, arvStringfromVolt(voltsSaHMax), 16);
	
	//LCD write out
	PB_LCD_GoToXY(0, 0);
	PB_LCD_WriteString(LCD_out, 10);
	PB_LCD_GoToXY(0, 1);
	PB_LCD_WriteString(LCD_minmax, 16);

}
 
 void SysTick_Handler (void) 
{ 
}
 
int main (void) {
	
	SystemCoreClockUpdate();
	initialiseAF();
	ADC_Initialise ();
	PB_LCD_Init();
	SysTick_Config(SystemCoreClock/2); 
	
	while(1){
		int i = 0;
		displayVoltage(voltsFromADC());
		while(i<1000000){
			i=i+1;
		}
	}
}
