//Управляющая программа микроконтроллера ATmega328P контроллера газовой колонки v.1.0
//Распиновка индикатора SPI:
//PB0 - D0 - SCK
//PB1 - D1 - SDIN
//PB2 - DC - DATA/COMMAND (LOW - COMMAND, HIGH - DATA)
//PD7 - CS - CHIP SELECT (LOW - ACTIVE)
//Распиновка клавиатуры (LOW - ACTIVE):
//PD1 - КН1
//PD0 - КН2
//PD2 - КН3
//PD3 - КН4
//Распиновка АЦП
//ADC2 - датчик 1
//ADC3 - датчик 2
//ADC6 - датчик 3
//ADC7 - датчик 4
//Остальные функции:
//PC0 - Реле включения помпы (HIGH - ACTIVE)
//PC1 - Реле включения колонки (HIGH - ACTIVE)
//PD6 - бипер (LOW - ACTIVE)
//PD5 - вход цепи защиты
//Автор Захаров Т. Р. 2024 г.

#include "heat.h"

//global variables declare
//массив индикатора 8 строк по 16 знакомест
unsigned char screen[8][16]={	{23,25,26,30,29,16,16,16,29,18,16,16,16,16,19,12},
															{0,30,29,26,30,29,16,16,29,18,16,16,16,16,19,12},
															{23,25,13,0,0,27,16,16,29,18,16,16,16,16,19,12},
															{0,30,29,13,0,0,27,16,29,18,16,16,16,16,19,12},
															{28,14,29,16,29,18,16,16,16,16,20,16,32,16,19,12},
															{17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17},
															{16,16,21,16,16,16,16,16,0,0,18,0,0,18,0,0},
															{16,16,16,16,16,16,0,0,17,0,0,17,2,0,0,0}};			
//universal variables
unsigned char i,ch=0,disp_on=DISP_TIME_ON,menu=0,blink=0,valid=0,state=0;
unsigned char state_code=0,eco=0,delay=TIME_OUT,repeat=NUM_REPEAT;
unsigned long on_time=0,off_time=0,idle_time=NUM_CYCLES_TIMEOUT;

//adc variables
unsigned char sel_adc[4]={ADC_CH},cur_adc=0;
int adc[4];

//i2c variables
unsigned char ram_data[RAM_ITEM_MAX],err=0,err_cnt=0;

//temperature variables
int temp[4]={0,0,0,0},temp_set=0,temp_hyst=1,temp_set_s=0,temp_hyst_s=1,temp_corr=0;

//button variables
unsigned char btn1,btn2,btn3,btn4;

//digit codegen
__flash const char img[33][5]={
{0x3E,0x41,0x41,0x41,0x3E},			//0										
{0x44,0x42,0x7F,0x40,0x40},			//1
{0x42,0x61,0x51,0x49,0x46},			//2
{0x21,0x41,0x45,0x4B,0x31},			//3
{0x18,0x14,0x12,0x7F,0x10},			//4
{0x27,0x45,0x45,0x45,0x39},			//5
{0x3C,0x4A,0x49,0x49,0x30},			//6
{0x01,0x71,0x09,0x05,0x03},			//7
{0x36,0x49,0x49,0x49,0x36},			//8
{0x06,0x49,0x49,0x29,0x1E},			//9
{0x7E,0x11,0x11,0x11,0x7E},		//A	10
{0x7F,0x49,0x49,0x49,0x36},		//B	11
{0x3E,0x41,0x41,0x41,0x22},		//C	12
{0x7F,0x41,0x41,0x41,0x3E},		//D	13
{0x7F,0x49,0x49,0x49,0x49},		//E	14
{0x7F,0x09,0x09,0x09,0x09},		//F	15
{0x00,0x00,0x00,0x00,0x00},		//_	16 (пробел)
{0x08,0x08,0x08,0x08,0x08},		//-	17
{0x00,0x00,0x36,0x36,0x00},		//:	18
{0x00,0x00,0x07,0x05,0x07},		//.	19 (градус)
{0x24,0x24,0x3F,0x24,0x24},		//± 20
{0x23,0x13,0x08,0x64,0x62},		//% 21
{0x7F,0x08,0x08,0x08,0x7F},		//H 22
{0x00,0x41,0x7F,0x41,0x00},		//I	23
{0x7F,0x06,0x18,0x06,0x7F},		//M 24
{0x7F,0x06,0x08,0x30,0x7F},		//N 25
{0x7F,0x09,0x09,0x09,0x06},		//P	26
{0x7F,0x09,0x09,0x19,0x66},		//R	27
{0x26,0x49,0x49,0x49,0x32},		//S	28
{0x01,0x01,0x7F,0x01,0x01},		//T	29
{0x3F,0x40,0x40,0x40,0x3F},		//U	30
{0x3F,0x40,0x3E,0x40,0x3F},		//W 31
{0x08,0x08,0x3E,0x08,0x08}};	//+ 32

