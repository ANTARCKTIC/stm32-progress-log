#include "stm32f4xx.h"

/* ---------- Config ---------- */
#define TIM_PSC        49U      /* 50 MHz / 50 = 1 MHz tick */
#define TIM_ARR        999U     /* 1 MHz / 1000 = 1 kHz PWM */

#define DUTY_CH1       65U     /* PB6 -> 50% */
#define DUTY_CH2       400U     /* PB7 -> 80% */
#define DUTY_CH3       1000U     /* PB8 -> 10% */

#define PWM_AF         2U       /* AF2 = TIM3..TIM5 */

/* ---------- Prototypes ---------- */
static void CLOCK_Init(void);
static void GPIO_Init(void);
static void TIMER_Init(void);

int main(void)
{
    CLOCK_Init();
    GPIO_Init();
    TIMER_Init();

    while (1)
    {
        /* timer hardware drives the pins */
    }
}

/* ---------- Clock: HSI 16 MHz -> PLL -> 100 MHz SYSCLK ---------- */
static void CLOCK_Init(void)
{
    /* Voltage scaling: needed for 100 MHz (check your device's reference manual) */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN |
                 FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_3WS;

    /* HSI/8 = 2 MHz, x100 = 200 MHz VCO, /2 = 100 MHz */
    RCC->PLLCFGR = (8U   << RCC_PLLCFGR_PLLM_Pos) |
                   (100U << RCC_PLLCFGR_PLLN_Pos) |
                   (0U   << RCC_PLLCFGR_PLLP_Pos) |
                   (4U   << RCC_PLLCFGR_PLLQ_Pos);

    /* AHB /1, APB1 /4 (25 MHz, timers x2 = 50 MHz), APB2 /1 */
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    (void)RCC->APB1ENR;     /* dummy read: let the clock enable settle */
}

/* ---------- GPIO: PB6/PB7/PB8 = TIM4 CH1/CH2/CH3 (AF2) ---------- */
static void GPIO_Init(void)
{
    /* Mode = 10 (alternate function). Two bits per pin. */
    GPIOB->MODER &= ~((3U << (2*6)) | (3U << (2*7)) | (3U << (2*8)));
    GPIOB->MODER |=  ((2U << (2*6)) | (2U << (2*7)) | (2U << (2*8)));

    /* Push-pull, low speed, no pull */
    GPIOB->OTYPER  &= ~((1U << 6) | (1U << 7) | (1U << 8));
    GPIOB->OSPEEDR &= ~((3U << (2*6)) | (3U << (2*7)) | (3U << (2*8)));
    GPIOB->PUPDR   &= ~((3U << (2*6)) | (3U << (2*7)) | (3U << (2*8)));

    /* AFR[0] covers pins 0-7 (4 bits each): PB6, PB7 */
    GPIOB->AFR[0] &= ~((0xFU << (4*6)) | (0xFU << (4*7)));
    GPIOB->AFR[0] |=  ((PWM_AF << (4*6)) | (PWM_AF << (4*7)));

    /* AFR[1] covers pins 8-15: PB8 is index 0 here, not 8 */
    GPIOB->AFR[1] &= ~(0xFU << (4*(8-8)));
    GPIOB->AFR[1] |=  (PWM_AF << (4*(8-8)));
}

/* ---------- TIM4: 1 kHz, three PWM channels ---------- */
static void TIMER_Init(void)
{
    TIM4->PSC = TIM_PSC;
    TIM4->ARR = TIM_ARR;

    TIM4->CCR1 = DUTY_CH1;   /* PB6 */
    TIM4->CCR2 = DUTY_CH2;   /* PB7 */
    TIM4->CCR3 = DUTY_CH3;   /* PB8 */

    /* CH1 + CH2 are in CCMR1: output mode, PWM mode 1 (110), preload on */
    TIM4->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M |
                     TIM_CCMR1_CC2S | TIM_CCMR1_OC2M);
    TIM4->CCMR1 |=  (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE |
                     TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE);

    /* CH3 is in CCMR2 */
    TIM4->CCMR2 &= ~(TIM_CCMR2_CC3S | TIM_CCMR2_OC3M);
    TIM4->CCMR2 |=  (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE);

    /* Enable the three outputs */
    TIM4->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E);

    TIM4->CR1 |= TIM_CR1_ARPE;   /* buffer ARR */
    TIM4->EGR  = TIM_EGR_UG;     /* load PSC/ARR/CCRx into shadow registers */
    TIM4->SR   = 0;

    TIM4->CR1 |= TIM_CR1_CEN;
}