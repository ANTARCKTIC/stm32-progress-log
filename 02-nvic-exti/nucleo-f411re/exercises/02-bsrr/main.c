#include "stm32f4xx.h"

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER &= ~(GPIO_MODER_MODER5_Msk); /* reset PA5 bits to 0 , msk = 110000000000 , ~ not function  , MODER = 00xxxxxxxxxx*/
    GPIOA->MODER |= GPIO_MODER_MODER5_0;  /*| or function , MODER 01xxxxxxxxxx , PA5 is now in output mode*/
    uint8_t x= 1U;
    while (1)
    {
        x ^= 1U;
        GPIOA->BSRR = ( x << 5 ) | ((!x) << (5+16)) ;/*toggle on and off bit 5*/

        for (volatile uint32_t i = 0; i < 250000; i++) { }
    }
}

void SysTick_Handler(void) {}