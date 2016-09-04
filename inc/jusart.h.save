#ifndef _JUSART_H_
#define _JUSART_H_

#include "stm32f0xx.h"

#define USART_Int					(uint32_t)(1 << USART1_IRQn)
//#define USART_Int					0x08000000
//#define USART1_TX_Interrupt			(uint32_t)(1 << USART_CR1_TXEIE)
#define USART1_TX_Interrupt		0x00000080

#define USART1_TX_Interrupt_Enable	(USART1->CR1 |= USART1_TX_Interrupt)
#define USART1_TX_Interrupt_Disable	(USART1->CR1 &= ~(USART1_TX_Interrupt))
#define USART1_Interrupt_Enable		(NVIC->ISER[0] = USART_Int)
#define USART1_Interrupt_Disable	(NVIC->ICER[0] = USART_Int)


void jusart_Config(void);
void jusart_Puts( char * CH);
uint8_t jusart_ItoA(int value, char* buffer, uint8_t zero_pad);
//uint8_t jusart_Scanf(uint32_t value);
//char getchar(void);
void jusart_GetLine(void);
uint8_t jusart_RXCR(void);




void USART1_IRQHandler(void);


#endif 		// _JUSART_H_
