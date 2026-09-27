#include "stm32f4xx.h"
#include "core_cm4.h"

void GPIO_Init(void);
void EXTI0_Init(void);
void NVIC_Init(void);
void delay(volatile uint32_t count);

void EXTI0_IRQHandler(void);

int main(void)
{
    GPIO_Init();
    EXTI0_Init();
    NVIC_Init();

    while (1)
    {
        GPIOA->ODR ^= GPIO_ODR_OD6;
        delay(1000000);
    }
}

void GPIO_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN;

    // PA6, PA7 -> output
    GPIOA->MODER &= ~(GPIO_MODER_MODER6_Msk |
                      GPIO_MODER_MODER5_Msk |
                      GPIO_MODER_MODER7_Msk);

    GPIOA->MODER |= GPIO_MODER_MODER6_0 |
                    GPIO_MODER_MODER5_0 |
                    GPIO_MODER_MODER7_0;

    // PC0 -> input with pull-down
    GPIOC->MODER &= ~GPIO_MODER_MODER0_Msk;

    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD0_Msk;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPD0_1;


    GPIOA->ODR = GPIO_ODR_OD5;
}

void EXTI0_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // Connect EXTI0 to PC0
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC;

    // Enable EXTI0
    EXTI->IMR |= EXTI_IMR_MR0;

    // Rising edge
    EXTI->RTSR |= EXTI_RTSR_TR0;

    // Falling edge disabled
    EXTI->FTSR |= EXTI_FTSR_TR0;

    // Clear pending flag
    EXTI->PR = EXTI_PR_PR0;
}

void NVIC_Init(void)
{
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
    NVIC_SetPriority(EXTI0_IRQn, 5);
    NVIC_EnableIRQ(EXTI0_IRQn);
}

void delay(volatile uint32_t count)
{
    while (count--)
    {
        __NOP();
    }
}

void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR0)
    {
        EXTI->PR = EXTI_PR_PR0;

         if (GPIOC->IDR & GPIO_IDR_ID0)
        {
            GPIOA->ODR ^= GPIO_ODR_OD7;
        }
        else
        {
            GPIOA->ODR ^= GPIO_ODR_OD5;
        }

    }
}

void SysTick_Handler(void) {}