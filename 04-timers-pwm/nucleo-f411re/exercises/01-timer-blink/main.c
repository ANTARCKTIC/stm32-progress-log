#include "stm32f4xx.h"
#include "core_cm4.h"


void CLOCK_Init(void);
void TIMER_Init(void);
void GPIO_Init(void);

void NVIC_Init(void);
void delay(volatile uint32_t count);



int main(void)
{   
    CLOCK_Init();
    GPIO_Init();
    EXTI0_Init();
    NVIC_Init();

    while (1)
    {
        GPIOA->ODR ^= GPIO_ODR_OD6;
        delay(1000000);
    }
}

void CLOCK_Init(void)
{
    RCC->PLLCFGR |= (16U << RCC_PLLCFGR_PLLM_Pos );
    RCC->PLLCFGR &= ~(0x1FFU << RCC_PLLCFGR_PLLN_Pos);
    RCC->PLLCFGR |= (400U << RCC_PLLCFGR_PLLN_Pos );
    RCC->PLLCFGR |= (0x1U << RCC_PLLCFGR_PLLP_Pos );
    RCC->CFGR |= (RCC_CFGR_PPRE1_DIV4 << RCC_CFGR_PPRE1_Pos);
    RCC->CFGR |= (RCC_CFGR_SW_PLL << RCC_CFGR_SW_Pos   );


    RCC->CR |= RCC_CR_HSION ;
    while(!(RCC->CR & RCC_CR_HSIRDY));
    RCC->CR |= RCC_CR_PLLON ;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN);
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

}
void TIMER_Init(void){
    
}