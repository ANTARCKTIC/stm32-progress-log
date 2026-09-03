#include "stm32f4xx.h"

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER &= ~(GPIO_MODER_MODER5_Msk);
    GPIOA->MODER |= GPIO_MODER_MODER5_0;

    while (1)
    {
        GPIOA->ODR ^= GPIO_ODR_OD5;

        for (volatile uint32_t i = 0; i < 200000; i++) { }
    }
}

void SysTick_Handler(void) {}