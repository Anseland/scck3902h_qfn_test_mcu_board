/**
  ******************************************************************************
  * @file  i2c_hw->c
  * @brief STM32F10x MCU hardware IIC driver->
  *
  * <h2><center>&copy; COPYRIGHT 2015 memsplus</center></h2>
  */
/*******************************************************************************/
/*******************************************************************************/
/* Includes -------------------------------------------*/

#include "I2C.h"
#include "delay.h"
#include <stdio.h>

#define I2C_LONG_TIMEOUT        ((u32)100000)

#define _I2C1_SCK		        { RCC_APB2Periph_GPIOB,	GPIOB,	GPIO_Pin_6 }
#define _I2C1_SDA		        { RCC_APB2Periph_GPIOB,	GPIOB,	GPIO_Pin_7 }

#define _I2C2_SCK		        { RCC_APB2Periph_GPIOB,	GPIOB,	GPIO_Pin_10 }
#define _I2C2_SDA		        { RCC_APB2Periph_GPIOB,	GPIOB,	GPIO_Pin_11 }


const I2CX_t _I2Cx[] = 	    {   { I2C1, RCC_APB1Periph_I2C1,    _I2C1_SCK,  _I2C1_SDA },
							    { I2C2,	RCC_APB1Periph_I2C2,    _I2C2_SCK,  _I2C2_SDA },
					 	    };

#define I2C1_CFG                { 400000,                           \
                                  I2C_Mode_I2C,                     \
                                  I2C_DutyCycle_2,                  \
                                  0x01,                             \
                                  I2C_Ack_Enable,                   \
                                  I2C_AcknowledgedAddress_7bit      \
                                }

#define I2C2_CFG                { 400000,                           \
                                  I2C_Mode_I2C,                     \
                                  I2C_DutyCycle_2,                  \
                                  0x02,                             \
                                  I2C_Ack_Enable,                   \
                                  I2C_AcknowledgedAddress_7bit      \
                                }

void I2C_Config(void *dat)
{
    I2C_InitTypeDef     I2C_InitStructure = I2C1_CFG;
    GPIO_InitTypeDef    GPIO_InitStructure = {0};
    I2CX_t              *I2Cx = (I2CX_t*)dat;

    RCC_APB1PeriphClockCmd(I2Cx->Res, ENABLE);
    RCC_APB2PeriphClockCmd(I2Cx->SCL.Res, ENABLE);
    RCC_APB2PeriphClockCmd(I2Cx->SDA.Res, ENABLE);

    /* Reset I2C peripheral */
    RCC_APB1PeriphResetCmd(I2Cx->Res, ENABLE);
    /* Release reset signal of I2C IP */
    RCC_APB1PeriphResetCmd(I2Cx->Res, DISABLE);

    /*!< Configure I2C pins: SCL */
    GPIO_InitStructure.GPIO_Pin = I2Cx->SCL.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2Cx->SCL.Port, &GPIO_InitStructure);

    /*!< Configure I2C pins: SDA */
    GPIO_InitStructure.GPIO_Pin = I2Cx->SDA.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(I2Cx->SDA.Port, &GPIO_InitStructure);

    I2C_Cmd(I2Cx->I2Cn, ENABLE);
    I2C_Init(I2Cx->I2Cn, &I2C_InitStructure);
    mDelay(100);
}

/*******************************************************************************
* Function Name  : I2C_read
* Description    : read data from miramems sample
* Input          : slave I2C address,register address, buff of read data
* Output         : None->
* Return         : None->
*******************************************************************************/
static inline s8 I2C_TIMEOUT_UserCallback(void *I2Cx)
{
    printf("I2C timeout !\r\n");

    I2C_Config(I2Cx);

    return -1;
}

