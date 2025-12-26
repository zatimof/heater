#include "..\heat.h"

extern unsigned char btn1, btn2, btn3, btn4;
extern unsigned char disp_on, menu, ram_data[RAM_ITEM_MAX];
extern int value;

void read_keyb(void)
{
//menu button	
	if(BUTTON1)
		btn1 = 0;
	else
		if((btn1 == 0) || (btn1 == 1))
		{
			if((btn1 == 0) || (!menu))
			{
				BEEP_ON;										//beep on
				__delay_cycles(100000);
				BEEP_OFF;										//beep off
			}
						
			if((btn1 == 0) && (!menu))
				btn1 = NCYCLE + NCYCLE;
			if((btn1 == 0) && menu)
			{
				menu++;
				if(menu > MENU_ITEM_MAX)
					menu = 1;
				btn1 = 1;
			}
			if((btn1 == 1) && (!menu))
				menu = 1;
			
			disp_on = DISP_TIME_ON;
		}
		else
			btn1--;

//up button	
	if(BUTTON2)
		btn2 = 0;
	else
		if((btn2 == 0) || (btn2 == 1))
		{
			if(btn2 == 0)
				btn2 = NCYCLE;
			BEEP_ON;										//beep on
			__delay_cycles(100000);
			BEEP_OFF;										//beep off

			switch(menu)
			{
				case 1: 
					ram_data[8]++;
					if(ram_data[8] > 99)
						ram_data[8] = 99;
					ram_data[10] = ram_data[8];
					break;
					
				case 2:
					ram_data[9]++;
					if(ram_data[9] > 9)
						ram_data[9] = 9;
					ram_data[11] = ram_data[9];
					break;
					
				case 3:
					ram_data[2]++;
					ram_data[2] &= 0x3F;					
					if(((ram_data[2] & 0x0F) > 9) && ((ram_data[2] & 0xF0) != 0x20))
					{
						ram_data[2] &= 0xF0;
						ram_data[2] += 0x10;
					}
					if((ram_data[2] & 0x3F) > 0x23)
						ram_data[2] = 0x00;
					break;
					
				case 4:
					ram_data[1]++;
					ram_data[1] &= 0x7F;					
					if(((ram_data[1] & 0x0F) > 9) && ((ram_data[1] & 0xF0) != 0x60))
					{
						ram_data[1] &= 0xF0;
						ram_data[1] += 0x10;
					}
					if((ram_data[1] & 0x7F) > 0x59)
						ram_data[1] = 0x00;
					break;
					
				case 5:
					ram_data[0]++;
					ram_data[0] &= 0x7F;					
					if(((ram_data[0] & 0x0F) > 9) && ((ram_data[0] & 0xF0) != 0x60))
					{
						ram_data[0] &= 0xF0;
						ram_data[0] += 0x10;
					}
					if((ram_data[0] & 0x7F) > 0x59)
						ram_data[0] = 0x00;
					break;
					
				case 6:
					ram_data[3]++;
					if(ram_data[3] > 7)
						ram_data[3] = 7;
					break;
									
				case 7:
					ram_data[4]++;
					ram_data[4] &= 0x3F;					
					if(((ram_data[4] & 0x0F) > 9) && ((ram_data[4] & 0xF0) != 0x30))
					{
						ram_data[4] &= 0xF0;
						ram_data[4] += 0x10;
					}
					if((ram_data[4] & 0x3F) > 0x31)
						ram_data[4] = 0x01;
					break;
					
				case 8:
					ram_data[5]++;
					ram_data[5] &= 0x1F;					
					if(((ram_data[5] & 0x0F) > 9) && ((ram_data[5] & 0xF0) != 0x10))
					{
						ram_data[5] &= 0xF0;
						ram_data[5] += 0x10;
					}
					if((ram_data[5] & 0x1F) > 0x12)
						ram_data[5] = 0x01;
					break;
									
				case 9:
					ram_data[6]++;
					if(((ram_data[6] & 0x0F) > 9) && ((ram_data[6] & 0xF0) != 0x90))
					{
						ram_data[6] &= 0xF0;
						ram_data[6] += 0x10;
					}
					if(ram_data[6] > 0x99)
						ram_data[6] = 0x00;
					break;
					
				default:
					break;				
			}
			
			disp_on = DISP_TIME_ON;
		}
		else
			btn2--;

//down button	
	if(BUTTON3)
		btn3 = 0;
	else
		if((btn3 == 0) || (btn3 == 1))
		{
			if(btn3 == 0)
				btn3 = NCYCLE;
			BEEP_ON;										//beep on
			__delay_cycles(100000);
			BEEP_OFF;										//beep off

			switch(menu)
			{
				case 1: 
					ram_data[8]--;
					if(ram_data[8] == 255)
					{
						ram_data[8] = 0;
					}
					ram_data[10] = ram_data[8];
					break;
					
				case 2:
					ram_data[9]--;
					if(ram_data[9] == 0)
					{
						ram_data[9] = 1;
					}
					ram_data[11] = ram_data[9];
					break;
					
				case 3:
					ram_data[2]--;
					ram_data[2] &= 0x3F;					
					if((ram_data[2] & 0x0F) > 9)
						if((ram_data[2] & 0x30) == 0x30)
							ram_data[2] = 0x23;
						else
						{
							ram_data[2] &= 0x30;
							ram_data[2] |= 0x09;
						}
					break;
					
				case 4:
					ram_data[1]--;
					ram_data[1] &= 0x7F;					
					if((ram_data[1] & 0x0F) > 9)
						if((ram_data[1] & 0x70) > 0x50)
							ram_data[1] = 0x59;
						else
						{
							ram_data[1] &= 0x70;
							ram_data[1] |= 0x09;
						}
					break;
									
				case 5:
					ram_data[0]--;
					ram_data[0] &= 0x7F;					
					if((ram_data[0] & 0x0F) > 9)
						if((ram_data[0] & 0x70) > 0x50)
							ram_data[0] = 0x59;
						else
						{
							ram_data[0] &= 0x70;
							ram_data[0] |= 0x09;
						}
					break;
					
				case 6:
					ram_data[3]--;
					if(ram_data[3] == 0)
						ram_data[3] = 1;
					break;
									
				case 7:
					ram_data[4]--;
					ram_data[4] &= 0x3F;					
					if((ram_data[4] & 0x0F) > 9)
					{
						ram_data[4] &= 0x30;
						ram_data[4] |= 0x09;
					}
					if(ram_data[4] == 0x00)
						ram_data[4] = 0x31;
					break;
					
				case 8:
					ram_data[5]--;
					ram_data[5] &= 0x1F;					
					if((ram_data[5] & 0x0F) > 9)
					{
						ram_data[5] &= 0x10;
						ram_data[5] |= 0x09;
					}
					if(ram_data[5] == 0x00)
						ram_data[5] = 0x12;
					break;
									
				case 9:
					ram_data[6]--;
					if((ram_data[6] & 0x0F) > 9)
						if(ram_data[6] > 0x99)
							ram_data[6] = 0x99;
						else
						{
							ram_data[6] &= 0xF0;
							ram_data[6] |= 0x09;
						}
					break;
					
				default:
				break;				
			}
			
			disp_on = DISP_TIME_ON;
		}
		else
			btn3--;	

//exit button	
	if(BUTTON4)
		btn4 = 0;
	else
		if(btn4 == 0)
		{
			btn4 = 1;
			BEEP_ON;										//beep on
			__delay_cycles(100000);
			BEEP_OFF;										//beep off
		
			menu = 0;
			disp_on = DISP_TIME_ON;
		}

	return;
}
