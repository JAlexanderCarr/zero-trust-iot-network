#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include "iot.h"

using namespace std;

//  SPI Bus Controls
int SPI_BUS             = 1;    //SPI Bus 1 (AKA SPIDEV2.x)
int SPI_FREQ            = 1000000; // SPI clock operating frequency
int SPI_MOSI_PIN 		= 76; 	//GPIO3_16
int SPI_MISO_PIN 		= 75; 	//GPIO3_15
int SPI_CLK_PIN 		= 77; 	//GPIO3_14
int SPI_CS0_PIN			= 74; 	//GPIO3_17
int SPI_CS1_PIN         = 88;   //GPIO0_7

// I2C Bus Controls
int I2CS_BUS        = 1;   	//I2C Bus to be used for Sensors
int I2C1_SCL_PIN    = 63;   //GPIO0_5
int I2C1_SDA_PIN    = 64;   //GPIO0_4

int I2CP_BUS        = 2;   	//I2C Bus Bus for Display 
int I2C2_SCL_PIN    = 65;  	//GPIO0_13
int I2C2_SDA_PIN    = 66;  	//GPIO0_12

int RELAY_PIN		= 87;   ////GPIO0_20

// GP - General Purpose io pins
int GP2             = 87; 	//GPIO0_20
int GP3             = 73; 	//GPIO3_19
int GP4             = 71; 	//GPIO3_21
int GP5             = 62;  	//GPIO1_19
int GP6             = 60;  	//GPIO1_18
int GP7             = 69;  	//GPIO1_17
int GP8             = 61;  	//GPIO1_16
int GP9             = 59;  	//GPIO0_31
int GPB4            = 57;  	//GPIO0_30


int UART1 			= 1;

// Buttons
int B1				= 69;
int B2				= 61;
int B3				= 59;
int B4				= 57;

mraa_i2c_context I2Chandle;
mraa_i2c_context i2cp;
mraa_gpio_context MCP3208_DIN;
mraa_gpio_context MCP3208_DOUT;
mraa_gpio_context MCP3208_CLK;
mraa_gpio_context MCP3208_CS;
const int ADC_CHAN = 7;
const int SLAVE_ADDR = 96;
const int STATUS = 0;
const int OUT_P_MSB = 1;
const int OUT_P_CSB = 2;
const int OUT_P_LSB = 3;
const int CTRL_REG1 = 38;
const int PT_DATA_CFG = 19;

// Helper functions needed for collecting IoT sensor data
void setup()
{
	// Setting up pressure sensor
	int status;
	I2Chandle = mraa_i2c_init_raw(I2CS_BUS);
	mraa_i2c_address(I2Chandle, SLAVE_ADDR);
	status = mraa_i2c_read_byte(I2Chandle);
	if (status < 0){
	     printf("Failed to Initialize Pressure Sensor -> Exiting program\n");
	     mraa_i2c_stop(I2Chandle);
	     exit(EXIT_FAILURE);
	}
	SetupPressureSensor();
	
	// Setting up temperature sensor
	// MCP3208_DIN = mraa_gpio_init(SPI_MOSI_PIN);
	// MCP3208_DOUT = mraa_gpio_init(SPI_MISO_PIN);
	// MCP3208_CLK = mraa_gpio_init(SPI_CLK_PIN);
	// MCP3208_CS = mraa_gpio_init(SPI_CS0_PIN);
		
	MCP3208_DIN = mraa_gpio_init (SPI_MOSI_PIN);
	MCP3208_DOUT = mraa_gpio_init (SPI_MISO_PIN);
	MCP3208_CLK = mraa_gpio_init (SPI_CLK_PIN);
	MCP3208_CS = mraa_gpio_init (SPI_CS0_PIN);
        sleep(1); // Need a delay before setting the direction.	
	mraa_gpio_dir(MCP3208_DIN, MRAA_GPIO_OUT_HIGH);
	mraa_gpio_dir(MCP3208_DOUT, MRAA_GPIO_IN);
	mraa_gpio_dir(MCP3208_CLK, MRAA_GPIO_OUT);
	mraa_gpio_dir(MCP3208_CS, MRAA_GPIO_OUT);
}

