#include "STM32F407xx.h"
#include "Board_LED.h"
#include "PB_LCD_Drivers.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define nullptr NULL

int sahOn = 0;	
int LEDState;
int refreshDisplay = 1;
int mode = 0;
int store = 0;
int ohmMode = 0;
int nextNode = 0;
int prevNode = 0;
int freq;
float voltsSaHMax; //Max sample and hold value
float voltsSaHMin; //Min sample and hold value
float valueGivenByADC(void); //Predefinition
void waitForSeconds(int seconds);
float systemTime;
int rFlag=0;

void EXTI15_10_IRQHandler (void) { 
/* Interrupt request handler for SW8 and SW9. Must be called before switches can work*/
	if (GPIOE -> IDR &= 0x00004000){
		EXTI -> PR = EXTI_PR_PR14;
		
		//Rotate through the 6 modes, V, I, little R, big R, frequency and store/recall.
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
		if (mode==2)
		{
			char msg[15];
			ohmMode++;	
			rFlag=1;
			if (ohmMode == 9)
			{
				ohmMode = 0;
			}
			snprintf(msg,15,"Mode %i",ohmMode);
			PB_LCD_Clear();
			PB_LCD_WriteString(msg,16);
	}
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

void EXTI9_5_IRQHandler (void) { 
	EXTI->PR = EXTI_PR_PR6;
	freq=freq+1;
}

void waitForSeconds(int seconds){
	systemTime = 0;
	int currentMode = mode;
	while(systemTime != seconds && currentMode == mode); 
}


void beepInit()
{
	RCC -> AHB1ENR = (RCC->APB1ENR & 0xFFFFFFFD) | 0x00000002;
	GPIOB -> MODER = (GPIOB -> MODER & 0xFFFFFF3F) | 0x00000040;
	GPIOB -> OTYPER = (GPIOB -> OTYPER & 0xFFFFFFF7);
	GPIOB -> PUPDR = (GPIOB -> PUPDR & 0xFFFFFF3F) | 0x00000080;
}
void beep()
{
	GPIOB -> BSRR = (GPIOB -> BSRR & 0xFFFFFFF7) | 0x00000008;
	waitForSeconds(3);
	GPIOB -> BSRR = (GPIOB -> BSRR & 0xFFF7FFFF) | 0x00080000;
}
void rBig()
{
	PB_LCD_Clear();
	PB_LCD_WriteString("Change Mode",16);
	PB_LCD_GoToXY(0, 1);
	PB_LCD_WriteString("R too large", 16);
	waitForSeconds(2);
}
void rSmall()
{
	PB_LCD_Clear();
	PB_LCD_WriteString("Change Mode",16);
	PB_LCD_GoToXY(0, 1);
	PB_LCD_WriteString("R too small", 16);
	waitForSeconds(2);
}
void ohmModeCheck(float volts)
{
		switch(ohmMode)
	{
		case 0:
			//if(volts>4.5492)
			//{
				//rBig();
			//}
			break;
		case 1:
			if(volts>9.811)
			{
				rBig();
			}
			else if(0.1<volts<0.9983)
			{
				rSmall();
			}
			break;
		case 2:
			if(volts>2.6471)
			{
				rBig();
			}
			else if(0.1<volts<0.995)
			{
				rSmall();
			}
			break;
		case 3: 
			if(volts>1.7543)
			{
				rBig();
			}
			else if(0.1<volts<1.3236)
			{
				rSmall();
			}
			break;
		case 4:
			if(volts>1.2432)
			{
				rBig();
			}
			else if(0.1<volts<0.8131)
			{
				rSmall();
			}
			break;
		case 5:
			if(volts>0.9983)
			{
				rBig();
			}
			else if(0.1<volts<0.7864)
			{
				rSmall();
			}
			break;
		case 6:
			if(volts>0.978)
			{
				rBig();
			}
			else if(0.1<volts<0.8222)
			{
				rSmall();
			}
			break;
		case 7:
			if(volts>0.9846)
			{
				rBig();
			}
			else if(0.1<volts<0.8906)
			{
				rSmall();
			}
			break;
		case 8:
			if(volts>0.9432)
			{
				rBig();
			}
			else if(0.1<volts<0.9176)
			{
				rSmall();
			}
			break;
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

void frequencyMeasure (){
	waitForSeconds(2);
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
		outputValue = ((volts - 1.5f)/0.15f - 0.2f);
	}
	else if (unit == 'A') //Amps
	{
		outputValue = ((volts - 1.5f)/0.15f)/10.05f;
	}
	else if (unit == (char)222) //Ohms
	{
		outputValue = (volts*995.8f)/(5-volts);
	}
	else if (unit == 'z') // Frequency
	{
		freq = 0;
		frequencyMeasure();
		outputValue=freq/1.023;
	}
		
	//Autoranging
	if (fabs(outputValue) >= 1 && unit != 'z') //Fabs gives absolute value of float, polarity of number is irrelevant 
	{
		snprintf (stringOut, 10, "%.3f", outputValue);
	}
	else if (unit != 'z')
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
	if (unit == 'z'){
		snprintf (stringOut, 10, "%f", outputValue);
		strcat(stringOut, "H");
	}
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
		case 3	:
			return 'z';
		}
	//If mode is ever equal to or greater than 3, you've broken something
	return 'X';
}


 
 void SysTick_Handler (void){
/*Many times a second, this is called*/
	refreshDisplay = 1; //Claims that the display is ready to be refreshed
	systemTime++;
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

void displayModeMessage(){
	PB_LCD_Clear();
	if (mode == 0){
			PB_LCD_WriteString("Voltmeter",16);
			//PB_LCD_GoToXY(0, 1);
		  //PB_LCD_WriteString("Accuracy:", 16); //ADD THE VALUE
	}
	else if (mode == 1){
			PB_LCD_WriteString("Ammeter",16);
			//PB_LCD_GoToXY(0, 1);
			//PB_LCD_WriteString("Accuracy:", 16); //ADD THE VALUE
	}
	else if (mode == 2){
			PB_LCD_WriteString("Resistance",16);
			//PB_LCD_GoToXY(0, 1);
			//PB_LCD_WriteString("Accuracy", 16); //ADD THE VALUE
	}
	else if (mode == 3){
			PB_LCD_WriteString("Frequency",16);
			//PB_LCD_GoToXY(0, 1);
			//PB_LCD_WriteString("Accuracy", 16); //ADD THE VALUE
	}
	else if (mode == 4){
			PB_LCD_WriteString("Stored values",16);
			//PB_LCD_GoToXY(0, 1);
			//PB_LCD_WriteString("Accuracy", 16); //ADD THE VALUE
	}
	waitForSeconds(3);
}

///////  lvl 3 requirement  //////
/// STRUCTURE TO STORE VALUES
struct ValuesList
{
	struct ValuesNode* head;
	int size;
};
struct ValuesNode
{
	char stringStored[16];
	int readingnumber;
	struct ValuesNode* next;
	struct ValuesNode* prev;
};
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
	newList = malloc(sizeof(struct ValuesList)); ////////////////////////////////error 1 forgot to allocate memory
	
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
	list->size++;
	
	//setting the new node
	newNode=nodeConstructor(string,list->size);
	
	//if list is empty
	if(list->head==nullptr)
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
	if(list->size>10)
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
	waitForSeconds(1);
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

void Stored_func(struct ValuesList* list)
{
	//current mode and number of stored values(? no) (displayed for 3sec)
	PB_LCD_Clear();
	PB_LCD_WriteString("Stored Values",13);
	
	//wait statement
	waitForSeconds(3);
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
				PB_LCD_WriteString("Nothing Stored",16);
			}
			else
			{
				printLCD(displayedNode);
				if(nextNode==1 && list->size!=1)
				{
					nextNode=0;
					displayedNode=displayedNode->next; // this fucntion is really simple do we really need it ?
				}
				if(prevNode==1 && list->size!=1)
				{
					prevNode=0;
					displayedNode=displayedNode->prev; // this fucntion is really simple do we really need it ?
				}
			}
		refreshDisplay = 0;
		}
	}
	nextNode=0;
	prevNode=0;
	store=0; // so that if the store button is pressed inside here it will be reseted automatically before the next function that actually uses it
}

