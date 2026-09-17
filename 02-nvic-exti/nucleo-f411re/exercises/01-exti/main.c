#include "stm32f4xx.h"
#include "core_cm4.h"
int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    GPIOA->MODER &= ~(GPIO_MODER_MODER6_Msk | GPIO_MODER_MODER7_Msk);
    GPIOA->MODER |= GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0;

    GPIOC->MODER &= ~(GPIO_MODER_MODER0_Msk);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0_Msk);
    GPIOC->PUPDR |= GPIO_PUPDR_PUPD0_0;

    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC;

    EXTI->IMR |= EXTI_IMR_MR0;
    EXTI->FTSR |= EXTI_FTSR_TR0;
    EXTI->RTSR &= ~(EXTI_RTSR_TR0);

    EXTI->PR = EXTI_PR_PR0;
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
    NVIC_SetPriority(EXTI0_IRQn, 5);
    NVIC_EnableIRQ(EXTI0_IRQn);

    while (1)
    {
        GPIOA->ODR ^= GPIO_ODR_OD6;
        for (volatile uint32_t i = 0; i < 1000000; i++) { }
    }
}
void EXTI0_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR = EXTI_PR_PR0;
        GPIOA->ODR ^= GPIO_ODR_OD7;
        for (volatile uint32_t i = 0; i < 5000000; i++) { } 
        GPIOA->ODR ^= GPIO_ODR_OD7;  // no delay loops for now, keep it simple
    }
}

void SysTick_Handler(void) {}