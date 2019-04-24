#define cs 15
#define nullptr ((void*)0)

//for storing I'll use joe's   char* arvStringfromValue(float volts, char unit)   function
//for valueTHATisDISPLAYED to check when to beep use Joe's function valueGivenByADC() and then do the math needed
//refreshDisplay = 0; and when it is needed (?)
/*while(1)
	{
		if (refreshDisplay == 1)
		{
			displayValue(valueGivenByADC());
			refreshDisplay = 0;
		}
	}
*/ 
//Needs to change for our codes to join 
//I though it would look something like this
/*
struct ValuesList* list=listConstructor;
while(1)
	{
		
		Vfunc(list); //mode 0
		Rfunc(list); //mode 1
		Ifunc(list); //mode 2
		Ffunc(list); //mode 3
		Stored_func(list); //mode 4
	}
*/ 
//and everything would happen in those functions
//happy to change it if you explain me how

int mode = 0;
int store = 0;
int ohmMode = 0;
int nextNode = 0;
int prevNode = 0;

void EXTI15_10_IRQHandler (void) { 
	/*JOES
	if (GPIOE -> IDR &= 0x00008000){
		EXTI -> PR = EXTI_PR_PR15;
		//enter here what you want to do when SW9 is pressed
	}
	else if (GPIOE -> IDR &= 0x00004000){ //we need to reset it at 5 now because we have 5 modes
		EXTI -> PR = EXTI_PR_PR14;
		
		//Rotate through the 5 modes, V, I, R, Frequency and Store
		mode++;	
		if (mode == 5)
		{
			mode = 0;
		}
	} */
	else if (GPIOE -> IDR &= 0x00002000){ //store button
		EXTI -> PR = EXTI_PR_PR13;
		store = 1;
	}
	else if (GPIOE -> IDR &= 0x00001000){ //ohmMode button
		EXTI -> PR = EXTI_PR_PR12;
		ohmMode=1-ohmMode;//swap between 0 <--> 1 
	}
	else if (GPIOE -> IDR &= 0x00000800){//next node button
		EXTI -> PR = EXTI_PR_PR11;
		nextNode=1;
	}
	else if (GPIOE -> IDR &= 0x00000400){//prev node button
		EXTI -> PR = EXTI_PR_PR10;
		prevNode=1;
	}
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

		//function that takes a volt value from 0-3 Volt
		//and returns a value from -10 to 10 V
		
		//if the user wants to store a value store the value
		if(store==1)
		{	
			store=0;
			storeValue(list,arvStringfromValue(VOLTS COMING IN ??,unitOfMode()));
		}
		
		//if the user tries to measure a value close to the max beep thrice (:P three times)
		if(abs(valueTHATisDISPLAYED)>9.5 && myFlag==0)
		{
			
			//briefly display what the problem is ?
			PB_LCD_Clear();
			PB_LCD_WriteString("Value measured",16);
			PB_LCD_GoToXY(0, 1);
			PB_LCD_WriteString("is close to max", 16);
			
			//beep
			//beep
			//beeeeeep
			beep();
			//do we need other wait ?			

			//set the flag high
			myFlag=1;
		}
		//reset the flag when the value drops lower 
		if(abs(valueTHATisDISPLAYED)<9.3 && myFlag==1) 
		{
			//set the flag low
			myFlag=0;
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

		while(ohmMode==0)
		{
			//we will check A pin and display at lcd the value we think the resistor has
			
			//if the user wants to store a value store the value
			if(store==1)
			{
				store=0;
				storeValue(list,arvStringfromValue(VOLTS COMING IN ??,unitOfMode()));
			}
		}
		while(ohmMode==1)
		{
			//we will check B pin and display at lcd the value we think the resistor has
			
			//if the user wants to store a value store the value
			if(store==1)
			{
				store=0;
				storeValue(list,arvStringfromValue(VOLTS COMING IN ??,unitOfMode()));
			}
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
		//function that takes a voltage value for low current
		//and returns the current value
		
		//if the user wants to store a value store the value
		if(store==1)
		{
			store=0;
			storeValue(list,arvStringfromValue(VOLTS COMING IN ??,unitOfMode()));
		}
		
		//if the user tries to measure a value close to the max beep thrice (:P three times)
		if(abs(valueTHATisDISPLAYED)>0.95 && myFlag==0)
		{
			//beep
			//beep
			//beeeeeep
			
			//briefly display what the problem is ?
			PB_LCD_Clear();
			PB_LCD_WriteString("Value measured",16);
			PB_LCD_GoToXY(0, 1);
			PB_LCD_WriteString("is close to max", 16);
			
			beep();
			
			wait1sec();//function beep beeps for 3 sec so we are already waiting we probs dont need this waits here
			wait1sec();

			//set the flag high
			myFlag=1;
		}
		//reset the flag when the value drops lower 
		if(abs(valueTHATisDISPLAYED)<0.93 && myFlag==1)
		{
			//set the flag low
			myFlag=0;
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
		//rising edge/time = freq 
		//display it 
		
		//if the user wants to store a value store the value
		if(store==1)
		{
			store=0;
			storeValue(list,arvStringfromValue(VOLTS COMING IN ??,unitOfMode()));
		}
		
		//if the user tries to measure a value close to the max beep thrice (:P three times)
		if(abs(valueTHATisDISPLAYED)>9.5 && myFlag==0)
		{
			//beep
			//beep
			//beeeeeep
			
			//briefly display what the problem is ?
			//this code has been reapeted maybe make a func for it ?
			PB_LCD_Clear();
			PB_LCD_WriteString("Value measured",16);
			PB_LCD_GoToXY(0, 1);
			PB_LCD_WriteString("is close to max", 16);
			
			beep();
			
			wait1sec(); // ?
			wait1sec(); // ?
			
			//set the flag high
			myFlag=1;
		}
		//reset the flag when the value drops lower 
		if(abs(valueTHATisDISPLAYED)<9.3 && myFlag==1)
		{
			//set the flag low
			myFlag=0;
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
	int counter=0;
	//we want a 10k frequency => 0.0001 period
	//systick has an f=60MHz
	// 60M/6k = 10k
	while(counter<30000) //when counter==30000 3 sec have passed counter when counter==1 0.00005+0.00005=0.0001 sec
	{
		//send high signal (+2.5)
		//wait for 0.00005 (sysSOMETHINGhandler?)
		//send low signal  (-2.5)
		//wait for 0.00005 (sysSOMETHINGhandler?)
		counter++;
	}
}

// tom said we cant work at the systick by dividing with values really big like 60M
// so maybe have a function that will run many smaller systick till we achieve the 3 sec 
// we want
void wait1sec()
{
	//systick()/30M ~ 0.5 sec // we can brake it further down if the value is 2 big
	//systick()/30M ~ 0.5 sec
}
void wait3sec()
{
	wait1sec();
	wait1sec();
	wait1sec();
	//= 3 sec
}
