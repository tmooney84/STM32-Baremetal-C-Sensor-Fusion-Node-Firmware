#include "stm32f4xx.h"
#include "adxl345.h"


#define GPIOBEN                 (1U<<1)
#define I2C1EN                  (1U<<21)
#define DMA1EN                  (1U<<21) // Clock for DMA1

#define I2C_100KHZ              80
#define SD_MODE_MAX_RISE_TIME   17

#define CR1_PE                  (1U<<0)
#define SR2_BUSY                (1U<<1)
#define CR1_START               (1U<<8)
#define SR1_SB                  (1U<<0)
#define SR1_ADDR                (1U<<1)
#define SR1_TXE                 (1U<<7)
#define CR1_ACK                 (1U<<10)
#define CR1_STOP                (1U<<9)
#define SR1_RXNE                (1U<<6)
#define SR1_BTF                 (1U<<2)

// DMA specific I2C bits
#define I2C_CR2_DMAEN           (1U<<11)
#define I2C_CR2_LAST            (1U<<12)

#define DMA_SCR_EN  		(1U<<0)
#define DMA_SCR_CIRC		(1U<<8)
#define DMA_SCR_MINC		(1U<<10)

/*
 * PB8 ---- SCL
 * PB9 ----- SDA
 * */

extern uint8_t data_buffer[ADXL_BUFF_SIZE];

// Weak callback that can be overridden in user code (e.g., adxl345.c)
__attribute__((weak)) void i2c1_rx_complete_callback(void) {
    // Default empty implementation
}

void i2c1_init(void)
{
	/*Enable clock access to GPIOB*/
	 RCC->AHB1ENR |=GPIOBEN;

	/*Set PB8 and PB9 mode to alternate function*/
	GPIOB->MODER &=~(1U<<16);
	GPIOB->MODER |=(1U<<17);

	GPIOB->MODER &=~(1U<<18);
	GPIOB->MODER |=(1U<<19);

	/*Set PB8 and PB9 output type to  open drain*/
	GPIOB->OTYPER |=(1U<<8);
	GPIOB->OTYPER |=(1U<<9);

	/*Enable Pull-up for PB8 and PB9*/
	GPIOB->PUPDR |=(1U<<16);
	GPIOB->PUPDR &=~(1U<<17);

	GPIOB->PUPDR |=(1U<<18);
	GPIOB->PUPDR &=~(1U<<19);

	/*Set PB8 and PB9 alternate function type to I2C (AF4)*/
	GPIOB->AFR[1] &=~(1U<<0);
	GPIOB->AFR[1] &=~(1U<<1);
	GPIOB->AFR[1] |=(1U<<2);
	GPIOB->AFR[1] &=~(1U<<3);

	GPIOB->AFR[1] &=~(1U<<4);
	GPIOB->AFR[1] &=~(1U<<5);
	GPIOB->AFR[1] |=(1U<<6);
	GPIOB->AFR[1] &=~(1U<<7);


	/*Enable clock access to I2C1*/
	 RCC->APB1ENR |= I2C1EN;

	/*Enter reset mode  */
	I2C1->CR1 |= (1U<<15);

	/*Come out of reset mode  */
	I2C1->CR1 &=~(1U<<15);

	/*Set Peripheral clock frequency*/
	I2C1->CR2 = (1U<<4);   //16 Mhz

	/*Set I2C to standard mode, 100kHz clock */
	I2C1->CCR = I2C_100KHZ;

	/*Set rise time */
	I2C1->TRISE = SD_MODE_MAX_RISE_TIME;

// //!!!!!!!!!!
//	//DMA Configuration???
//	/*Enable clock access to DMA1*/
//	RCC->AHB1ENR |= DMA1EN;
//
//	/*Disabl//!!!!!!!!!!
//	//DMA Configuration???
//	/*Enable clock access to DMA1*/
//	RCC->AHB1ENR |= DMA1EN;
//
//	/*Disable DMA1 stream0 Rx*/
//	DMA1_Stream0->CR &=~DMA_SCR_EN;
//
//	/*Disable DMA1 stream1 Tx*/
//	DMA1_Stream1->CR &=~DMA_SCR_EN;
//
//	/*Wait till DMA is disabled*/
//	while((DMA1_Stream0->CR & DMA_SCR_EN)
//			|| DMA1_Stream1->CR & DMA_SCR_EN){}
//
//	/*Enable Circular mode*/
//	DMA1_Stream0->CR |=DMA_SCR_CIRC;
//	DMA1_Stream1->CR |=DMA_SCR_CIRC;
//
//	/*Set MSIZE i.e Memory data size to byte*/
//
//	DMA1_Stream0->CR &= ~(1U<<13);
//	DMA1_Stream0->CR &= ~(1U<<14);
//
//	DMA1_Stream1->CR &= ~(1U<<13);
//	DMA1_Stream1->CR &= ~(1U<<14);
//
//	/*Set PSIZE i.e Peripheral data size to byte*/
//	DMA1_Stream0->CR &= ~(1U<<11);
//	DMA1_Stream0->CR &= ~(1U<<12);
//
//	DMA1_Stream1->CR &= ~(1U<<11);
//	DMA1_Stream1->CR &= ~(1U<<12);
//
//	/*Enable memory addr increment*/
//	DMA1_Stream0->CR |=DMA_SCR_MINC;
//	DMA1_Stream1->CR |=DMA_SCR_MINC;
//



	/*Enable I2C1 module */
	I2C1->CR1 |= CR1_PE;
}



	void i2c1_dma_init(void) {
	    // 1. Initialize the standard I2C pins and speeds
	    i2c1_init();

	    // 2. Enable clock access to DMA1
	    RCC->AHB1ENR |= DMA1EN;

	    // 3. Disable DMA1 Stream 0 before configuration
	    DMA1_Stream0->CR &= ~(1U<<0);
	    while(DMA1_Stream0->CR & (1U<<0)){} // Wait until disabled

	    // 4. Set DMA1 Stream 0 Channel to Channel 1 (CHSEL = 001) for I2C1_RX
	    DMA1_Stream0->CR |=  (1U<<25);
	    DMA1_Stream0->CR &= ~(1U<<26);
	    DMA1_Stream0->CR &= ~(1U<<27);

	    // 5. Set memory increment mode (MINC)
	    DMA1_Stream0->CR |= (1U<<10);

	    // 6. Set transfer direction: Peripheral to Memory (DIR = 00)
	    DMA1_Stream0->CR &= ~(1U<<6);
	    DMA1_Stream0->CR &= ~(1U<<7);

	    // 7. Enable DMA Transfer Complete Interrupt (TCIE)
	        DMA1_Stream0->CR |= (1U<<4);

	    // 8. Enable DMA1 Stream 0 Interrupt in the NVIC
	    NVIC_EnableIRQ(DMA1_Stream0_IRQn);
	}





