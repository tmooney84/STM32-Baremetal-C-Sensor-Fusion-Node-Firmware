#include "adxl345.h"

 /********PINOUT********/
/* STM32 ---- ADXL345*/
/*
 * PB8   ----  SCL
 * PB9   ----  SDA
 * GND   ----  GND
 * 5V+   ----  VCC
 * */

// Variable to store single byte of data
char data;
uint8_t data_buffer[ADXL_BUFF_SIZE];

volatile int16_t accel_x, accel_y, accel_z;
volatile double accel_x_g, accel_y_g, accel_z_g;
volatile uint8_t data_ready_flag = 0;

void adxl_read_address (uint8_t reg)
{
	 i2c1_byte_read( ADXL345_DEVICE_ADDR, reg, &data);

}

void adxl_write (uint8_t reg, char value)
{
	char data[1];
	data[0] = value;
	i2c1_burst_write( ADXL345_DEVICE_ADDR, reg,1, data) ;
}

void adxl_read_values (uint8_t reg)
{
    // Read 6 bytes into wthe data buffer
	i2c1_burst_read_dma(ADXL345_DEVICE_ADDR, reg, ADXL_BUFF_SIZE, (char *)data_buffer);
}


void adxl_init (void)
{
	/*Enable I2C*/
	i2c1_dma_init();

	/*Read the DEVID, this should return 0xE5*/
	adxl_read_address(ADXL345_REG_DEVID);

	/*Set data format range to +-4g*/
	adxl_write (ADXL345_REG_DATA_FORMAT, ADXL345_RANGE_4G);

	/*Reset all bits*/
	adxl_write (ADXL345_REG_POWER_CTL, ADXL345_RESET);

	/*Configure power control measure bit*/
	adxl_write (ADXL345_REG_POWER_CTL, ADXL345_MEASURE_BIT);
}

void i2c1_rx_complete_callback(void) {
    // Combine high and low bytes
    accel_x = (int16_t)((data_buffer[1] << 8) | data_buffer[0]);
    accel_y = (int16_t)((data_buffer[3] << 8) | data_buffer[2]);
    accel_z = (int16_t)((data_buffer[5] << 8) | data_buffer[4]);

    // Convert raw data to g values
    accel_x_g = accel_x * 0.0078;
    accel_y_g = accel_y * 0.0078;
    accel_z_g = accel_z * 0.0078;

    // Signal to the main loop that new data is ready to be used
    data_ready_flag = 1;
}
