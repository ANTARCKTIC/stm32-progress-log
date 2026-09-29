#include "stm32f4xx.h"
#define LED_MASK   (GPIO_BSRR_BS5 | GPIO_BSRR_BS6 | GPIO_BSRR_BS7)
#define LEDS_OFF   (GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7)
/* ---------- Prototypes ---------- */
void CLOCK_Init(void);
void GPIO_Init(void);
void TIMER_Init(void);
volatile uint32_t ticks = 0;

/* ---------- Main ---------- */
int main(void)
{
    CLOCK_Init();
    GPIO_Init();
    TIMER_Init();

    while (1)
    {
        if (ticks >= 10) GPIOA->BSRR = GPIO_BSRR_BS5;
        if (ticks >= 15) GPIOA->BSRR = GPIO_BSRR_BS6;
        if (ticks >= 17) GPIOA->BSRR = GPIO_BSRR_BS7;
        if (ticks >= 60)
        {
        GPIOA->BSRR = LEDS_OFF;
        ticks = 0;
        }
}
}

/* ---------- Clock: HSI 16 MHz -> PLL -> 100 MHz SYSCLK ---------- */
/*  VCO in  = 16 / 8   = 2 MHz
    VCO out = 2 * 100  = 200 MHz
    SYSCLK  = 200 / 2  = 100 MHz
    APB1    = 100 / 4  = 25 MHz  (TIM2 clock = 2x = 50 MHz)
    APB2    = 100 / 1  = 100 MHz                                  */
void CLOCK_Init(void)
{
    // HSI on
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    // Flash wait states first (3 WS for 100 MHz @ 3.3 V), caches on
    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN |
                 FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_3WS;

    // PLL config (PLLSRC = 0 -> HSI, PLLP = 00 -> /2)
    RCC->PLLCFGR = (8U   << RCC_PLLCFGR_PLLM_Pos) |
                   (100U << RCC_PLLCFGR_PLLN_Pos) |
                   (0U   << RCC_PLLCFGR_PLLP_Pos) |
                   (4U   << RCC_PLLCFGR_PLLQ_Pos);

    // Bus prescalers: AHB /1, APB1 /4, APB2 /1
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

    // PLL on
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // Switch SYSCLK to PLL and wait for it
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    // Peripheral clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    (void)RCC->APB1ENR;     // dummy read: let the enables take effect
}

/* ---------- GPIO: PA6 as push-pull output ---------- */


void GPIO_Init(void)
{
    // Drive the output latch low BEFORE the pins become outputs (no glitch on startup)
    GPIOA->BSRR = LEDS_OFF;

    // Mode: 01 = general purpose output
    GPIOA->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
    GPIOA->MODER |=  (GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0);

    // Output type: push-pull
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);

    // Speed: low is plenty for LEDs and reduces edge noise
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7);

    // Pull: 10 = pull-down
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);
    GPIOA->PUPDR |=  (GPIO_PUPDR_PUPD5_1 | GPIO_PUPDR_PUPD6_1 | GPIO_PUPDR_PUPD7_1);
}

/* ---------- TIM2: update interrupt at 10 Hz ---------- */
/*  f = 50 MHz / ((PSC+1) * (ARR+1)) = 50 MHz / 5,000,000 = 10 Hz */
void TIMER_Init(void)
{
    TIM2->PSC = 0;
    TIM2->ARR = 4999999;
    TIM2->EGR = TIM_EGR_UG;          // load PSC/ARR now
    TIM2->SR  = 0;                   // clear the flag that UG just set
    TIM2->DIER |= TIM_DIER_UIE;      // update interrupt enable

    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;        // start
}

/* ---------- Interrupt handlers (yours to fill) ---------- */
void TIM2_IRQHandler(void)
{
    // remember to clear TIM2->SR UIF here, or it retriggers forever
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;

        ticks++;
    }
}