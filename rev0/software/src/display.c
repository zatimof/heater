#include "..\heat.h"

extern int temp[4],temp_corr;
extern unsigned char screen[8][16],disp_on,ch,menu,blink,ram_data[RAM_ITEM_MAX],valid,state,state_code,eco;
__flash const extern char img[33][5],init2[8];
extern unsigned long on_time,off_time;

//‘ункци€ обновлени€ индикатора. ѕередает образ диспле€ screen[][] в индикатор.
void display(void)
{
	unsigned char i,k,line;
	
	for(i=0;i<8;i++)																//установка указателей на 
		send(init2[i],0);															//начало экрана
	
	for(line=0;line<8;line++)												//цикл по строкам
		for(i=0;i<128;i++)														//цикл по столбцам
		{
			k=i&0x07;

			if((k>1)&&(k<7)&&disp_on)										//вывод строки сообщени€
				send(img[screen[line][i>>3]][k-2]<<1,1);
			else
				send(0x00,1);
		}

	return;
}

//‘ункц€и пересылки байта в индикатор по SPI.
void send(unsigned char data,unsigned char dc)
{
  if(dc)																					//выставл€ем сигнал DC и CS
    PORTB|=0x04;
  else
    PORTB&=0xFB;
	
	PORTD&=0x7F;
  
  for(unsigned char l=7;l!=255;l--)								//цикл из 8 бит
  {
    PORTB&=0xFE;																	//формируем спад CLK
		
    if(data&(0x01<<l))														//выставл€ем данные
      PORTB|=0x02;
		else
			PORTB&=0xFD;
		
		PORTB|=0x01;																	//формируем фронт CLK
  }
  
  PORTD|=0x80;																		//убираем сигнал CS
	
	return;
}

