#include "tim.h"

volatile uint8_t timer_trigger_flag = 0;

// --- Timer 2 Initialization ---
void tim2_init(void)
{
    // 1. Enable clock access to TIM2 (APB1 bus)
    RCC->APB1ENR |= (1U<<0);

    // 2. Set Prescaler value to get 1 millisecond per tick
    // System clock is 16 MHz. 16,000,000 / 16,000 = 1,000 Hz (1 ms)
    TIM2->PSC = 16000 - 1;

    // 3. Set Auto-Reload value for 50ms
    TIM2->ARR = 50 - 1;

    // 4. Clear the counter
    TIM2->CNT = 0;

    // 5. Enable the TIM2 Update Interrupt
    TIM2->DIER |= (1U<<0);

    // 6. Enable the TIM2 Interrupt in the NVIC (Nested Vectored Interrupt Controller)
    NVIC_EnableIRQ(TIM2_IRQn);

    // 7. Enable Timer 2 counter
    TIM2->CR1 |= (1U<<0);
}

// --- Timer 2 Interrupt Handler ---
void TIM2_IRQHandler(void)
{
    // Check if the update interrupt flag (UIF) is set
    if (TIM2->SR & (1U<<0)) {

        // Clear the update interrupt flag
        TIM2->SR &= ~(1U<<0);

        // Tell the main loop it's time to read the sensor
        timer_trigger_flag = 1;
    }
}
