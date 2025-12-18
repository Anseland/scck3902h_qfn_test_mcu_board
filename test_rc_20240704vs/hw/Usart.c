/**
  ******************************************************************************
  * @file     usart.c
  * @author   lianshuang@memsplus.com
  * @version  V1.0.0
  * @date     27-Feb-2014
  * @brief    USART common.
  ******************************************************************************/

#include "Usart.h"
#include "delay.h"
#include "ProjectCfg.h"


/*******************************************************************************
* 函数名  		: USART_Configuration
* 函数描述    	: 设置USART1
* 输入参数      : None
* 输出结果      : None
* 返回值        : None
*******************************************************************************/
void USART_Configuration(void)
{
    /* 设置 USART IO端口*/
    /* 定义 GPIO 初始化结构体 GPIO_InitStructure */
    GPIO_InitTypeDef GPIO_InitStructure;
    /* 定义USART初始化结构体 USART_InitStructure */
    USART_InitTypeDef USART_InitStructure;
    
    /* 设置USART1的Tx脚（PA.9）为第二功能推挽输出模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA , &GPIO_InitStructure);
    /* 设置USART1的Rx脚（PA.10）为浮空输入脚 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA , &GPIO_InitStructure);
    
    /*	
    *	波特率为115200bps
    *	8位数据长度
    *	1个停止位，无校验
    *	禁用硬件流控制
    *	禁止USART时钟
    *	时钟极性低
    *	在第2个边沿捕获数据
    *	最后一位数据的时钟脉冲不从 SCLK 输出
    */ 
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(STDIO_UART , &USART_InitStructure);
    
    /* 使能USART1 */
    USART_Cmd(STDIO_UART , ENABLE);
}


/**
  * @brief  USART configuration
  */
void USART_Config(USART_TypeDef*  USARTx, u32 br)
{
        GPIO_InitTypeDef    GPIO_InitStructure;
        USART_InitTypeDef   USART_InitStructure;
        u8                  i;

        for(i = 0; i < (sizeof(USARTX)/sizeof(USARTX[0])); i++){
            if(USARTx == USARTX[i].USARTx)
                break;
        };

		/* config USART clock */
        if(USARTX[i].USARTx == USART1)
            RCC_APB2PeriphClockCmd(USARTX[i].RCCVal, ENABLE);
        else
            RCC_APB1PeriphClockCmd(USARTX[i].RCCVal, ENABLE);

		/* USART GPIO config */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | USARTX[i].Tx.RCCVal | USARTX[i].Rx.RCCVal, ENABLE);

        /* Configure USART Tx as alternate function push-pull */
        GPIO_InitStructure.GPIO_Pin = USARTX[i].Tx.GPIO_Pin;
        GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(USARTX[i].Tx.GPIOx, &GPIO_InitStructure);

        /* Configure USART Rx as input floating */
        GPIO_InitStructure.GPIO_Pin = USARTX[i].Rx.GPIO_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPD;//
        GPIO_Init(USARTX[i].Rx.GPIOx, &GPIO_InitStructure);

        /* USART mode config */
        USART_InitStructure.USART_BaudRate = br;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No ;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USARTX[i].USARTx, &USART_InitStructure);
        
        /* 使能USART1 */
        USART_Cmd(USARTX[i].USARTx, ENABLE);
        
            
    /* 开启USART1和GPIOA时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA , ENABLE);
}

s8 USART_ConfigInt(USART_TypeDef* USARTx, u16 USART_IT, FunctionalState NewState)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    if(USARTx == USART1)
            NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    else if(USARTx == USART2)
            NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    else if(USARTx == USART3)
            NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    else
            return -1;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = RS232_USART_IRQ_PP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = RS232_USART_IRQ_SP;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    //Initializes the NVIC peripheral according to the specified
    //parameters in the NVIC_InitStruct.
    NVIC_Init(&NVIC_InitStructure);

    //Enables or disables the specified USART interrupts.
    USART_ITConfig(USARTx, USART_IT, NewState);

    return 0;
}

int putcharTo(USART_TypeDef *UART, int ch)
{
    /* 发送一个字节数据到USART1 */
    USART_SendData(UART, (u8)ch);

    /* 等待发送完毕 */
    while (USART_GetFlagStatus(UART, USART_FLAG_TXE) == RESET);

    return (ch);
}

int getcharTimeoutFrom(USART_TypeDef *UART, u16 timeout_ms)
{
    u16 ms = timeout_ms + 1;

    do{
        if(USART_GetFlagStatus(UART, USART_FLAG_RXNE) == SET){
            return (int)USART_ReceiveData(UART);
        }
        mDelay(1);
        if(timeout_ms != 0){
            ms--;
        }
    }while(ms!=0);

    return (int)-1;
}

int getcharFrom(USART_TypeDef *UART)
{
    /* 等待串口1输入数据 */
    while (USART_GetFlagStatus(UART, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(UART);
}

void UASRT_Flush(USART_TypeDef *UART)
{
    /* clear receive buffer */
    while(USART_GetFlagStatus(UART, USART_FLAG_RXNE) == SET){
        USART_ReceiveData(UART);
    }
}

///重定向c库函数printf到USART
int putchar(int ch)
{
        return putcharTo(STDIO_UART, ch);
}

///重定向c库函数scanf到USART
int getchar(void)
{
        return getcharFrom(STDIO_UART);
}
