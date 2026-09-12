#include "stm32f4xx.h"





int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0_Msk);
GPIOC->PUPDR |= GPIO_PUPDR_PUPD0_1;

    GPIOA->MODER &= ~(2U << 10); /* reset PA5 bits to 0 , msk = 110000000000 , ~ not function  , MODER = 00xxxxxxxxxx*/
    GPIOA->MODER |= (1U <<10);  /*| or function , MODER 01xxxxxxxxxx , PA5 is now in output mode*/

    while (1)
    {
        if (GPIOC->IDR & 1U)
        
        {GPIOA->ODR |= (1U<<5); /*toggle on and off bit 5*/
            

        }
        else
        {
    // PC0 is currently 0 → button released
             GPIOA->ODR &= ~(1U<<5);
        }
      
        
    }
}

void SysTick_Handler(void) {}