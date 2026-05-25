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
extern volatile uint8_t timer_trigger_flag;

//extern char uart_data_buffer[UART_DATA_BUFF_SIZE];
char msg_buff[150] ={'\0'};
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
	adc_dma_init();

	//Start up message
	sprintf(msg_buff,"Initialization...cmplt\n\r");
	dma1_stream6_uart_tx_config((uint32_t)msg_buff,strlen(msg_buff));
    while(!g_tx_cmplt){}
    g_tx_cmplt = 0;

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

		// Loop until BOTH flags are true (g_adc_rx_cmplt == 1 AND data_ready_flag == 1)
		while(!g_adc_rx_cmplt || !data_ready_flag)
		        {
		            __disable_irq();
		            // Double check flags before sleeping to prevent race conditions
		            if(!g_adc_rx_cmplt || !data_ready_flag)
		            {
		                enter_sleep_mode();
		            }
		            __enable_irq(); // Handlers run instantly here if an interrupt fired during sleep
		        }

		/* =========================================
		           STATE 3: PROCESS DATA
		   ========================================= */
		sprintf(msg_buff,
		    "Photoresistor Value : %d \n\raccel_x: %d \t accel_y: %d \t accel_z: %d\n\r",
		    adc_raw_data[0], accel_x, accel_y, accel_z);

		// Reset the flag for the next cycle
			g_tx_cmplt = 0;
			g_uart_cmplt = 0;
			g_adc_rx_cmplt = 0;
			data_ready_flag = 0;


			dma1_stream6_uart_tx_config((uint32_t)msg_buff,strlen(msg_buff));

			// Go to sleep while UART sends the data
			while(!g_tx_cmplt)
				{
			    	__disable_irq();
			        if(!g_tx_cmplt)
			            {
			                enter_sleep_mode();
			            }
			        __enable_irq();
			     }
		    g_tx_cmplt = 0;
		}

	return 0;
}
