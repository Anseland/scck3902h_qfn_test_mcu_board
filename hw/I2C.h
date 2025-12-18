/******************** (C) COPYRIGHT 2014 CLS ********************
* File Name          : I2C_HW.c
* Author             : lianshuang@memsplus.com
* Version            : V1.0
* Date               : 01/10/2014
* Description        : HW I2C routine
*******************************************************************************/

#ifndef __I2C_H__
#define __I2C_H__

#include "libcfg.h"
#include "stm32f10x_i2c.h"

typedef struct {

	u32					Res;
	GPIO_TypeDef*       Port;
    u16                 Pin;
} I2CLine_t;

typedef struct{
	I2C_TypeDef*		I2Cn;
	u32					Res;
    I2CLine_t			SCL;
    I2CLine_t			SDA;
} I2CX_t;

extern const I2CX_t _I2Cx[];

#define _I2C1           	(void*)(&_I2Cx[0])
#define _I2C2           	(void*)(&_I2Cx[1])

void I2C_Config(void *data);
s8 I2C_Read(void *data, u8 I2C_Addr, u8 Data_Addr, u8 *pData, u8 Len);
s8 I2C_Write(void *data, u8 I2C_Addr, u8 Data_Addr, u8 *pData, u8 Len);
#endif