void SetupPressureSensor()
{
	unsigned char tempVal;
	mraa_i2c_write_byte_data(I2Chandle, 0x07, PT_DATA_CFG); // Enable all three pressure and temp event flags 
	tempVal =  mraa_i2c_read_byte_data(I2Chandle, CTRL_REG1);
	
	// Activate Barometer mode 
	tempVal &= ~(1 << 7); //Clear ALT bit
	tempVal |= (1 << 0); //Set SBYB bit for Active mode
	mraa_i2c_write_byte_data(I2Chandle, tempVal, CTRL_REG1);
}

void toggleOneShot()
{
	unsigned char Value = mraa_i2c_read_byte_data(I2Chandle, CTRL_REG1); //Read current settings
	Value &= ~(1 << 1); //Clear OST bit
	mraa_i2c_write_byte_data(I2Chandle, Value, CTRL_REG1);

	Value = mraa_i2c_read_byte_data(I2Chandle, CTRL_REG1); //Read current settings to be safe
	Value |= (1 << 1); //Set OST bit
	mraa_i2c_write_byte_data(I2Chandle, Value, CTRL_REG1);
}

float ReadPressure()
{
	unsigned char msb = 0;
	unsigned char csb = 0;
	unsigned char lsb = 0;

	if (mraa_i2c_read_byte_data(I2Chandle, STATUS) & ((1 << 2) == 0)) {
		toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading
	}

	// Wait for PDR bit, indicates we have new pressure data
	while (mraa_i2c_read_byte_data(I2Chandle, STATUS) & ((1 << 2) == 0)) {
		usleep(10);
	}
	
	msb = mraa_i2c_read_byte_data(I2Chandle, OUT_P_MSB);
	csb = mraa_i2c_read_byte_data(I2Chandle, OUT_P_CSB);
	lsb = mraa_i2c_read_byte_data(I2Chandle, OUT_P_LSB);
	
	toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

	// Pressure comes back as a left shifted 20 bit number
	long pressure_whole = (long)msb << 16 | (long)csb << 8 | (long)lsb;
	pressure_whole >>= 6;

	lsb &= 0x30; //Bits 5/4 represent the fractional component
	lsb >>= 4; //Get it right aligned
	float pressure_decimal = (float)lsb / 4.0; //Turn it into fraction

	float pressure = (float)pressure_whole + pressure_decimal;
	return(pressure/100);
}

float ReadTemperature()
{
	int i;
	int val;
	
	mraa_gpio_write (MCP3208_DIN, 0);
	mraa_gpio_write (MCP3208_CLK, 0);
	mraa_gpio_write (MCP3208_CS, 0);

	int Channel = ADC_CHAN | 0x18;
	for (i = 0; i < 5; i ++)
	{
		if (Channel & 0x10)
		{
			mraa_gpio_write (MCP3208_DIN, 1);
		}
		else
		{
			mraa_gpio_write (MCP3208_DIN, 0);
		}
		Channel <<= 1;

		mraa_gpio_write (MCP3208_CLK, 0);
		mraa_gpio_write (MCP3208_CLK, 1);
	}

	mraa_gpio_write (MCP3208_CLK, 0);
	mraa_gpio_write (MCP3208_CLK, 1);

	mraa_gpio_write (MCP3208_CLK, 0);
	mraa_gpio_write (MCP3208_CLK, 1);

	val = 0;
	for (i = 0; i < 12; i ++)
	{
		mraa_gpio_write (MCP3208_CLK, 0);
		mraa_gpio_write (MCP3208_CLK, 1);
		
		val = (val << 1) | ((int) mraa_gpio_read (MCP3208_DOUT));	
	}
	
	mraa_gpio_write (MCP3208_CS, 1);
	mraa_gpio_write (MCP3208_DIN, 0);
	mraa_gpio_write (MCP3208_CLK, 0);

	return ((((float) val) * 3300 / 4096)-500)/10;
	
}