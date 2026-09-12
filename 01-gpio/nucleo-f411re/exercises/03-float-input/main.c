#include "stm32f4xx.h"

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

    GPIOA->MODER &= ~(GPIO_MODER_MODER5_Msk); /* reset PA5 bits to 0 , msk = 110000000000 , ~ not function  , MODER = 00xxxxxxxxxx*/
    GPIOA->MODER |= GPIO_MODER_MODER5_0;  /*| or function , MODER 01xxxxxxxxxx , PA5 is now in output mode*/

    GPIOC->MODER &= ~(GPIO_MODER_MODER5_Msk); 
    
    
    while (1)
    {
        uint32_t pc5 = (GPIOC->IDR >> 5) & 1U;
        GPIOA->ODR = (GPIOA->ODR & ~GPIO_ODR_OD5) | (pc5 << 5);

        for (volatile uint32_t i = 0; i < 1000000; i++) { }
    }
}

void SysTick_Handler(void) {}