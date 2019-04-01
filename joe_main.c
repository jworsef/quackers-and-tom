#include "STM32F407xx.h"
#include "Board_LED.h"
#include "PB_LCD_Drivers.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

int sahHighOn = 0;
int sahLowOn = 0;	
int LEDState;
float voltsSaHMax; //Max sample and hold value
float voltsSaHMin; //Min sample and hold value
float VoltsReadADC();

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
		sahHighOn = 1 -sahHighOn;
		voltsSaHMax = VoltsReadADC();
		
	}
	else if (GPIOE -> IDR &= 0x00008000){
		EXTI -> PR = EXTI_PR_PR15;
		//enter here what you want to do when SW9 is pressed and remove LEDState if wanted
		sahLowOn = 1 - sahLowOn;
		voltsSaHMin = VoltsReadADC();
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

float VoltsReadADC(){ //Reads the value from the ADC and returns a voltage	
	
	ADC1 -> CR2  = (ADC1->CR2 & 0xBFFFFFFF)|0x40000000;
	
	if (ADC -> CSR &= 0x00000002){
		float ADCValue;
		
		ADCValue = ADC1 -> DR;
		DAC -> DHR12R1 = ADCValue;
		return ((ADCValue/4096)*3);
	}
	else
	{
		return VoltsReadADC(); //And hope it doesn't fail more than once
	}
}

char* arvStringfromVolt(float volts) //returns an autoranged value as a string from a given voltage
{
	char stringOut[10];
	char* ptrStringOut = stringOut;
	/*if (volts <= 0) 
	{
		
		// always 4 s.f.
		if (volts >= 0.1)
		{
			snprintf (stringOut, 10, "%.1fmV ", volts*1000);
		}
		else
		{
			if (volts >= 0.01)
			{
				snprintf (stringOut, 10, "%.2fmV ", volts*1000);
			}
			
			else
			{
				snprintf (stringOut, 10, "%.3fmV ", volts*1000);
			}
		}
	}
	else {*/
	if (volts >= 1)
	{
		snprintf (stringOut, 10, "%.3fV ", volts);
	}
	else
	{
		if (volts >= 0.1)
		{
			snprintf (stringOut, 10, "%.1fmV ", volts*1000);
		}
		else 
		{
			if (volts >= 0.01)
			{
				snprintf (stringOut, 10, "%.2fmV ", volts*1000);
			}
			else 
			{
				snprintf (stringOut, 10, "%.3fmV ", volts*1000);
			}
		}
	}
		//}
	// TEST: PB_LCD_WriteString(stringOut, 10);
	
  return ptrStringOut;
}

void displayVoltage(float voltsADC) //Writes a voltage to the LCD
{
	char LCD_out[15];
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
	
	//LCD write out
	PB_LCD_Clear();
	//PB_LCD_GoToXY(0, 0);

	PB_LCD_WriteString(LCD_out, 16);
	
	if (sahHighOn == 1 && sahLowOn == 1)
	{
		strcpy(LCD_minmax, arvStringfromVolt(voltsSaHMin));
  	strncat(LCD_minmax, arvStringfromVolt(voltsSaHMax), 16);
		PB_LCD_GoToXY(0, 1);
		PB_LCD_WriteString(LCD_minmax, 16);
	}

}
 
 void SysTick_Handler (void) 
{ 
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
	
	SystemCoreClockUpdate();
	//initialiseAF();
	ADC_Initialise ();
	PB_LCD_Init();
	LED_Initialize();
	SysTick_Config(SystemCoreClock/2); 
	
	//displayVoltage(VoltsReadADC());
	
	while(1)
	{
		int i = 0;	
		displayVoltage(VoltsReadADC());
		while(i<1000000)
		{
			i=i+1;
		}
	}
} 