void i2c1_byte_read(char saddr, char maddr, char* data) {

      volatile int tmp;

	  /* Wait until bus not busy */
	  while (I2C1->SR2 & (SR2_BUSY)){}

	  /* Generate start */
	  I2C1->CR1 |= CR1_START;

	  /* Wait until start flag is set */
	  while (!(I2C1->SR1 & (SR1_SB))){}

	  /* Transmit slave address + Write */
	   I2C1->DR = saddr << 1;

	  /* Wait until addr flag is set */
	   while (!(I2C1->SR1 & (SR1_ADDR))){}

	  /* Clear addr flag */
	  tmp = I2C1->SR2;

      /* Send memory address */
      I2C1->DR = maddr;

      /*Wait until transmitter empty */
     while (!(I2C1->SR1 & SR1_TXE)){}

     /*Generate restart */
      I2C1->CR1 |= CR1_START;

      /* Wait until start flag is set */
     while (!(I2C1->SR1 & SR1_SB)){}

     /* Transmit slave address + Read */
     I2C1->DR = saddr << 1 | 1;

     /* Wait until addr flag is set */
    while (!(I2C1->SR1 & (SR1_ADDR))){}

    /* Disable Acknowledge */
    I2C1->CR1 &= ~CR1_ACK;

    /* Clear addr flag */
    tmp = I2C1->SR2;

    /* Generate stop after data received */
     I2C1->CR1 |= CR1_STOP;

    /* Wait until RXNE flag is set */
    while (!(I2C1->SR1 & SR1_RXNE)){}

    /* Read data from DR */
      *data++ = I2C1->DR;
}

void i2c1_burst_read(char saddr, char maddr, int n, char* data) {

	volatile int tmp;

	 /* Wait until bus not busy */
	  while (I2C1->SR2 & (SR2_BUSY)){}

      /* Generate start */
    I2C1->CR1 |= CR1_START;

    /* Wait until start flag is set */
     while (!(I2C1->SR1 & SR1_SB)){}

     /* Transmit slave address + Write */
     I2C1->DR = saddr << 1;

     /* Wait until addr flag is set */
    while (!(I2C1->SR1 & SR1_ADDR)){}

    /* Clear addr flag */
     tmp = I2C1->SR2;

     /* Wait until transmitter empty */
    while (!(I2C1->SR1 & SR1_TXE)){}

    /*Send memory address */
    I2C1->DR = maddr;

    /*Wait until transmitter empty */
    while (!(I2C1->SR1 & SR1_TXE)){}

    /*Generate restart */
    I2C1->CR1 |= CR1_START;

    /* Wait until start flag is set */
    while (!(I2C1->SR1 & SR1_SB)){}

    /* Transmit slave address + Read */
    I2C1->DR = saddr << 1 | 1;

    /* Wait until addr flag is set */
    while (!(I2C1->SR1 & (SR1_ADDR))){}

    /* Clear addr flag */
    tmp = I2C1->SR2;

    /* Enable Acknowledge */
      I2C1->CR1 |=  CR1_ACK;

    while(n > 0U)
    {
    	/*if one byte*/
    	if(n == 1U)
    	{
    		/* Disable Acknowledge */
    	    I2C1->CR1 &= ~CR1_ACK;

    	    /* Generate Stop */
    	    I2C1->CR1 |= CR1_STOP;

 			/* Wait for RXNE flag set */
            while (!(I2C1->SR1 & SR1_RXNE)){}

            /* Read data from DR */
            *data++ = I2C1->DR;
            break;
    	}
    	else
    	{
       	 /* Wait until RXNE flag is set */
           while (!(I2C1->SR1 & SR1_RXNE)){}

           /* Read data from DR */
           (*data++) = I2C1->DR;

           n--;
    	}
    }

}


