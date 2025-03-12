#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>


#define RCC_BASE_ADDR		0x40023800
#define SYSCFG_BASE_ADDR	0x40013800
#define EXTI_BASE_ADDR		0x40013C00
#define NVIC_ISER_BASE_ADDR	0xE000E100
#define GPIOA_BASE_ADDR		0x40020000
#define GPIOD_BASE_ADDR         0x40020C00
#define GPIOB_BASE_ADDR		0x40020400
#define USART1_BASE_ADDR	0x40011000

void setHighBlueLed()
{
	uint32_t *GPIOD_ODR = (uint32_t*)(GPIOD_BASE_ADDR + 0x14);
	*GPIOD_ODR |= (1U << 15);
}

void setLowBlueLed()
{
	uint32_t *GPIOD_ODR = (uint32_t*)(GPIOD_BASE_ADDR + 0x14);
	*GPIOD_ODR &= ~ (1U << 15);
}

bool readStatusButton()
{
	uint32_t *GPIOA_IDR = (uint32_t*)(GPIOA_BASE_ADDR + 0x10);
	return (*GPIOA_IDR & 1);
}
void init_usart1()
{
	// config clock gpio b
	uint32_t *RCC_AHB1ENR = (uint32_t*)(RCC_BASE_ADDR + 0x30);
	*RCC_AHB1ENR |= (1U << 1);

	uint32_t *GPIOB_MODER = (uint32_t*)(GPIOB_BASE_ADDR);
	*GPIOB_MODER |= (2U << 12);

	uint32_t *GPIOB_AFRL = (uint32_t*)(GPIOB_BASE_ADDR + 0x20);
	*GPIOB_AFRL |= (7U << 24);

	// config clock usart1
	uint32_t *RCC_APB2ENR = (uint32_t*)(RCC_BASE_ADDR + 0x44);
	*RCC_APB2ENR |= (1U << 4);

	uint32_t *USART1_CR1 = (uint32_t*)(USART1_BASE_ADDR + 0x0C);
	*USART1_CR1 &= ~ (1U << 15);

	*USART1_CR1 &= ~ (1U << 12);

	uint32_t *USART1_CR2 = (uint32_t*)(USART1_BASE_ADDR + 0x10);
	*USART1_CR2 &= ~ (3U << 12);

	uint32_t *USART1_BRR = (uint32_t*)(USART1_BASE_ADDR + 0x08);
	*USART1_BRR = (8U << 4) | (11U << 0);

	*USART1_CR1 |= (1U << 3);
	*USART1_CR1 |= (1U << 13);
}

void transmissionEachByte(char *buf)
{
	uint32_t *USART1_DR = (uint32_t*)(USART1_BASE_ADDR + 0x04);
	uint32_t *USART1_SR = (uint32_t*)USART1_BASE_ADDR;
	while(*buf)
	{
		while(! ( (*USART1_SR >> 6) & 1 ) );
		*USART1_DR = *buf++;
		while(!(*USART1_SR & (1U << 7)));

	}
}

void myPrintf(const char *fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	char buffer[256];
	vsnprintf(buffer,sizeof(buffer),fmt,args);
	transmissionEachByte(buffer);
	va_end(args);
}

void my_handler()
{
	if(readStatusButton())
	{
		setHighBlueLed();
		myPrintf("hello\n");
	}
	else
	{
		setLowBlueLed();
		myPrintf("bye\n");
	}

	uint32_t *EXTI_PR = (uint32_t*)(EXTI_BASE_ADDR + 0x14);
	*EXTI_PR |= (1U << 0);
}

void init_exti0()
{
	uint32_t *RCC_APB2ENR = (uint32_t*)(RCC_BASE_ADDR + 0x44);
	*RCC_APB2ENR |= (1U << 14);

	uint32_t *SYSCFG_EXTICR1 = (uint32_t*)(SYSCFG_BASE_ADDR + 0x08);
	*SYSCFG_EXTICR1 &= ~ (0xF << 0);

	uint32_t *EXTI_IMR = (uint32_t*)(EXTI_BASE_ADDR);
	*EXTI_IMR |= (1U << 0);

	uint32_t *EXTI_RTSR = (uint32_t*)(EXTI_BASE_ADDR + 0x08);
	*EXTI_RTSR |= (1U << 0);

	uint32_t *EXTI_FTSR = (uint32_t*)(EXTI_BASE_ADDR + 0x0C);
	*EXTI_FTSR |= (1U << 0);

	uint32_t *NVIC_ISER0 = (uint32_t*)(NVIC_ISER_BASE_ADDR);
	*NVIC_ISER0 |= (1U << 6);

	char *vector_table = 0x00;
	char *ram = (char*)0x20000000;

	for(int i = 0; i < 0x194; i++)
	{
		ram[i] = vector_table[i];
	}
	uint32_t *VTOR = (uint32_t*)0xE000ED08;
	*VTOR = 0x20000000;

	uint32_t *ptr = (uint32_t*)0x20000058;
	*ptr = (uint32_t)my_handler;
}
void initButton()
{
	uint32_t *RCC_AHB1ENR = (uint32_t*)(RCC_BASE_ADDR + 0x30);
	*RCC_AHB1ENR |= (1U << 0);

	uint32_t *GPIOA_MODER = (uint32_t*)(GPIOA_BASE_ADDR);
	*GPIOA_MODER &= ~ (3U << 0);
}
void initBlueLed()
{
	uint32_t *RCC_AHB1ENR = (uint32_t*)(RCC_BASE_ADDR + 0x30);
	*RCC_AHB1ENR |= (1U << 3);

	uint32_t *GPIOD_MODER = (uint32_t*)(GPIOD_BASE_ADDR);
	*GPIOD_MODER |= (0b01 << 30);
}

int main(void)
{
	init_exti0();
	initButton();
	initBlueLed();
	init_usart1();
	while(1);
}
