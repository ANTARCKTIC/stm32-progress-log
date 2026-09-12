#include "stm32f4xx.h"

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER &= ~(GPIO_MODER_MODER5_Msk); /* reset PA5 bits to 0 , msk = 110000000000 , ~ not function  , MODER = 00xxxxxxxxxx*/
    GPIOA->MODER |= GPIO_MODER_MODER5_0;  /*| or function , MODER 01xxxxxxxxxx , PA5 is now in output mode*/

    while (1)
    {
        GPIOA->ODR ^= GPIO_ODR_OD5; /*toggle on and off bit 5*/

        for (volatile uint32_t i = 0; i < 1000000; i++) { }
    }
}

void SysTick_Handler(void) {}