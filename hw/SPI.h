/**
  ******************************************************************************
  * @file     SPI.h
  * @author   lianshuang@memsplus.com
  * @version  V1.0.0
  * @date     27-Feb-2018
  * @brief    SPI abstract layer.
  *           This file provides template for all SPI's operations.
  ******************************************************************************/

#ifndef __SPI_H__
#define __SPI_H__

#include "libcfg.h"

typedef struct {

    u32			Res;
    GPIO_TypeDef*       Port;
    u16                 Pin;
} SPILine_t;

typedef struct{
	SPI_TypeDef *		SPIn;
	u32			Res;
        SPILine_t		SCK;
        SPILine_t		MISO;
	SPILine_t		MOSI;
	SPILine_t		CS;
} SPIX_t;

extern const SPIX_t _SPIx[];

#define _SPI1           	(void*)(&_SPIx[0])
#define _SPI2           	(void*)(&_SPIx[1])
#define _SPI3           	(void*)(&_SPIx[2])
#define _SPI4           	(void*)(&_SPIx[3])
#define _SPI5           	(void*)(&_SPIx[4])

#define SPI_CS_LOW(SPIX)	GPIO_ResetBits(SPIX->CS.Port, SPIX->CS.Pin)
#define SPI_CS_HIGH(SPIX)	GPIO_SetBits(SPIX->CS.Port, SPIX->CS.Pin)

void SPIx_Init(void);
u8 SPI_WriteReadByte(SPI_TypeDef *SPIx, u8 wData);
s8 SPI_Read(void *_SPI, u8* pBuffer, u8 ReadAddr, u8 NumByteToRead);
s8 SPI_Write(void *_SPI, u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);

#endif

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
