//By LiaoAnmou, for AiP controller, SPI interface


//include

#include "SPI_GPIO.h"



void AiP_SPI_Init(void)        //初始化SPI端口
{
    /* 定义 GPIO 初始化结构体 GPIO_InitStructure */
    GPIO_InitTypeDef GPIO_InitStructure; 

    //GPIO setting for AiP SPI,GPIOA, Pin.0,1,2,3 (SDO, CS, SDI,CLK)
    //CS:PA1, SDI:PA2, CLK:PA3， 输出
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

    //SDO: PA0, 输入
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    ////////////////////////////////////////////////////
    
    
    /* 定义PA.0为外部中断0输入通道（EXIT0） */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA , GPIO_PinSource0);
    
    /////////
    /* 定义NVIC初始化结构体 NVIC_InitStructure */
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 定义SDO（PA.0）为外部中断0输入通道（EXIT0） */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA , GPIO_PinSource0);
    
    /* 选择NVIC优先级分组2 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* 使能EXIT 0通道 ，2级先占优先级 ，0级次占优先级 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
//    //使能中断
//    EXTI_InitTypeDef EXTI_InitStructure;
//    /* 设置外部中断0通道（EXIT Line0）在下降沿时触发中断 */  
//    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXTI_InitStructure);
    
        
}


//write a byte to the spi device, write MB fisrt
void AIP_SPI_WB(uint8_t com)
{
  uint8_t com_temp=com;
  for(uint8_t s=0;s<8;s++)
  {
    SCLK_L;
    if(com_temp&0x80)
    {
      SDI_H;
    }
    else
    {
      SDI_L;
    }
    Delay(10);
    com_temp<<=1;
  
    SCLK_H;
    Delay(10);
  }
}

//write a byte from the spi device, get MB fisrt
uint8_t AIP_SPI_RB(void)
{
  uint8_t Rdata=0;
  for(uint8_t s=0;s<8;s++)
  {
    SCLK_L;
    Delay(10);  //wait data
    
    SCLK_H;
    Rdata<<=1;
    if(SDO)Rdata++;  //read data
    
    Delay(10);
    
  }
  return Rdata;
}

////////////////////////////////////////////////////////////

//u8* AIP_RDATA(u8 n)// read n bytes
//{
//  if(n<=0)
//  {
//    return -1;
//  }
//  
//  u32 Rdata=0;
//  for(u8 s=0;s<24;s++)
//  {
//    SCLK_L;
//    Rdata<<=1;
//    if(SDO)Rdata++;
//    SCLK_H;
//  }
//  return Rdata;
//}


uint16_t Get_ID(void)
{
  uint16_t ID;
  ID=0;
  CS_L;
  AIP_SPI_WB(0x47);
  ID=AIP_SPI_RB();
  ID<<=8;
  ID|=AIP_SPI_RB();
  CS_H;
  return ID;
}

//complete reading proccess
u8 AIP_Read_Reg8(u8 addr) //1 bytes
{
  u8 reg;
  CS_L;
  AIP_SPI_WB(0x47);
  reg=AIP_SPI_RB();

  CS_H;
  return reg;
}

//complete reading proccess
uint16_t AIP_Read_Reg(uint8_t addr) //2 bytes
{
  uint16_t reg;
  CS_L;
  AIP_SPI_WB(addr);
  reg=AIP_SPI_RB();
  reg<<=8;
  reg|=AIP_SPI_RB();
  CS_H;
  return reg;
}

//complete writing proccess
void AIP_Write_Reg(uint8_t addr, uint8_t data) //1 bytes
{

  CS_L;
  AIP_SPI_WB(addr);
  AIP_SPI_WB(data);
  CS_H;

}


void AIP_RESET(void)
{
  SCLK_H;
  Delay(0xffff);
  CS_L;
  for(uint8_t a=0;a<10;a++)
  {
    AIP_SPI_WB(0xff);
  }
  CS_H;
  Delay(0xffff);//等待复位完成 
  CS_L;
}

void Delay(vu32 nCount) 
{ 
  for(; nCount != 0; nCount--); 
} 

#ifdef  DEBUG 
void assert_failed(u8* file, u32 line) 
{  
  while (1) { } 
} 
#endif