/* IIC read routine */
s8 I2C_Read(void *I2C, u8 DevAddr, u8 ReadAddr, u8* pBuffer, u8 NumByteToRead)
{
    u32     I2CTimeout = I2C_LONG_TIMEOUT;
    I2CX_t  *I2Cx = (I2CX_t *)I2C;

    /* While the bus is busy */
    I2CTimeout = I2C_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2Cx->I2Cn,I2C_FLAG_BUSY)){
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
    }

    I2C_GenerateSTART(I2Cx->I2Cn, ENABLE);

    I2CTimeout = I2C_LONG_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx->I2Cn, I2C_EVENT_MASTER_MODE_SELECT)){
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
    }

    /* Send device address for write */
    I2C_Send7bitAddress(I2Cx->I2Cn, (DevAddr<<1), I2C_Direction_Transmitter);

    I2CTimeout = I2C_LONG_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx->I2Cn, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
    }

    I2C_SendData(I2Cx->I2Cn, ReadAddr);
    I2CTimeout = I2C_LONG_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx->I2Cn, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
    }

    /* Send START condition a second time */
    I2C_GenerateSTART(I2Cx->I2Cn, ENABLE);
    I2CTimeout = I2C_LONG_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx->I2Cn, I2C_EVENT_MASTER_MODE_SELECT)){
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
    }

    /* Send device address for read */
    I2C_Send7bitAddress(I2Cx->I2Cn, (DevAddr<<1), I2C_Direction_Receiver);
    I2CTimeout = I2C_LONG_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx->I2Cn, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
    }

    while (NumByteToRead > 1)
    {
        /* Poll on BTF */
        I2CTimeout = I2C_LONG_TIMEOUT;
        while(!I2C_CheckEvent(I2Cx->I2Cn, I2C_EVENT_MASTER_BYTE_RECEIVED)){
            if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
        }

        *pBuffer++ = I2C_ReceiveData(I2Cx->I2Cn);
        /* Decrement the read bytes counter */
        NumByteToRead--;
    }

    /* Disable Acknowledgement */
    I2C_AcknowledgeConfig(I2Cx->I2Cn, DISABLE);

        /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx->I2Cn, ENABLE);

    I2CTimeout = I2C_LONG_TIMEOUT;
    while(!I2C_GetFlagStatus(I2Cx->I2Cn, I2C_FLAG_RXNE)){
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
    }

    *pBuffer = I2C_ReceiveData(I2Cx->I2Cn);

//    /* Wait to make sure that STOP control bit has been cleared */
//    I2CTimeout = I2C_LONG_TIMEOUT;
//    while(I2Cx->CR2 & I2C_CR2_STOP){
//        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx->I2Cn);
//    }

    /* Re-Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2Cx->I2Cn, ENABLE);
    return 0;
}

/* IIC write routine */
s8 I2C_Write(void *I2C, u8 DevAddr, u8 ReadAddr, u8* pBuffer, u8 NumByteToWrite)
{
    u32     I2CTimeout = I2C_LONG_TIMEOUT;
    I2CX_t  *I2Cx = (I2CX_t *)I2C;

    /* While the bus is busy */
    I2CTimeout = I2C_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(I2Cx->I2Cn,I2C_FLAG_BUSY)){
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
    }

    /* Send START condition */
    I2C_GenerateSTART(I2Cx->I2Cn, ENABLE);

    I2CTimeout = I2C_LONG_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx->I2Cn, I2C_EVENT_MASTER_MODE_SELECT)){
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
    }

    I2C_Send7bitAddress(I2Cx->I2Cn, (DevAddr<<1), I2C_Direction_Transmitter);
    I2CTimeout = I2C_LONG_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx->I2Cn, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
    }

    I2C_SendData(I2Cx->I2Cn, ReadAddr);
    I2CTimeout = I2C_LONG_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx->I2Cn, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
    }

    while(NumByteToWrite-- > 1){
        /* Send the byte to be written */
        I2C_SendData(I2Cx->I2Cn, *pBuffer++);
        I2CTimeout = I2C_LONG_TIMEOUT;
        while(!I2C_CheckEvent(I2Cx->I2Cn, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
            if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
        }
    }

    /* Send the byte to be written */
    I2C_SendData(I2Cx->I2Cn, *pBuffer);
    I2CTimeout = I2C_LONG_TIMEOUT;
    while(!I2C_GetFlagStatus(I2Cx->I2Cn, I2C_FLAG_TXE)){
        if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(I2Cx);
    }

    I2C_GenerateSTOP(I2Cx->I2Cn,ENABLE);

    return 0;
}
/**********************************************************************************/