//OLED init string 1
__flash const char init1[23]={0xAE,0xD5,0x90,0xA8,
0x3F,0xD3,0x00,0x40,0x8D,0x14,0xA1,0xC8,0xDA,0x12,
0x81,0xB0,0xD9,0x22,0xDB,0x30,0xA4,0xA6,0xAF};				

//OLED init string 2
__flash const char init2[8]={0x20,0x00,0x21,0x00,0x7F,0x22,0x00,0x07};

//main function
void main(void)
{
  initialize();										//начальная инициализация
	
  while(1)
  {
    PORTB^=0x20;									//led blink
		blink++;
		
		err=0;
		if(menu)
			i2c_write();								//write rtc
		else
			i2c_read();									//read rtc

		if(ch)												//ch correct
			ch_correct();
		
		i2c_err();

		logic();											//main logic
		
		//disp_on=10;										//display off disable
		if(disp_on)										//display on timeout
			disp_on--;
		else
			menu=0;
		
		fill_disp();
		display();										//screen refresh
		
		read_keyb();									//keyb read
		
		__watchdog_reset();						//watchdog reset
		
    __delay_cycles(1000000);			//delay
  }
}

//initialize function
void initialize(void)
{
//init ports  
	DDRB=0x27;
	PORTB=0x07;
	
	DDRD=0xC0;
	PORTD=0xCF;
	
	DDRC=0x03;
	PORTC=0x00;
	
	MCUCR=0x00;
	  
//init timer indicator
	TCCR1A=0x00;
	TCCR1B=0x01;
	TCCR1C=0x00;
	
//init ADC
	ADMUX=0x42;
	ADCSRA=0xCF;
	ADCSRB=0x00;
	DIDR0=0xCC;
	
//init WatchDog
	__watchdog_reset();
	WDTCSR|=0x18;
	WDTCSR=0x1F;
	
//init interrupts
	SREG|=128;

//init i2c
	TWBR=I2C_SPEED;
	TWSR=0x02;
	TWAR=0xFE;
	TWAMR=0x00;
	TWCR=0x00;
	
//init display
	BEEP_ON;

	for(i=0;i<23;i++)								//загрузка строки инициализации
		send(init1[i],0);
	
	__delay_cycles(1000000);
	
	for(i=0;i<8;i++)								//загрузка второй строки инициализации
		send(init2[i],0);

	BEEP_OFF;
	
	return;
}

#pragma vector=ADC_vect						//прерывание по АЦП
__interrupt void adcf(void)
{
	int adct;	

	ADCSRA=0x0F;
	adct=ADC;
	//adc[cur_adc]+=(adct-adc[cur_adc])>>5;
	adc[cur_adc]=adct;
	cur_adc++;
	cur_adc&=0x03;
	ADMUX&=0xF0;
	ADMUX|=sel_adc[cur_adc];
	ADCSRA=0xCF;
	
	return;
}

#pragma vector=TWI_vect 
__interrupt void twi(void)
{
	return;
}


