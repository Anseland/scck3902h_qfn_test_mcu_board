/********************************************************************************
  * @file     board.h
  * @author   Jackie
  * @version  V1.0.0
  * @date     16-Jul-2014
  * @brief    board level initilization.
  *
  ******************************************************************************/

#ifndef __BOARD_H__
#define __BOARD_H__

#include "libcfg.h"

void Board_LowLevelInit(void);
void Board_Reset(void);

//LED IO definition 
#define LED0                    GPIO_Pin_14             //PA8
#define GPIO_LED0               GPIOB
#define RCC_APB_GPIO_LED0       RCC_APB2Periph_GPIOB

#define LED1                    GPIO_Pin_15             //PD2
#define GPIO_LED1               GPIOB
#define RCC_APB_GPIO_LED1       RCC_APB2Periph_GPIOB

#define LED0_ON         GPIO_ResetBits(GPIO_LED0, LED0)
#define LED1_ON         GPIO_ResetBits(GPIO_LED1, LED1)
#define LED0_OFF        GPIO_SetBits(GPIO_LED0, LED0)
#define LED1_OFF        GPIO_SetBits(GPIO_LED1, LED1)
//End LED Definition


#endif /* __BOARD_H__ */
