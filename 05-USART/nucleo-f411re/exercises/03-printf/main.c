#include "stm32f4xx.h"
#include "stdio.h"
static volatile uint32_t ms_ticks = 0;
void uart2_init(uint32_t baud);
void uart2_write(char c);
void SysTick_Handler(void)
{
    ms_ticks++;
}
int _write(int fd, char *buf, int len)
{
    for (int i = 0; i < len; i++) {
        if (buf[i] == '\n') uart2_write('\r');   /* terminals want \r\n */
        uart2_write(buf[i]);
    }
    return len;
}

static void delay_ms(uint32_t ms)
{
    uint32_t start = ms_ticks;
    while ((ms_ticks - start) < ms) { }
}

static void uart2_gpio_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* PA2/PA3 -> alternate function (10) */
    GPIOA->MODER &= ~((3U << 4) | (3U << 6));
    GPIOA->MODER |=  ((2U << 4) | (2U << 6));

    /* AF7 = USART2 */
    GPIOA->AFR[0] &= ~((0xFU << 8) | (0xFU << 12));
    GPIOA->AFR[0] |=  ((7U   << 8) | (7U   << 12));

    /* pull-up on RX so it doesn't float */
    GPIOA->PUPDR &= ~(3U << 6);
    GPIOA->PUPDR |=  (1U << 6);
}

static void uart2_periph_init(uint32_t baud)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    USART2->CR1 &= ~USART_CR1_UE;

    /* fck / baud, rounded. Assumes APB1 = 16 MHz (HSI, no PLL) */
    USART2->BRR = (SystemCoreClock + baud / 2U) / baud;

    USART2->CR1 = USART_CR1_TE | USART_CR1_RE;   /* 8N1 by default */
    USART2->CR2 = 0;
    USART2->CR3 = 0;
    USART2->CR1 |= USART_CR1_UE;
}

void uart2_init(uint32_t baud)
{
    uart2_gpio_init();
    uart2_periph_init(baud);
}

void uart2_write(char c)
{
    while (!(USART2->SR & USART_SR_TXE)) { }     /* TXE: DR free */
    USART2->DR = (uint8_t)c;
}

void uart2_write_string(const char *s)
{
    while (*s) {
        uart2_write(*s++);
    }
}

void uart2_flush(void)
{
    while (!(USART2->SR & USART_SR_TC)) { }      /* TC: last bit sent */
}

int uart2_available(void)
{
    return (USART2->SR & USART_SR_RXNE) != 0;    /* RXNE: byte received */
}

char uart2_read(void)
{
    while (!uart2_available()) { }
    return (char)USART2->DR;                     /* reading DR clears RXNE */
}

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000U);
    uart2_init(115200);

    while (1) {
        printf("tick %lu\n", ms_ticks);
        delay_ms(1000);
    }
}