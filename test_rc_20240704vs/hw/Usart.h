#ifndef __USART1_H
#define	__USART1_H

#include "libcfg.h"
#include <stdio.h>
#include "ProjectCfg.h"

#define STDIO_UART          USART1

#define RCC_USART1_GPIO         RCC_APB2Periph_GPIOA
#define USART1_GPIO             GPIOA
#define USART1_TX               GPIO_Pin_9
#define USART1_RX               GPIO_Pin_10

#define RCC_USART2_GPIO         RCC_APB2Periph_GPIOA
#define USART2_GPIO             GPIOA
#define USART2_TX               GPIO_Pin_2
#define USART2_RX               GPIO_Pin_3

#define RCC_USART3_GPIO         RCC_APB2Periph_GPIOB
#define USART3_GPIO             GPIOB
#define USART3_TX               GPIO_Pin_10
#define USART3_RX               GPIO_Pin_11

struct port_st {

    u32             RCCVal;
    GPIO_TypeDef*   GPIOx;
    u16             GPIO_Pin;
};

struct usart_st {

    u32             RCCVal;
    USART_TypeDef*  USARTx;
    struct port_st  Tx;
    struct port_st  Rx;
};

//static const struct usart_st USARTX[] = { { RCC_APB2Periph_USART1, USART1, { RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_9 },  { RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_10 } },
//                                          { RCC_APB1Periph_USART2, USART2, { RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_2 },  { RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_3 }  },
//                                          { RCC_APB1Periph_USART3, USART3, { RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_10 }, { RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_11 } },
//                                        };

static const struct usart_st USARTX[] = {   { RCC_APB2Periph_USART1, USART1, { RCC_USART1_GPIO, USART1_GPIO, USART1_TX },  { RCC_USART1_GPIO, USART1_GPIO, USART1_RX } },
                                            { RCC_APB1Periph_USART2, USART2, { RCC_USART2_GPIO, USART2_GPIO, USART2_TX },  { RCC_USART2_GPIO, USART2_GPIO, USART2_RX }  },
                                            { RCC_APB1Periph_USART3, USART3, { RCC_USART3_GPIO, USART3_GPIO, USART3_TX },  { RCC_USART1_GPIO, USART3_GPIO, USART3_RX } },
                                    };


void USART_Configuration(void);
void USART_Config(USART_TypeDef* USARTx, u32 br);

s8 USART_ConfigInt(USART_TypeDef* USARTx, u16 USART_IT, FunctionalState NewState);
int putcharTo(USART_TypeDef *UART, int ch);
int getcharFrom(USART_TypeDef *UART);
int getcharTimeoutFrom(USART_TypeDef *UART, u16 timeout_ms);
void UASRT_Flush(USART_TypeDef *UART);

#endif /* __USART1_H */