void i2c1_burst_write(char saddr, char maddr, int n, char* data) {

	volatile int tmp;

	 /* Wait until bus not busy */
	 while (I2C1->SR2 & (SR2_BUSY)){}

     /* Generate start */
    I2C1->CR1 |= CR1_START;

    /* Wait until start flag is set */
    while (!(I2C1->SR1 & (SR1_SB))){}

    /* Transmit slave address */
    I2C1->DR = saddr << 1;

    /* Wait until addr flag is set */
    while (!(I2C1->SR1 & (SR1_ADDR))){}

    /* Clear addr flag */
    tmp = I2C1->SR2;

    /* Wait until data register empty */
    while (!(I2C1->SR1 & (SR1_TXE))){}

    /* Send memory address */
    I2C1->DR = maddr;

    for (int i = 0; i < n; i++) {

     /* Wait until data register empty */
        while (!(I2C1->SR1 & (SR1_TXE))){}

      /* Transmit memory address */
      I2C1->DR = *data++;
    }

    /* Wait until transfer finished */
    while (!(I2C1->SR1 & (SR1_BTF))){}

    /* Generate stop */
   I2C1->CR1 |= CR1_STOP;
}


void i2c1_burst_read_dma(char saddr, char maddr, int n, char* data) {
    volatile int tmp;

    /* Wait until bus not busy */
    while (I2C1->SR2 & SR2_BUSY){}

    /* Generate start */
    I2C1->CR1 |= CR1_START;
    while (!(I2C1->SR1 & SR1_SB)){}

    /* Transmit slave address + Write */
    I2C1->DR = saddr << 1;
    while (!(I2C1->SR1 & SR1_ADDR)){}
    tmp = I2C1->SR2;

    /* Wait until transmitter empty */
    while (!(I2C1->SR1 & SR1_TXE)){}

    /* Send memory address to read from */
    I2C1->DR = maddr;
    while (!(I2C1->SR1 & SR1_TXE)){}

    /* ---- CONFIGURE DMA FOR RECEPTION ---- */
    /* Ensure DMA is disabled before setting addresses */
    DMA1_Stream0->CR &= ~(1U<<0);
    while(DMA1_Stream0->CR & (1U<<0)){}

    /* Clear DMA1 Stream 0 interrupt flags (TCIF0, HTIF0, TEIF0, DMEIF0, FEIF0) */
    DMA1->LIFCR = 0x3D;

    /* Set Peripheral address (I2C1 Data Register) */
    DMA1_Stream0->PAR = (uint32_t)&(I2C1->DR);

    /* Set Memory address (Your Data Buffer) */
    DMA1_Stream0->M0AR = (uint32_t)data;

    /* Set number of data items to transfer */
    DMA1_Stream0->NDTR = n;

    /* Enable DMA Stream 0 */
    DMA1_Stream0->CR |= (1U<<0);

    /* Enable I2C DMA Requests and NACK generation for last byte */
    I2C1->CR2 |= I2C_CR2_DMAEN;
    I2C1->CR2 |= I2C_CR2_LAST;

    /* Enable Acknowledge */
    I2C1->CR1 |= CR1_ACK;

    /* ---- RESTART I2C IN READ MODE ---- */
    I2C1->CR1 |= CR1_START;
    while (!(I2C1->SR1 & SR1_SB)){}

    /* Transmit slave address + Read */
    I2C1->DR = (saddr << 1) | 1;
    while (!(I2C1->SR1 & SR1_ADDR)){}
    tmp = I2C1->SR2; // Clear ADDR flag

    //The function exits immediately while the hardware handles the transfer.
}


// ---- DMA INTERRUPT HANDLER ----
void DMA1_Stream0_IRQHandler(void) {
    // Check if Transfer Complete Interrupt Flag (TCIF0) is set
    if (DMA1->LISR & (1U<<5)) {

        // 1. Clear the interrupt flag
        DMA1->LIFCR = (1U<<5);

        // 2. Generate I2C Stop Condition
        I2C1->CR1 |= CR1_STOP;

        // 3. Clean up I2C DMA settings
        I2C1->CR2 &= ~I2C_CR2_DMAEN;
        I2C1->CR2 &= ~I2C_CR2_LAST;
        I2C1->CR1 &= ~CR1_ACK;

        // 4. Call the callback to process the data
        i2c1_rx_complete_callback();
    }
}
