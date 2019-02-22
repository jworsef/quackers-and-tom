#include "STM32F407xx.h"
#include "Board_LED.h"
#include "PB_LCD_Drivers.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

float ADCconv;
char LCD_out[7];
int i;

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

void ADC_Initialise (){
	RCC->APB2ENR = (RCC->APB2ENR & 0xFFFFFEFF) | 0x00000100;
	
	ADC1 -> SQR3  = (ADC1->SQR3 & 0xFFFFFFE0)|0x00000000E;
	
	ADC1 -> CR2  = (ADC1->CR2 & 0xFFFFFBFE)|0x00000401;
	
	ADC1 -> CR1  = (ADC1->CR1 & 0xFFFFF7FF)|0x000000800;
}

void ADC_Control(){	
	ADC1 -> CR2  = (ADC1->CR2 & 0xBFFFFFFF)|0x40000000;
	
	if (ADC -> CSR &= 0x00000002){
		
		ADCconv = ADC1 -> DR;
		DAC -> DHR12R1 = ADCconv;
		PB_LCD_Init();
		PB_LCD_Clear();
		ADCconv = (ADCconv/4096)*3;
		snprintf (LCD_out, 7, "%f", ADCconv);
		PB_LCD_WriteString(LCD_out, 7);
		
	}
 }

int main (void) {
	initialiseAF();
	ADC_Initialise ();
	while(1){
		i=0;
		ADC_Control();
		while(i<1000000){
			i=i+1;
		}
	}
}