void displayValue(struct ValuesList* list){
/* Takes a volt value from ADC and displays it on the LCD 
 * Only performs string and LCD operations, all convertions and calculates happen in arvStringfromValue*/

	char LCD_out[15];
	char LCD_minmax[15];
	char unit = unitOfMode();
	float voltsADC = valueGivenByADC();
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
	
	//Store value
	if(store==1)
	{	
		storeValue(list, LCD_out);  /////////////////////changed that as well
		store=0;
	}
	
	//if the user tries to measure a value close to the max beep thrice (:P three times)
	int myFlag=0;
	if(fabs(valueGivenByADC())>9.5 && myFlag==0)
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
	if(fabs(valueGivenByADC())<9.3 && myFlag==1) 
	{
		//set the flag low
		myFlag=0;
	}
}
void Rfunc(struct ValuesList* list)
{
	//int CurrentOhmMode=0;
	char LCD_out[15];
	//char msg[15];
	char LCD_minmax[15];
	float volts;
	
	//while(mode==2)
	//{
	/*	if (ohmMode==CurrentOhmMode)
			{
				snprintf(msg,15,"Mode %i",ohmMode);
				PB_LCD_Clear();
				PB_LCD_WriteString(msg,16);
				waitForSeconds(2);
				if(ohmMode==8)
				{
					CurrentOhmMode=0;
				}
				else
				{
					CurrentOhmMode=ohmMode+1;
				}
			}*/
		volts =(valueGivenByADC()*995.8f)/(5-valueGivenByADC());
		if (rFlag==1)
		{
			rFlag=0;
			waitForSeconds(2);
		}
		ohmModeCheck(valueGivenByADC());
		if (refreshDisplay==1)
		{
			refreshDisplay=0;
			if (ohmMode == 0)
			{
				if (sahOn == 1)
				{
					voltsSaHMax = compReturnHigh(volts, voltsSaHMax);
					voltsSaHMin = compReturnLow(volts, voltsSaHMin);
				}
				
				//Pass the volt value to arvStringfromValue() and collect the string it returns.
				snprintf(LCD_out, 10, "%0.3f%c", volts,(char)222);
				

				PB_LCD_Clear();
				PB_LCD_WriteString(LCD_out, 16);
				
				if (sahOn == 1)
				{
					snprintf(LCD_minmax, 15, "%0.3f%c %0.3f%c", voltsSaHMin,(char)222,voltsSaHMax,(char)222);

					PB_LCD_GoToXY(0, 1);
					PB_LCD_WriteString(LCD_minmax, 16);
				}
			}
			else
			{
				if (sahOn == 1)
				{
					voltsSaHMax = compReturnHigh(volts, voltsSaHMax);
					voltsSaHMin = compReturnLow(volts, voltsSaHMin);
				}
				
				//Pass the volt value to arvStringfromValue() and collect the string it returns.
				snprintf(LCD_out, 10, "%0.3fK%c", volts,(char)222);

				PB_LCD_Clear();
				PB_LCD_WriteString(LCD_out, 16);
				
				if (sahOn == 1)
				{
					snprintf(LCD_minmax, 15, "%0.2fK%c %0.2fK%c", voltsSaHMin,(char)222,voltsSaHMax,(char)222);

					PB_LCD_GoToXY(0, 1);
					PB_LCD_WriteString(LCD_minmax, 16);
				}
				
			}
		}
		
		if(store==1)
		{	
			storeValue(list, LCD_out);  /////////////////////changed that as well
			store=0;
		}

	//}
}
int main (void) {
	//Something to do with interrupts, ask Tom, not me.
	RCC -> AHB1ENR = (RCC->APB1ENR & 0xFFFFFFE2) | 0x0000001D;
		
	RCC -> APB2ENR = (RCC -> APB2ENR & 0xFFFFBFFE) | 0x00004001;

	GPIOE -> MODER = (GPIOE -> MODER & 0x0000CFFF) | 0x00000000;
		
	GPIOE -> PUPDR = (GPIOE -> PUPDR & 0x0000CFFF) | 0xAAAA1000;
	
	SYSCFG -> EXTICR[1] = (SYSCFG -> EXTICR[1] & 0xFFFFFCFF) | 0x00000400;
	
	SYSCFG -> EXTICR[2] = (SYSCFG -> EXTICR[2] & 0xFFFF0000) | 0x00004444;
			
	SYSCFG -> EXTICR[3] = (SYSCFG -> EXTICR[3] & 0xFFFF0000) | 0x00004444;
	
	EXTI -> IMR = (EXTI -> IMR & 0xFFFF00BF) |	0x0000FF40;
		
	EXTI -> RTSR = (EXTI -> RTSR & 0xFFFF00BF) |	0x0000FF40;

	EXTI -> FTSR = (EXTI -> FTSR & 0xFFFF00BF);
	
	// My code starts here
	SystemCoreClockUpdate();
	initialiseAF();
	ADC_Initialise ();
	PB_LCD_Init();
	LED_Initialize();
	beepInit();
	SysTick_Config(SystemCoreClock/2); 
	int currentMode = 11;
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	EXTI15_10_IRQHandler ();
	EXTI9_5_IRQHandler ();
	struct ValuesList* list=listConstructor();
	list->size=0;
	
	while(1)
	{
		if (refreshDisplay == 1)
		{
			if (currentMode != mode)
			{
				displayModeMessage();
			}
			if (mode != 2 && mode != 4)
			{
				displayValue(list);
			}
			else if (mode == 2)
			{
				Rfunc(list);
			}
			else if (mode == 4)
			{
				Stored_func(list);
			}
			currentMode = mode;
			refreshDisplay = 0;
		}
	}
} 
