
#define I2C_SPEED 64 				//(~33 kHz)
#define I2C_TIMEOUT 5000		//подобрано

#define NCYCLE 10 					//количество тестов на дребезг
#define ADC_CH 2,3,7,6			//сканируемые каналы АЦП
#define DISP_TIME_ON 255		//длительность включенного дисплея

#define BEEP_ON PORTD&=0xBF
#define BEEP_OFF PORTD|=0x40

#define HEAT_ON PORTC|=0x02
#define HEAT_OFF PORTC&=0xFD

#define PUMP_ON PORTC|=0x01
#define PUMP_OFF PORTC&=0xFE

#define MENU_ITEM_MAX 9			//количество пунктов в меню
#define RAM_ITEM_MAX 12			//количество используемых байт в ОЗУ
#define BLINK_DEV 2					//частота мигания

#define BUTTON1 PIND&0x01		//MENU BUTTON
#define BUTTON2 PIND&0x02		//UP BUTTON
#define BUTTON3 PIND&0x08		//DOWN BUTTON
#define BUTTON4 PIND&0x04		//EXIT BUTTON
#define PROTECT PIND&0x20		//protection circuit input

#define MIN_INP_TEMP 5
#define MAX_OUT_TEMP 65
#define MIN_DIFF_TEMP 5
#define MAX_DIFF_TEMP 40
#define THRESHOLD_TEMP 10
#define	STEP_CORR_TEMP 3
#define MAX_CORR_TEMP 9
#define TIME_OUT 250
#define NUM_REPEAT 3
#define NUM_CYCLES_TIMEOUT 18000	//~30 мин

#define NUM_CYCLES_LOAD_COUNT 0x10000 

#include <iom328p.h>
#include <intrinsics.h> 

//functions declare;
void initialize(void);																//initialize function
void i2c_read(void);
void i2c_write(void);
void ch_correct(void);
void i2c_err(void);
void wait(void);																			//timeout wait function
void display(void);
void fill_disp(void);
void read_keyb(void);
void logic(void);
void send(unsigned char data,unsigned char dc);				//SPI send function