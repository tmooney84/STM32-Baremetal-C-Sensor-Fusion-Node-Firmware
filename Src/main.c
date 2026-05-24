#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "uart_dma.h"
#include "adc_dma.h"
#include "stm32f4xx.h"
#include "adxl345.h"
#include "tim.h"

extern volatile uint8_t g_uart_cmplt;
extern volatile uint8_t g_tx_cmplt;
extern volatile uint8_t g_adc_rx_cmplt;
extern volatile uint8_t timer_trigger_flag = 0;

//extern char uart_data_buffer[UART_DATA_BUFF_SIZE];
char msg_buff[150] ={'\0'};

//!!!!!!!!
extern uint16_t adc_raw_data[NUM_OF_CHANNELS];

void enter_sleep_mode(void)
{
    // Clear the SLEEPDEEP bit in the System Control Block (SCB)
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

    // Wait For Interrupt
    __WFI();
}

int main(void)
{

	uart2_tx_init();
	dma1_init();
	adxl_init();


	sprintf(msg_buff,"Initialization...cmplt\n\r");
	dma1_stream6_uart_tx_config((uint32_t)msg_buff,strlen(msg_buff));

	//!!! CAN I PUT THIS WITH THE OTHER INITS without breaking things???
	adc_dma_init();

    while(!g_tx_cmplt){}

    // Initialize our 50ms timer
    tim2_init();

	while(1)

	{
		/* =========================================
		           STATE 1: WAIT FOR 50ms TIMER
		   ========================================= */
		__disable_irq();
		if(!timer_trigger_flag) {
		    enter_sleep_mode();
		  }
		__enable_irq();

		//Clear the flag for the next cycle.
		timer_trigger_flag = 0;

		/* =========================================
		           STATE 2: TRIGGER DMA AND WAIT FOR DATA
		   ========================================= */
		// Kick off the asynchronous DMA read
		adxl_read_values(ADXL345_REG_DATA_START);


		// Go right back to sleep until the DMA hardware tells us it's done
		__disable_irq();

		//!!! WILL THIS WORK WITH BOTH or data_ready_flag original
		if(!g_adc_rx_cmplt || !data_ready_flag) //!!! >>> interrupt version of adc is much slower
		{
			enter_sleep_mode();
		}
		__enable_irq(); // Interrupts are instantly serviced here upon wake-up

		//!!! Checking if the data is ready, if not, go back to sleep.
		while(!g_adc_rx_cmplt && !data_ready_flag){}

		/* =========================================
		           STATE 3: PROCESS DATA
		   ========================================= */
		sprintf(msg_buff,
		    "Photoresistor Value : %d \n\raccel_x: %d \t accel_y: %d \t accel_z: %d\n\r",
		    adc_raw_data[0], accel_x, accel_y, accel_z);

		//!!! IS THIS NEEDED... FROM ORIGINAL CODE
		//for( int i = 0; i < 90000; i++){}

		// 5. Reset the flag for the next cycle
			g_tx_cmplt = 0;
			g_uart_cmplt = 0;
			g_adc_rx_cmplt = 0;
			data_ready_flag = 0;


			dma1_stream6_uart_tx_config((uint32_t)msg_buff,strlen(msg_buff));
		    while(!g_tx_cmplt){}

		}
	return 0;
}
