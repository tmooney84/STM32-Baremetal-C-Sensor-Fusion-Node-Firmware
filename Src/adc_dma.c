#include "adc_dma.h"

#define GPIOAEN			(1U<<0)
#define ADC1EN			(1U<<8)
#define CR1_SCAN		(1U<<8)
#define CR2_DMA			(1U<<8)
#define CR2_DDS			(1U<<9)
#define CR2_CONT		(1U<<1)
#define CR2_ADCON		(1U<<0)
#define CR2_SWSTART		(1U<<30)

#define DMA2EN				(1U<<22)
#define DMA_SCR_EN  		(1U<<0)
#define DMA_SCR_MINC		(1U<<10)
#define DMA_SCR_PINC		(1U<<9)
#define DMA_SCR_CIRC		(1U<<8)
#define DMA_SCR_TCIE		(1U<<4)
#define DMA_SCR_TEIE		(1U<<2)
#define DMA_SFCR_DMDIS		(1U<<2)
#define ADC_SEQ_LEN_1  		0x00

#define LISR_TCIF0	(1U << 5)
#define LIFCR_CTCIF0	(1U << 5)

uint16_t adc_raw_data[NUM_OF_CHANNELS];
uint8_t g_adc_rx_cmplt;

void adc_dma_init(void)
{
	/************GPIO Configuration**********/
	/*Enable clock access to ADC GPIO Pin's Port*/
	RCC->AHB1ENR |= GPIOAEN;

	/*Set PA0 and PA1 mode to analog mode*/
//	GPIOA->MODER |= (1U<<0);
//	GPIOA->MODER |= (1U<<1);

	GPIOA->MODER |= (1U<<2);
	GPIOA->MODER |= (1U<<3);

	/************ADC Configuration**********/
	/*Enable clock access to ADC*/
	RCC->APB2ENR |= ADC1EN;

	/*Set sequence length*/
	ADC1->SQR1 = ADC_SEQ_LEN_1;

	/*Set sequence*/
	ADC1->SQR3 = (1U<<0);

	/*Disable scan mode*/
	ADC1->CR1 &= ~CR1_SCAN;

	/*Select to use DMA*/
	ADC1->CR2 |=CR2_CONT |CR2_DMA|CR2_DDS;


	/************DMA Configuration**********/
	/*Enable clock access to DMA*/
	RCC->AHB1ENR |=DMA2EN;


	/*Disable DMA stream*/
	DMA2_Stream0->CR &=~DMA_SCR_EN;

	/*Wait till DMA is disabled*/
	while((DMA2_Stream0->CR & DMA_SCR_EN)){}

	/*Enable Circular mode*/
	DMA2_Stream0->CR |=DMA_SCR_CIRC;

	/*Set MSIZE i.e Memory data size to half-word*/

	DMA2_Stream0->CR |= (1U<<13);
	DMA2_Stream0->CR &= ~(1U<<14);

	/*Set PSIZE i.e Peripheral data size to half-word*/
	DMA2_Stream0->CR |= (1U<<11);
	DMA2_Stream0->CR &= ~(1U<<12);

	/*Enable memory addr increment*/
	DMA2_Stream0->CR |=DMA_SCR_MINC;

	/*Set periph address*/
	DMA2_Stream0->PAR = (uint32_t)(&(ADC1->DR));
	/*Set mem address*/
	DMA2_Stream0->M0AR = (uint32_t)(&adc_raw_data);

	/*Set number of transfer*/
	DMA2_Stream0->NDTR = (uint16_t)NUM_OF_CHANNELS;

	//!!!!!!!!!!!!!!
	/* ---> STEP 1: Enable DMA Transfer Complete Interrupt <--- */
	    DMA2_Stream0->CR |= DMA_SCR_TCIE;

	//!!!!!!!!!!!!!!
	    /* ---> STEP 2: Configure NVIC for DMA2 Stream 0 <--- */
	        // Enable IRQ and set priority (adjust priority as needed for your application)
	        NVIC_SetPriority(DMA2_Stream0_IRQn, 0);
	        NVIC_EnableIRQ(DMA2_Stream0_IRQn);

	/*Enable DMA stream*/
	DMA2_Stream0->CR |= DMA_SCR_EN;

	/************ADC Configuration**********/
	/*Enable ADC*/
	ADC1->CR2 |=CR2_ADCON;

	/*Start ADC*/
	ADC1->CR2 |=CR2_SWSTART;


}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!
void DMA2_Stream0_IRQHandler(void)
{
    // Check if the Transfer Complete Interrupt Flag for Stream 0 is set
    //if (DMA2->LISR & (1U << 5))
    if (DMA2->LISR & LISR_TCIF0)
    {
    	//!!!!!!!!!!!!!!!
        g_adc_rx_cmplt = 1;

        /* ---> STEP 3: Clear the flag <--- */
        // Writing to LIFCR (Low Interrupt Flag Clear Register) clears the flag
        DMA2->LIFCR |= LIFCR_CTCIF0;
        // Your data processing goes here!
        // adc_raw_data now contains fresh, fully updated conversion values.

    }
}
