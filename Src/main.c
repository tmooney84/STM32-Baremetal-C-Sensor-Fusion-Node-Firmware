//#include <stdio.h>
//#include "uart.h"
//#include "adc_dma.h"
//
//extern uint16_t adc_raw_data[NUM_OF_CHANNELS];
//
//int main(void)
//{
//
//	/*Initialize debug UART*/
//	uart_init();
//
//	/*Initialize ADC DMA*/
//	adc_dma_init();
//
//
//	while(1)
//	{
//
//		printf("Value from sensor one : %d \n\r ",adc_raw_data[0]);
//
//		for( int i = 0; i < 90000; i++){}
//	}
//}



#include <stdio.h>
#include <string.h>

//#include "uart.h"
#include "uart_dma.h"
#include "adc_dma.h"

extern uint8_t g_rx_cmplt;
extern uint8_t g_uart_cmplt;
extern uint8_t g_tx_cmplt;

//extern char uart_data_buffer[UART_DATA_BUFF_SIZE];
char msg_buff[150] ={'\0'};

//!!!!!!!!
extern uint16_t adc_raw_data[NUM_OF_CHANNELS];

int main(void)
{

	uart2_tx_init();
	dma1_init();
	//dma1_stream5_uart_rx_config();
	sprintf(msg_buff,"Initialization...cmplt\n\r");
	dma1_stream6_uart_tx_config((uint32_t)msg_buff,strlen(msg_buff));

	//!!!!!!!!!
	/*Initialize ADC DMA*/
	adc_dma_init();

    while(!g_tx_cmplt){}


	while(1)
	{

		//if(g_rx_cmplt)
		if(1) //!!!
		{
			//sprintf(msg_buff, "Message received : %s \r\n",uart_data_buffer);
			sprintf(msg_buff, "Photoresistor Value : %d \n\r ",adc_raw_data[0]);

		for( int i = 0; i < 90000; i++){}
			g_rx_cmplt = 0;
			g_tx_cmplt = 0;
			g_uart_cmplt = 0;
			dma1_stream6_uart_tx_config((uint32_t)msg_buff,strlen(msg_buff));
		    while(!g_tx_cmplt){}

		}
	}
}
