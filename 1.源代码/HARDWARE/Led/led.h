#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define BEEP_PORT	GPIOB
#define BEEP_PIN	GPIO_Pin_3

#define BEEP_ON		(PBout(3) = 1)	// 位带
#define BEEP_OFF	(PBout(3) = 0)	// 位带
#define BEEP_FLASH	GPIO_TogglePin(BEEP_PORT, BEEP_PIN)




#define WARN_LED_PORT	GPIOB
#define WARN_LED_PIN	GPIO_Pin_4

#define WARN_LED_ON		(PBout(4) = 0)	// 位带
#define WARN_LED_OFF	(PBout(4) = 1)	// 位带
#define WARN_LED_FLASH	GPIO_TogglePin(WARN_LED_PORT, WARN_LED_PIN)

#define LED_TIP_PORT	GPIOC
#define LED_TIP_PIN		GPIO_Pin_13

#define LED_TIP_ON		(PCout(13) = 0)	// 位带
#define LED_TIP_OFF		(PCout(13) = 1)	// 位带
#define LED_TIP_FLASH	GPIO_TogglePin(LED_TIP_PORT, LED_TIP_PIN)

#define FAN_PORT		GPIOB
#define FAN_PIN			GPIO_Pin_14

#define FAN_ON			(PBout(14) = 0)	// 位带
#define FAN_OFF			(PBout(14) = 1)	// 位带
#define FAN_FLASH		GPIO_TogglePin(FAN_PORT, FAN_PIN)
#define FAN_STAT		(PBin(14) == 0)

#define LIGHT_PORT		GPIOB
#define LIGHT_PIN		GPIO_Pin_7

#define LIGHT_ON		(PBout(7) = 0)	// 位带
#define LIGHT_OFF		(PBout(7) = 1)	// 位带
#define LIGHT_FLASH		GPIO_TogglePin(LIGHT_PORT, LIGHT_PIN)
#define LIGHT_STAT		(PBin(13) == 0)
extern void LED_Init( void );
extern void BEEP_Init(void);

#endif	/* __LED_H */

