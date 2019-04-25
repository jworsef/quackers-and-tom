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
int store = 0;
int ohmMode = 0;
int nextNode = 0;
int prevNode = 0;
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
		if (mode == 5)
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
	else if (GPIOE -> IDR &= 0x00002000){ //store button
		EXTI -> PR = EXTI_PR_PR13;
		store = 1;
	}
	else if (GPIOE -> IDR &= 0x00001000){ //ohmMode button
		EXTI -> PR = EXTI_PR_PR12;
		ohmMode=1-ohmMode;//swap between 0 <--> 1 
	}
	else if (GPIOE -> IDR &= 0x00000800){ //next node button
		EXTI -> PR = EXTI_PR_PR11;
		nextNode=1;
	}
	else if (GPIOE -> IDR &= 0x00000400){ //prev node button
		EXTI -> PR = EXTI_PR_PR10;
		prevNode=1;
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
	//Timer 1 external trigger and DAC out 1 enable
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


void Vfunc(struct ValuesList* list)
{
	//Voltmeter Mode and accuracy (displayed for 3sec)
	PB_LCD_Clear();
	PB_LCD_WriteString("Voltmeter",14);
	PB_LCD_GoToXY(0, 1);
	PB_LCD_WriteString("Accuracy", 16); //ADD THE VALUE
	
	//wait statement
	wait3sec();	

	int myFlag=0;
	
	while(mode==0)
	{
		if (refreshDisplay == 1)
		{
			//function that takes a volt value from 0-3 Volt
			//and returns a value from -10 to 10 V
			displayValue(valueGivenByADC());
			
			//if the user wants to store a value store the value
			if(store==1)
			{	
				store=0;
				storeValue(list,arvStringfromValue(valueGivenByADC(),unitOfMode()));
			}
			
			//if the user tries to measure a value close to the max beep thrice (:P three times)
			if(abs(valueGivenByADC())>9.5 && myFlag==0)
			{
				
				//briefly display what the problem is ?
				PB_LCD_Clear();
				PB_LCD_WriteString("Value measured",16);
				PB_LCD_GoToXY(0, 1);
				PB_LCD_WriteString("is close to max", 16);
				
				beep();
				//do we need other wait ?			

				//set the flag high
				myFlag=1;
			}
			//reset the flag when the value drops lower 
			if(abs(valueGivenByADC())<9.3 && myFlag==1) 
			{
				//set the flag low
				myFlag=0;
			}
			refreshDisplay = 0;
		}
	}
}

//do we wanna beep inside here ?
void Rfunc(struct ValuesList* list)
{
	//Ohmmeter Mode, what mode is at and accuracy (displayed for 3sec)
	PB_LCD_Clear();
	PB_LCD_WriteString("Ohmmeter",13);
	PB_LCD_GoToXY(0, 1);
	PB_LCD_WriteString("Accuracy", 16); //ADD THE VALUE
	
	//wait statement
	wait3sec();
	
	while(mode==1)
	{
		if (refreshDisplay == 1)
		{
			while(ohmMode==0)
			{
				//we will check A pin and display at lcd the value we think the resistor has
				displayValue(valueGivenByADC()); //both A and B use the same
				
				//if the user wants to store a value store the value
				if(store==1)
				{
					store=0;
					storeValue(list,arvStringfromValue(valueGivenByADC(),unitOfMode()));
				}
			}
			while(ohmMode==1)
			{
				//we will check B pin and display at lcd the value we think the resistor has
				displayValue(valueGivenByADC()); //both A and B use the same
				
				//if the user wants to store a value store the value
				if(store==1)
				{
					store=0;
					storeValue(list,arvStringfromValue(valueGivenByADC(),unitOfMode()));
				}
			}
		refreshDisplay = 0;
		}
	}
}	

void Ifunc(struct ValuesList* list)
{
	//current mode and accuracy (displayed for 3sec)
	PB_LCD_Clear();
	PB_LCD_WriteString("Ammeter",12);
	PB_LCD_GoToXY(0, 1);
	PB_LCD_WriteString("Accuracy", 16); //ADD THE VALUE
	
	//wait statement
	wait3sec();

///for calculating only low current else we need to add modes
	while(mode==2)
	{
		if (refreshDisplay == 1)
		{
			//function that takes a voltage value for low current
			//and returns the current value
			displayValue(valueGivenByADC());
			
			//if the user wants to store a value store the value
			if(store==1)
			{
				store=0;
				storeValue(list,arvStringfromValue(valueGivenByADC(),unitOfMode()));
			}
			
			//if the user tries to measure a value close to the max beep thrice (:P three times)
			if(abs(valueGivenByADC())>0.95 && myFlag==0)
			{			
				//briefly display what the problem is ?
				PB_LCD_Clear();
				PB_LCD_WriteString("Value measured",16);
				PB_LCD_GoToXY(0, 1);
				PB_LCD_WriteString("is close to max", 16);
				
				beep();

				//set the flag high
				myFlag=1;
			}
			//reset the flag when the value drops lower 
			if(abs(valueGivenByADC())<0.93 && myFlag==1)
			{
				//set the flag low
				myFlag=0;
			}
		refreshDisplay=0;
		}
	}
}

void Stored_func(struct ValuesList* list)
{
	//current mode and number of stored values(? no) (displayed for 3sec)
	PB_LCD_Clear();
	PB_LCD_WriteString("Stored Values",13);
	
	//wait statement
	wait3sec();

	struct ValuesNode* displayedNode=list->head; //allways display the first node first
	//reset buttons
	nextNode=0;
	prevNode=0;
	while(mode==4)
	{
		if (refreshDisplay == 1)
		{
			//if list is empty let the user know
			//else let the user interact with the list
			if (list->size==0)
			{
				PB_LCD_Clear();
				PB_LCD_WriteString("Nothing Stored",15);
			}
			else
			{
				printLCD(displayedNode);
				if(nextNode==1 && list->size!=1)
				{
					nextNode=0;
					moveToNextNode(displayedNode); // this fucntion is really simple do we really need it ?
				}
				if(prevNode==1 && list->size!=1)
				{
					prevNode=0;
					moveToPrevNode(displayedNode); // this fucntion is really simple do we really need it ?
				}
			}
		refreshDisplay = 0;
		}
	}
	nextNode=0;
	prevNode=0;
	store=0; // so that if the store button is pressed inside here it will be reseted automatically before the next function that actually uses it
}

void Ffunc(struct ValuesList* list)
{
	//current mode and accuracy (3 sec)
	PB_LCD_Clear();
	PB_LCD_WriteString("Frequency Meter",15);
	PB_LCD_GoToXY(0, 1);
	PB_LCD_WriteString("Accuracy", 16); //ADD THE VALUE
	
	wait3sec();
	
	while(mode==3)
	{
		if (refreshDisplay == 1)
		{
			//rising edge/time = freq 
			//display it 
			
			//if the user wants to store a value store the value
			if(store==1)
			{
				store=0;
				storeValue(list,arvStringfromValue(valueGivenByADC(),unitOfMode()));
			}
			
			//if the user tries to measure a value close to the max beep thrice (:P three times)
			if(abs(valueGivenByADC())>9.5 && myFlag==0)
			{
				//briefly display what the problem is ?
				//this code has been reapeted maybe make a func for it ?
				PB_LCD_Clear();
				PB_LCD_WriteString("Value measured",16);
				PB_LCD_GoToXY(0, 1);
				PB_LCD_WriteString("is close to max", 16);
				
				beep();
				
				//set the flag high
				myFlag=1;
			}
			//reset the flag when the value drops lower 
			if(abs(valueGivenByADC())<9.3 && myFlag==1)
			{
				//set the flag low
				myFlag=0;
			}
		refreshDisplay=0;	
		}
	}
}

///////  lvl 3 requirement  //////

/// STRUCTURE TO STORE VALUES

struct ValuesList
{
	struct ValuesNode* head;
	int size;
}

struct ValuesNode
{
	char stringStored[cs];
	int readingnumber;
	struct ValuesNode* next;
	struct ValuesNode* prev;
}

struct ValuesNode* nodeConstructor(char string[],int readingnumber)
{
	struct ValuesNode* newNode;
	
	//reserve memory for the node
	newNode = malloc(sizeof(struct ValuesNode));
	
	//initialise values
	newNode->readingnumber=readingnumber;
	snprintf(newNode->stringStored,sizeof(newNode->stringStored),"%s",string);
	newNode->next=nullptr;
	newNode->prev=nullptr;
	
	//return constructed node
	return newNode;
}

void nodeDestructor(struct ValuesNode* node)
{
	free(node);
}

struct ValuesList* listConstructor()
{
	struct ValuesList* newList;
	
	//reserve memory for the list
	newList->size=0;
	newList->head=nullptr;
	
	//return constructed list
	return newList;
}

/// FUNCTIONS FOR CREATING LIST
/* We will have a button that stores a new value once pressed
every mode (I,R,V) will pass a different unit (I->Amp,R->Ohm,V->Volt,etc.)
we want this function to add the new value at the start of the list and when we print 
it we are going to print it from the last value we stored having the ability to go to the next and prev
we can also go from the first to last because it is going to be a doubly linked circular list */

void storeValue(struct ValuesList* list, char string[])
{
	struct ValuesNode* newNode;
	struct ValuesNode* tmpNode; //node to be destructed if needed
	
	//incrementing the size of the list
	ValuesList->size++;
	
	//setting the new node
	newNode=nodeConstructor(string,ValuesList->size);
	
	//if list is empty
	if(ValuesList->head==nullptr)
	{
		list->head=newNode;
		newNode->next=newNode;	//to help with 
		newNode->prev=newNode;	//overall logic
	}
	//if list isn't empty
	else
	{
		//previous node of current head connection
		//with newNode
		newNode->prev=list->head->prev;
		list->head->prev->next=newNode;
		
		//current head connection with newNode
		newNode->next=list->head;
		list->head->prev=newNode;
		
		//changing head to newNode
		list->head=newNode;
	}
	
	//if size of list is bigger than 10 then delete the first value that was stored
	//so that the heap and the stack will not crash 
	if(ValuesList->size>10)
	{
		//setting tmpNode to the node that was saved earlier
		tmpNode=newNode->prev;
		
		//changing the connection in the list
		tmpNode->prev->next=tmpNode->next;
		newNode->prev=tmpNode->prev;
		
		//destroying the node
		nodeDestructor(tmpNode);
	}
	//print at LCD "VALUE STORED"
	PB_LCD_Clear();
	PB_LCD_WriteString("Value Stored",16);
	wait1sec();
}

void printLCD(struct ValuesNode* DisplayedNode)
{
	//char lcd1[20];
	//sprintf(lcd1,???);	maybe add the number of element of the list 1_2_.._10
	PB_LCD_Clear();
	PB_LCD_WriteString(DisplayedNode->stringStored,16);
	//PB_LCD_GoToXY(0, 1);
	//PB_LCD_WriteString(???, 16);
}
void moveToNextNode(struct ValuesNode* DisplayedNode)
{
	DisplayedNode=DisplayedNode->next;
}

void moveToPrevNode(struct ValuesNode* DisplayedNode)
{
	DisplayedNode=DisplayedNode->prev;
}

//////FUNCTION TO CREATE A WAVE NEEDED FOR THE BEEPER
void beep()
{
	GPIOB -> MODER (GPIOB -> MODER & 0xFFFFFFFC) | 0x00000001;
	GPIOB -> OTYPER (GPIOB -> OTYPER & 0xFFFFFFFE);
	GPIOB -> PUPDR (GPIOB -> PUPDR & 0xFFFFFFFC) | 0x00000002;
	GPIOB -> BSRR (GPIOB -> BSRR & 0xFFFFFFFE) | 0x00000001;
	wait3sec();
	GPIOB -> BSRR (GPIOB -> BSRR & 0xFFFEFFFF) | 0x00010000;
}

// tom said we cant work at the systick by dividing with values really big like 60M
// so maybe have a function that will run many smaller systick till we achieve the 3 sec 
// we want
void wait1sec()
{
	int counter=0;
	//refreshDisplay changes at a frequency of 8mega
	//so we need to just count 8 m times to reach a second (?)
	while (counter<2)
	{
		if(refreshDisplay==1)
		{
			counter++;
			refreshDisplay=0;
		}
	}

}
void wait3sec()
{
	wait1sec();
	wait1sec();
	wait1sec();
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
	struct ValuesList* list=listConstructor;
	while(1)
		{
			
			Vfunc(list); //mode 0
			Rfunc(list); //mode 1
			Ifunc(list); //mode 2
			Ffunc(list); //mode 3
			Stored_func(list); //mode 4
		}
} 
