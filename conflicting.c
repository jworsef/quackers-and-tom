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
