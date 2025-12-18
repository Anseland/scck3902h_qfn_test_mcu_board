/********************************************************************************
  * @file     board.c
  * @author   Jackie
  * @version  V1.0.0
  * @date     16-Jul-2014
  * @brief    board level initilization.
  *
  ******************************************************************************/
#include "board.h"
#include "delay.h"
#include "usart.h"
#include "SPI_GPIO.h"
//#include "I2C.h"
//#include "SPI.h"
#include "stm32f10x_iwdg.h"


#ifdef USE_HSI
static void RCC_Configuration_HSI(void)
{
    RCC_DeInit();                                                               //将外设RCC寄存器重设为缺省值

    RCC_HSICmd(ENABLE);                                                         //使能内部高速晶振
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);                         //等待高速晶振稳定

    RCC_HSEConfig(RCC_HSE_OFF);
    RCC_LSEConfig(RCC_LSE_OFF);

    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);                       //预取指缓存使能

    FLASH_SetLatency(FLASH_Latency_2);                                          //设置代码延时值为2个时钟周期

    RCC_HCLKConfig(RCC_SYSCLK_Div1);                                            //设置AHB时钟=系统时钟，Configures the AHB clock (HCLK).

    RCC_PCLK2Config(RCC_HCLK_Div1);                                             //设置高速AHB时钟=系统时钟，Configures the High Speed APB clock (PCLK2).

    RCC_PCLK1Config(RCC_HCLK_Div2);                                             //设置低速AHB时钟=系统时钟/2，Configures the Low Speed APB clock (PCLK1).

    RCC_PLLConfig(RCC_PLLSource_HSI_Div1, RCC_PLLMul_9);                       //设置PLL时钟源=内部高速晶体 倍频系数为9  PLLCLK=8/1*9=72
                                                                                //Configures the PLL clock source and multiplication factor.

    RCC_PLLCmd(ENABLE);                                                         //使能PLL

    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)                          //等待PLL稳定
    {
    }

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);                                  //选择PLL输出时钟作为系统时钟

    while(RCC_GetSYSCLKSource() != 0x08)                                       //等待时钟稳定
    {
    }
    
    /* 开启GPIOB时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    
    /* 开启USART1和GPIOA时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA , ENABLE);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
}
#endif /* USE_HSI */

/*******************************************************************************
* 函数名 	: RCC_Configuration
* 函数描述  	: 设置系统各部分时钟
* 输入参数  	: 无
* 输出结果  	: 无
* 返回值    	: 无
*******************************************************************************/
void RCC_Configuration(void)
{ 
    /* 定义枚举类型变量 HSEStartUpStatus */
    ErrorStatus HSEStartUpStatus;
    /* 复位系统时钟设置 */
    RCC_DeInit();
    
    /* 开启HSE */
    RCC_HSEConfig(RCC_HSE_ON);
    /* 等待HSE起振并稳定 */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    /* 判断HSE起是否振成功,是则进入if()内部 */
    if(HSEStartUpStatus==SUCCESS)
    {
            /* 选择HCLK(AHB)时钟源为SYSCLK1分频  */
            RCC_HCLKConfig(RCC_SYSCLK_Div1);
            /* 选择PCLK2时钟源为 HCLK(AHB)1分频  */
            RCC_PCLK2Config(RCC_HCLK_Div1);
            /* 选择PCLK1时钟源为 HCLK(AHB)2分频  */
            RCC_PCLK1Config(RCC_HCLK_Div2);
            
            /* 设置Flash延时周期数为2 */
            FLASH_SetLatency(FLASH_Latency_2);
            /* 使能Flash预取缓存 */
            FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
            
            /* 选择PLL时钟源为HSE1分频,倍频数为9,则PLL=8MHz×9=72MHz */
            RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
            /* 使能PLL */
            RCC_PLLCmd(ENABLE);
            /* 等待PLL输出稳定 */
            while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);
            
            /* 选择SYSCLK时钟源为PLL */
            RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
            /* 等待PLL成为SYSCLK时钟源 */
            while(RCC_GetSYSCLKSource()!=0x08);
    }
    
    /* 开启GPIOB时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    
    /* 开启USART1和GPIOA时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA , ENABLE);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
    
    //LED IO
    RCC_APB2PeriphClockCmd(RCC_APB_GPIO_LED0, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB_GPIO_LED1, ENABLE);
    
}

/*******************************************************************************
* 函数名  	: GPIO_Configuration
* 函数描述    	: 设置各GPIO端口功能
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
*******************************************************************************/

void GPIO_Configuration(void)
{
    /* 定义 GPIO 初始化结构体 GPIO_InitStructure */
    GPIO_InitTypeDef GPIO_InitStructure; 
    
    /* 设置LED0 (PA8),LED1(PD2)口为推挽输出，最大翻转频率为50MHz*/
    GPIO_InitStructure.GPIO_Pin = LED0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIO_LED0 , &GPIO_InitStructure);
    
   /////////////////////////////////////////////////
    GPIO_InitStructure.GPIO_Pin =  LED1; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIO_LED1, &GPIO_InitStructure); 
    
    ///////////////////////////////////////////////
      	/* 将 PC.4 设置为模拟输入脚 */
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//  	GPIO_Init(GPIOC , &GPIO_InitStructure);
}



void IWDG_Init(void)
{
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    /* IWDG configuration: IWDG is clocked by LSI = 40KHz */
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    /* Timeout 2000MS  < 0xFFF */
    IWDG_SetReload(1);
    /* Reload IWDG counter */
    IWDG_ReloadCounter();
}

void Board_Reset(void)
{
    /* Enable IWDG */
    IWDG_Enable();
}

/******************************************************************************
* 函数名  		: Systick_Configuration
* 函数描述    	: 设置Systick定时器,重装载时间为1ms
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
******************************************************************************/
void Systick_Configuration(void)
{
  	/* 主频为72MHz，配置计数值72000000除以9000可以得到1ms定时间隔 */
	SysTick_Config(SystemCoreClock / 9000);
	/* 选择HCLK进行8分频后作为Systick时钟源 */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}


void Board_LowLevelInit(void)
{
#ifdef USE_HSI
    RCC_Configuration_HSI();
#else
    /* 设置系统时钟 */
    RCC_Configuration();
#endif

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    IWDG_Init();

    /* 设置GPIO端口 */
    GPIO_Configuration();
    /* 设置USART */
    USART_Config(STDIO_UART, 115200);
    USART_ConfigInt(STDIO_UART, USART_IT_RXNE, ENABLE);
    
      
    AiP_SPI_Init();

//    I2C_Config(_I2C1);
//    SPIx_Init();

    mDelay(100);
}
