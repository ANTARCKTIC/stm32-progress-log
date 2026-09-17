#include "stm32f4xx.h"

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

    GPIOA->MODER &= ~(GPIO_MODER_MODER6_Msk | GPIO_MODER_MODER7_Msk);
    GPIOA->MODER |= GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0;

    GPIOC->MODER &= ~(GPIO_MODER_MODER5_Msk | GPIO_MODER_MODER6_Msk);

    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD5_Msk | GPIO_PUPDR_PUPD6_Msk);
    GPIOC->PUPDR |= GPIO_PUPDR_PUPD5_1 | GPIO_PUPDR_PUPD6_0;

    while (1)
    {
        uint32_t inputs = GPIOC->IDR;

        uint32_t pc5 = (inputs & GPIO_IDR_ID5) >> 5;
        uint32_t pc6 = (inputs & GPIO_IDR_ID6) >> 6;

        GPIOA->ODR = (GPIOA->ODR & ~(GPIO_ODR_OD6 | GPIO_ODR_OD7))
                   | (pc5 << 6)
                   | (pc6 << 7);
    }
}