//‘ункци€ заполнени€ образа диспле€ данными.
void fill_disp(void)
{
	unsigned char i,k;
	int t;

//расчет и вывод температур с термодатчиков
	for(k=0;k<4;k++)																//цикл по 4 датчикам
	{
		t=temp[k];
		if((t<100)&&(t>-41))
		{
			screen[k][14]=19;														//вывод "градус —"
			screen[k][15]=12;
			valid|=(0x01<<k);														//установка бита валидности
			if(t<0)
			{
				t=(~t)+1;
				screen[k][11]=17;		//-
			}
			else
				screen[k][11]=16;		//_
				
			i=0;																				//преобразование в двоично-
			while(t>=10)																//дес€тичный вид и вывод
			{
				t-=10;
				i++;
			}
			screen[k][12]=i;
			screen[k][13]=t;
		}
		else
		{
			valid&=(~(0x01<<k));												//сброс бита валидности			
			screen[k][14]=16;
			screen[k][15]=16;

			if(t>99)																		//OH - over heat
			{
				screen[k][12]=0;
				screen[k][13]=22;
			}
			if(t>220)																		//NC - not connected
			{
				screen[k][12]=25;
				screen[k][13]=12;
			}
			if(t<-40)																		//OC - over cool
			{
				screen[k][12]=0;
				screen[k][13]=12;
			}
			if(t<-173)																	//SC - short circuit
			{
				screen[k][12]=28;
				screen[k][13]=12;
			}
		}
	}

//вывод времени, даты и уставок из буфера RTC
	i=ram_data[0];																	//sec
	if((menu==5)&&(blink&BLINK_DEV))	
	{
		screen[6][14]=16;
		screen[6][15]=16;
	}
	else
	{
		screen[6][14]=(i&0x70)>>4;
		screen[6][15]=i&0x0F;
	}

	if(i&0x80)
		ch=1;
	else
		ch=0;
	
	i=ram_data[1];																	//min
	if((menu==4)&&(blink&BLINK_DEV))	
	{
		screen[6][11]=16;
		screen[6][12]=16;
	}
	else
	{
		screen[6][11]=(i&0x70)>>4;
		screen[6][12]=i&0x0F;
	}
	
	i=ram_data[2];																	//hour
	if((menu==3)&&(blink&BLINK_DEV))
	{
		screen[6][6]=16;	
		screen[6][7]=16;	
		screen[6][8]=16;	
		screen[6][9]=16;		
	}
	else
	{
		screen[6][9]=i&0x0F;
	
		if(i&0x40)
		{
			screen[6][8]=(i&0x10)>>4;
			screen[6][7]=24;				//M
			if(i&0x20)
				screen[6][6]=26;			//P
			else
				screen[6][6]=10;			//A
		}
		else
		{
			screen[6][8]=(i&0xF0)>>4;
			screen[6][7]=16;
			screen[6][6]=16;
		}
	}
	
	i=ram_data[3];																	//day
	if((menu==6)&&(blink&BLINK_DEV))
	{
		screen[7][1]=16;
		screen[7][2]=16;
		screen[7][3]=16;
	}
	else	
		switch(i)
		{
			case 1: //MON
				screen[7][1]=24;
				screen[7][2]=0;
				screen[7][3]=25;
				break;
				
			case 2: //TUE
				screen[7][1]=29;
				screen[7][2]=30;
				screen[7][3]=14;
				break;	
				
			case 3: //WED
				screen[7][1]=31;
				screen[7][2]=14;
				screen[7][3]=13;
				break;
				
			case 4: //THU
				screen[7][1]=29;
				screen[7][2]=22;
				screen[7][3]=30;
				break;
				
			case 5: //FRI
				screen[7][1]=15;
				screen[7][2]=27;
				screen[7][3]=23;
				break;
				
			case 6: //SAT
				screen[7][1]=28;
				screen[7][2]=10;
				screen[7][3]=29;
				break;
				
			case 7: //SUN
				screen[7][1]=28;
				screen[7][2]=30;
				screen[7][3]=25;
				break;
			
			default:
				screen[7][1]=16;
				screen[7][2]=16;
				screen[7][3]=16;			
		}
	
	i=ram_data[4];																	//date
	if((menu==7)&&(blink&BLINK_DEV))
	{
		screen[7][6]=16;
		screen[7][7]=16;
	}
	else
	{
		screen[7][6]=(i&0x30)>>4;
		screen[7][7]=i&0x0F;
	}
	
	i=ram_data[5];																	//month
	if((menu==8)&&(blink&BLINK_DEV))
	{
		screen[7][9]=16;
		screen[7][10]=16;
	}
	else
	{
		screen[7][9]=(i&0x10)>>4;
		screen[7][10]=i&0x0F;
	}
	
	i=ram_data[6];																	//year
	if((menu==9)&&(blink&BLINK_DEV))
	{
		screen[7][14]=16;
		screen[7][15]=16;	
	}
	else
	{
		screen[7][14]=(i&0xF0)>>4;
		screen[7][15]=i&0x0F;	
	}
	
	i=ram_data[7];																	//control register (not used)
		
	i=ram_data[8];																	//set temperature
	if(i>99)
		i=0;
		
	k=0;																						//преобразование в двоично-
  while(i>=10)																		//дес€тичный вид и вывод
  {
  	i-=10;
    k++;
  }
  if((menu==1)&&(blink&BLINK_DEV))
	{
		screen[4][9]=16;
		screen[4][8]=16;
	}
	else
	{
		screen[4][9]=i;
		screen[4][8]=k;
	}
	
	i=ram_data[9];																	//set hysteresis
	if((i<1)||(i>9))
		i=1;
	
	if((menu==2)&&(blink&BLINK_DEV))
		screen[4][11]=16;
	else
		screen[4][11]=i;
	
	screen[4][13]=temp_corr&0x0F;

//вывод строки состо€ни€	
	switch(state)
	{
		case 0: //-OFF-
			screen[5][5]=17;
			screen[5][6]=0;
			screen[5][7]=15;
			screen[5][8]=15;
			screen[5][9]=17;
			break;
				
		case 1: //-STOP
			screen[5][5]=17;
			screen[5][6]=28;
			screen[5][7]=29;
			screen[5][8]=0;
			screen[5][9]=26;
			break;
		
		case 2: //START
			screen[5][5]=28;
			screen[5][6]=29;
			screen[5][7]=10;
			screen[5][8]=27;
			screen[5][9]=29;
			break;
				
		case 3: //-HEAT
			screen[5][5]=17;
			screen[5][6]=22;
			screen[5][7]=14;
			screen[5][8]=10;
			screen[5][9]=29;
			break;
			
		case 4: //ERROR
			screen[5][5]=14;
			screen[5][6]=27;
			screen[5][7]=27;
			screen[5][8]=0;
			screen[5][9]=27;
			break;
			
		default: //-----
			screen[5][5]=17;
			screen[5][6]=17;
			screen[5][7]=17;
			screen[5][8]=17;
			screen[5][9]=17;
			break;
	}
/*
	switch(eco&0x03)
	{
		case 0: //eco off
			screen[5][8]=0;			
			break;
				
		case 1: //day
			screen[5][8]=13;	//D
			break;
		
		case 2: //night
			screen[5][8]=25;		//N
			break;
				
		case 3: //eco off
			screen[5][8]=0;			
			break;
	}
*/	
	if(on_time||off_time)
		t=on_time*100/(on_time+off_time);
	else
		t=0;
	
	if((on_time+off_time)>NUM_CYCLES_LOAD_COUNT)
	{
		if(on_time>t)
			on_time-=t;
		if(off_time>(100-t))
			off_time-=(100-t);
	}	
	
	k=0;																						//преобразование в двоично-
  while(t>=10)																		//дес€тичный вид и вывод
  {
  	t-=10;
    k++;
  }
	screen[6][0]=k;
	screen[6][1]=t;
	
	t=state_code;
	screen[6][4]=((t&0xF0)>>4);
	screen[6][5]=(t&0x0F);
	screen[6][6]=(MCUSR&0x0F);
	
	return;
}
