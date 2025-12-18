/**
  ******************************************************************************
  * @file     Delay.c
  * @author   lianshuang@memsplus.com
  * @version  V1.0.0
  * @date     27-Feb-2014
  * @brief    delay function via systick
  ******************************************************************************/


#include "Delay.h"


#define US_TO_TICK(U)       (U*9UL)
#define MS_TO_TICK(M)       (M*9000UL)

/* Microsecond delay func */
void uDelay(u16 nus)
{
#if 1
	u32 temp;

	SysTick->LOAD = (u32)US_TO_TICK(nus);
	SysTick->VAL=0x00;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	do{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL =0x00;
#else
    nus *= 32;
    while(nus--);
#endif
}

/* Millisecond delay func */
void mDelay(u32 nms)
{
#if 1
	u32 temp;

	SysTick->LOAD = MS_TO_TICK(nms);                                            /* only 24bits valid */
	SysTick->VAL=0x00;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	do{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL =0x00;
#else
    nms *= 32000;
    while(nms--);
#endif
}
































