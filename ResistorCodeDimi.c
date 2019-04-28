void Rfunc(struct ValuesList* list)
{
	int CurrentOhmMode=-1;
	char LCD_out[15];
	char msg[15];
	char LCD_minmax[15];
	char buffer[6];
	float volts;
	while(mode==2)
		if (ohmMode!=CurrentOhmMode)
		{
			snprintf(msg,15,"Mode %i",ohmMode);
			PB_LCD_Clear();
			PB_LCD_WriteString(msg,16);
			waitForSeconds(2);
		}
		CurrentOhmMode=ohmMode;
		volts =(valueGivenByADC()*995.8f)/(5-valueGivenByADC());
		ohmModeCheck(volts);
		if (ohmMode == 0)
		{
			if (sahOn == 1)
			{
				voltsSaHMax = compReturnHigh(volts, voltsSaHMax);
				voltsSaHMin = compReturnLow(volts, voltsSaHMin);
			}
			
			//Pass the volt value to arvStringfromValue() and collect the string it returns.
			snprintf(LCD_out, 10, "%f", volts);
			strcat(LCD_out, (char)222);
			

			PB_LCD_Clear();
			PB_LCD_WriteString(LCD_out, 16);
			
			if (sahOn == 1)
			{
				snprintf(LCD_minmax, 15, "%f", voltsSaHMin);
				strcat(LCD_minmax, (char)222);
				strcat(LCD_minmax," ");
				snprintf(buffer, 15, "%f", voltsSaHMax);
				strcat(LCD_minmax, buffer);
				strcat(LCD_minmax, (char)222);

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
			snprintf(LCD_out, 10, "%fK", volts);
			strcat(LCD_out, (char)222);

			PB_LCD_Clear();
			PB_LCD_WriteString(LCD_out, 16);
			
			if (sahOn == 1)
			{
				snprintf(LCD_minmax, 15, "%fK", voltsSaHMin);
				strcat(LCD_minmax, (char)222);
				strcat(LCD_minmax," ");
				snprintf(buffer, 15, "%fK", voltsSaHMax);
				strcat(LCD_minmax, buffer);
				strcat(LCD_minmax, (char)222);

				PB_LCD_GoToXY(0, 1);
				PB_LCD_WriteString(LCD_minmax, 16);
			}
			
		}
		
		if(store==1)
		{	
			storeValue(list, LCD_out);  /////////////////////changed that as well
			store=0;
		}
		
}

void rBig()
{
	B_LCD_Clear();
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
			if(volts>4.5492)
			{
				rBig();
			}
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
