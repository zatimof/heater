#include "..\heat.h"

extern int temp[4],adc[4],temp_set,temp_hyst,temp_corr;
extern unsigned char menu,blink,ram_data[RAM_ITEM_MAX],valid,state,state_code,eco,delay,repeat;
extern unsigned long on_time,off_time,idle_time;

void logic(void)
{
	unsigned char i;
	
	for(i=0;i<4;i++)
			temp[i]=(adc[i]>>1)-273;
	
	if(valid&0x08)								//outdoor temp correction
		if(THRESHOLD_TEMP>temp[3])
		{
			i=THRESHOLD_TEMP-temp[3];
			if((temp_corr*STEP_CORR_TEMP)<(i-STEP_CORR_TEMP))
			{
				temp_corr++;
				if(temp_corr>MAX_CORR_TEMP)
					temp_corr=MAX_CORR_TEMP;
			}
			if((temp_corr*STEP_CORR_TEMP)>(i+STEP_CORR_TEMP))
				temp_corr--;
		}
		else
			temp_corr=0;
	else
		temp_corr=0;
	
	temp_set+=temp_corr;
	
	if((valid&0x03)==0x03)
		state_code&=0xBF;	
	else	
		state_code|=0x40;
	
	if(temp[0]<MIN_INP_TEMP)
		state_code|=0x10;
	else	
		state_code&=0xEF;	
	
	if(temp[1]>MAX_OUT_TEMP)
		state_code|=0x08;
	else
		state_code&=0xF7;	
	
	switch(state)
	{
		case 0:	//Stop
			HEAT_OFF;
			PUMP_OFF;
			
			if((!(state_code&0x40))&&(!(state_code&0x10))&&(!(state_code&0x02)))
				if(!delay)
				{
					state=1;					
					delay=TIME_OUT;					
				}
		
			if(PROTECT)
				state_code|=0x80;
			else
				state_code&=0x7F;	
			break;
				
		case 1:	//Idle
			PUMP_ON;
			HEAT_OFF;
			
			off_time++;
			
			if((state_code&0x40)||(state_code&0x10)||(state_code&0x02))
				state=0;

			if(((temp[0]+temp_hyst)<temp_set)&&(!delay)&&(!(state_code&0x08)))
			{
				state=2;
				delay=TIME_OUT;
			}
			
			if((temp[0]>(temp_set-temp_hyst))&&(temp[0]<(temp_set+temp_hyst))&&(!delay)&&(!idle_time)&&(!(state_code&0x08)))
			{
				state=2;
				delay=TIME_OUT;
			}			
			
			if(PROTECT)
				state_code|=0x80;
			else				
				state_code&=0x7F;	
			
			if((temp[1]-temp[0])>MAX_DIFF_TEMP)
			{
				state_code|=0x20;
				state=4;
			}
			else
				state_code&=0xDF;
			break;
		
		case 2:	//Start_heat
			PUMP_ON;
			HEAT_ON;
			
			if(state_code&0x08)
			{
				state=1;
				delay=TIME_OUT;	
			}
			
			if((temp[1]-temp[0])<MIN_DIFF_TEMP)
			{
				if(!delay)
				{
					state=1;
					delay=TIME_OUT;	
					repeat--;
					if(!repeat)
						state=4;
				}
				state_code|=0x04;
			}
			else
			{
				state=3;
				delay=TIME_OUT;
				state_code&=0xFB;
			}

			if(PROTECT)
				state_code&=0x7F;
			else	
			{
				if(!delay)
					state=4;
				state_code|=0x80;
			}
			
			if((temp[1]-temp[0])>MAX_DIFF_TEMP)
			{
				state_code|=0x20;
				state=4;
			}
			else
				state_code&=0xDF;
			
			if(state_code&0x40)
				state=4;
			
			break;	
			
		case 3:	//Heat
			PUMP_ON;
			HEAT_ON;
			
			on_time++;
			
			if((temp[0]>(temp_set+temp_hyst))||(state_code&0x08))
			{
				state=1;
				idle_time=NUM_CYCLES_TIMEOUT;
				delay=TIME_OUT;
			}
			
			if(temp[1]>MAX_OUT_TEMP)
			{
				state_code|=0x08;
				state=4;
			}
			else
				state_code&=0xF7;		
			
			if((temp[1]-temp[0])>MAX_DIFF_TEMP)
			{
				state_code|=0x20;
				state=4;
			}
			else
				state_code&=0xDF;		
			
			if((temp[1]-temp[0])<MIN_DIFF_TEMP)
			{
				if(!delay)
				{
					state=1;
					idle_time=NUM_CYCLES_TIMEOUT;
					delay=TIME_OUT;	
				}
				state_code|=0x01;
			}
			else
				state_code&=0xFE;		
			
			if(PROTECT)
				state_code&=0x7F;
			else	
			{
				if(!delay)
					state=4;
				state_code|=0x80;
			}
			
			if(state_code&0x40)
				state=4;
			break;
			
		case 4:	//Error
			PUMP_ON;
			HEAT_OFF;
			BEEP_ON;
			break;
		
		default:
			break;
	}
	
	if(delay)
		delay--;
	
	if(idle_time)
		idle_time--;
	
	return;
}