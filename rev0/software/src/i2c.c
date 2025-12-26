#include "..\heat.h"

//variables declare
extern unsigned char ram_data[RAM_ITEM_MAX], state_code, err, err_cnt;
extern int temp_set, temp_hyst, temp_set_s, temp_hyst_s, temp_corr;

void i2c_read(void)
{
	TWCR = 0xE5;							//start
	wait();

	TWDR = 0xD0;							//address
	TWCR = 0xC5;
	wait();
		
	TWDR = 0x00;							//register
	TWCR = 0xC5;
	wait();
		
	TWCR = 0xE5;							//restart
	wait();
		
	TWDR = 0xD1;							//address
	TWCR = 0xC5;
	wait();
		
	for(unsigned char i = 0; i < RAM_ITEM_MAX; i++)
	{
		TWCR = 0xC5;
		wait();
		ram_data[i] = TWDR;
	}
	
	TWCR = 0xD5;							//stop
	TWCR = 0x00;							//off	

	return;
}

void i2c_write(void)
{
	TWCR = 0xE5;							//start
	wait();

	TWDR = 0xD0;							//address
	TWCR = 0xC5;
	wait();

	TWDR = 0x00;							//register
	TWCR = 0xC5;
	wait();
	
	for(unsigned char i = 0; i < RAM_ITEM_MAX; i++)
	{
		TWDR = ram_data[i];
		TWCR = 0xC5;
		wait();
	}
	
	TWCR = 0xD5;							//stop
	TWCR = 0x00;							//off	

	return;
}

void ch_correct(void)
{
	unsigned char i;	
	
	TWCR = 0xE5;							//start
	wait();

	TWDR = 0xD0;							//address
	TWCR = 0xC5;
	wait();

	TWDR = 0x00;							//register
	TWCR = 0xC5;
	wait();

	TWCR = 0xE5;							//restart
	wait();
	
	TWDR = 0xD1;							//address
	TWCR = 0xC5;
	wait();
	
	TWCR = 0xC5;							//sec
	wait();
	i = TWDR;

	TWCR = 0xE5;							//restart
	wait();
	
	TWDR = 0xD0;							//address
	TWCR = 0xC5;
	wait();
	
	TWDR = 0x00;							//register
	TWCR = 0xC5;
	wait();
	
	TWDR = i & 0x7F;
	TWCR = 0xC5;							//clear ch
	wait();
	
	TWCR = 0xD5;							//stop
	TWCR = 0x00;							//off	

	return;
}

void wait(void)						//timeout wait function
{
	unsigned int t = I2C_TIMEOUT;
	
	while(!(TWCR & 0x80))
		if(t)
			t--;
		else
		{
			err++;
			break;
		}

	return;
}

void i2c_err(void)
{
	if(err)												//i2c timeout error check
		err_cnt = NUM_REPEAT;
	else
		if(err_cnt)
			err_cnt--;
		else
			state_code &= 0xFD;
		
	if((ram_data[8] == ram_data[10]) && (!err) && (!err_cnt))
	{
		temp_set = ram_data[8];
		temp_set_s = temp_set;
		state_code &= 0xFD;
	}
	else													//i2c data error check
	{
		temp_set = temp_set_s;
		state_code |= 0x02;
	}
	
	if((ram_data[9] == ram_data[11]) && (!err) && (!err_cnt))
	{
		temp_hyst = ram_data[9];
		temp_hyst_s = temp_hyst;
		state_code &= 0xFD;
	}
	else													//i2c data error check
	{
		temp_hyst = temp_hyst_s;
		state_code |= 0x02;
	}
	return;
}