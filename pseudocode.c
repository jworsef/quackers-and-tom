
int checkButton(inputFromButton)
{
		if(inputFromButton goes high)
		{
			while(1)
			{
				if(inputFromButton goes low)
				{
					return 1;
				}	
			}
		}
		return 0;
}

void Vfunc()
{
	//Voltmeter Mode and accuracy (displayed for 3sec)
	
	while(1)
	{

		//function that takes a volt value from 0-3 Volt
		//and returns a value from -10 to 10 V
		
		if(checkButton(changeMenuButton)==1)
		{
			break;
		}
	}
}

void Rfunc()
{
	//Ohmmeter Mode and accuracy (displayed for 3sec)
	int Mode=0;
	while(1)
	{
		if(checkButton(changeModeButton)==1)
		{
			Mode~; // inverting mode which is one bit and can go from zero to 1
		}
		
		if(Mode==0)
		{
			//we will check A pin and display at lcd the value we think the resistor has
		}
		else if(Mode==1)
		{
			//we will check B pin and display at lcd the value we think the resistor has
		}
		
		
		if(checkButton(changeMenuButton)==1)
		{
			break;
		}
	}
}	
