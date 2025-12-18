/**
  ******************************************************************************
  * @file     SPI.c
  * @author   lianshuang@memsplus.com
  * @version  V1.0.0
  * @date     27-Feb-2018
  * @brief    SPI abstract layer.
  *           This file provides template for all SPI's operations.
  ******************************************************************************/

#include "delay.h"
#include "SPI.h"
#include "stm32f10x_spi.h"

#define SPI_FLAG_TIMEOUT        (0x1000)

#define _SPI1_SCK		{ RCC_APB2Periph_GPIOA,	GPIOA,	GPIO_Pin_5 }
#define _SPI1_MISO		{ RCC_APB2Periph_GPIOA,	GPIOA,	GPIO_Pin_6 }
#define _SPI1_MOSI		{ RCC_APB2Periph_GPIOA, GPIOA,	GPIO_Pin_7 }
#define _SPI1_CS		{ RCC_APB2Periph_GPIOA, GPIOA,	GPIO_Pin_4 }

#define _SPI2_SCK		{ RCC_APB2Periph_GPIOB,	GPIOB,	GPIO_Pin_13 }
#define _SPI2_MISO		{ RCC_APB2Periph_GPIOB,	GPIOB,	GPIO_Pin_14 }
#define _SPI2_MOSI		{ RCC_APB2Periph_GPIOB, GPIOB,	GPIO_Pin_15 }
#define _SPI2_CS		{ RCC_APB2Periph_GPIOB, GPIOB,	GPIO_Pin_12 }

const SPIX_t _SPIx[] = 	{	{ SPI1,	RCC_APB2Periph_SPI1,	_SPI1_SCK,		_SPI1_MISO,		_SPI1_MOSI,		_SPI1_CS	},
				{ SPI2,	RCC_APB1Periph_SPI2,	_SPI2_SCK,		_SPI2_MISO,		_SPI2_MOSI,		_SPI2_CS	},
					 	};

#define SPI_DUMMY_BYTE          0x00

void SPIx_Init(void)
{
    SPI_InitTypeDef     SPI_InitStructure;
    GPIO_InitTypeDef    GPIO_InitStructure;
    u8 			i = 0;

    for(i = 0; i < sizeof(_SPIx)/sizeof(_SPIx[0]); i++)
    {
      RCC_APB2PeriphClockCmd(_SPIx[i].SCK.Res|_SPIx[i].MISO.Res|_SPIx[i].MOSI.Res|_SPIx[i].CS.Res|RCC_APB2Periph_AFIO, ENABLE );
      /* NOTE: SPI1 are different from SPI2 */
      if(_SPIx[i].SPIn==SPI1)
        RCC_APB2PeriphClockCmd(_SPIx[i].Res, ENABLE);
      else
        RCC_APB1PeriphClockCmd(_SPIx[i].Res, ENABLE);

        GPIO_InitStructure.GPIO_Pin=_SPIx[i].SCK.Pin;
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
        GPIO_Init(_SPIx[i].SCK.Port,&GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin=_SPIx[i].MISO.Pin;
        GPIO_Init(_SPIx[i].MISO.Port,&GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin=_SPIx[i].MOSI.Pin;
        GPIO_Init(_SPIx[i].MOSI.Port,&GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin=_SPIx[i].CS.Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
        GPIO_Init(_SPIx[i].CS.Port,&GPIO_InitStructure);
        GPIO_SetBits(_SPIx[i].CS.Port, _SPIx[i].CS.Pin);

        SPI_I2S_DeInit(_SPIx[i].SPIn);
        SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
        SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
        SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;

        SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;
        SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;
        SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;

        SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_64;
        SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
        SPI_InitStructure.SPI_CRCPolynomial=7;

        SPI_Init(_SPIx[i].SPIn,&SPI_InitStructure);
        SPI_Cmd(_SPIx[i].SPIn,ENABLE);

        SPI_WriteReadByte(_SPIx[i].SPIn, 0x00);
    }
}

s8 SPI_TIMEOUT_UserCallback(void)
{
    SPIx_Init();

    return -1;
}

u8 SPI_WriteReadByte(SPI_TypeDef *SPIx, u8 wData)
{
    u32     SPITimeout;

    SPITimeout = SPI_FLAG_TIMEOUT;
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET){
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback();
    }
    /*!< Send the byte */
    SPI_I2S_SendData(SPIx, wData);

    SPITimeout = SPI_FLAG_TIMEOUT;
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET){
        if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback();
    }
    /*!< Get the received data */
    return (u8)(0xFF & SPI_I2S_ReceiveData(SPIx));
}

s8 SPI_Read(void *SPIx, u8* pBuffer, u8 ReadAddr, u8 NumByteToRead)
{
	SPIX_t *SPIX = (SPIX_t *)SPIx;

	SPI_CS_LOW(SPIX);

    /*!< Send the address */
    SPI_WriteReadByte(SPIX->SPIn, ReadAddr);

    while( NumByteToRead-- > 0 ){
        /*!< Get the received data */
        *pBuffer++ = SPI_WriteReadByte(SPIX->SPIn, SPI_DUMMY_BYTE);;
    }

    SPI_CS_HIGH(SPIX);

    return 0;
}

/*******************************************************************************/
s8 SPI_Write(void *SPIx, u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
    SPIX_t *SPIX = (SPIX_t *)SPIx;

    SPI_CS_LOW(SPIX);

    /*!< Send the address */
    SPI_WriteReadByte(SPIX->SPIn, WriteAddr);

    while( NumByteToWrite-- > 0 ){
        /*!< Send the byte */
        SPI_WriteReadByte(SPIX->SPIn, *pBuffer++);
    }

    SPI_CS_HIGH(SPIX);

    return 0;
}