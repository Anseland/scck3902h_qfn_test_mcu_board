#ifndef __SPI_GPIO_H__
#define __SPI_GPIO_H__


//本代码基于IO模拟SPI，移植时仅与IO配置相关


#include "stm32f10x_conf.h"
#include "stm32f10x.h"



#define SCLK_H  GPIO_SetBits(GPIOA,GPIO_Pin_3)
#define SCLK_L  GPIO_ResetBits(GPIOA,GPIO_Pin_3)
#define SDI_H   GPIO_SetBits(GPIOA,GPIO_Pin_2)
#define SDI_L   GPIO_ResetBits(GPIOA,GPIO_Pin_2)
#define CS_H    GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define CS_L    GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define SDO     GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)


//void RCC_Configuration(void); 
//void GPIO_Configuration(void);
void Delay(vu32 nCount);

void AiP_SPI_Init(void);                //初始化SPI端口
void AIP_SPI_WB(uint8_t com);           //SPI接口（字节）写入函数
uint8_t AIP_SPI_RB(void);               //SPI接口（字节）读取函数

uint32_t AIP_RDATA(void);               //数据读取函数（读取3Byte）
void AIP_RESET(void);                   //复位函数
uint16_t Get_ID(void);                  //读取芯片ID，ID的值为：0X00DX;
uint16_t AIP_Read_Reg(uint8_t addr);
//complete reading proccess
u8 AIP_Read_Reg8(u8 addr); //1 bytes

#endif 

