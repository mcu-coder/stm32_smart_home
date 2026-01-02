#ifndef __OLED_H
#define __OLED_H	

#include "sys.h"
#include "stdlib.h"	    	

#define OLED_RCC			RCC_APB2Periph_GPIOB
#define OLED_SDA_GPIO_PORT	GPIOB
#define OLED_SDA_GPIO_PIN	GPIO_Pin_15
#define OLED_SCL_GPIO_PORT	GPIOB
#define OLED_SCL_GPIO_PIN	GPIO_Pin_14

#define OLED_IIC_SDA		PBout(15)
#define OLED_IIC_SCL		PBout(14)
#define OLED_READ_SDA	PBin(15)


void OLED_IIC_Init(void);
void OLED_SDA_OUT(void);
void OLED_SDA_IN(void);
void OLED_IIC_Start(void);
void OLED_IIC_Stop(void);
u8 OLED_IIC_Wait_Ask(void);
void OLED_IIC_WriteByte(u8 data);
u8 OLED_IIC_ReadByte(void);
void OLED_WriteCmd(u8 command);
void OLED_WriteDat(u8 data);
void OLED_Init(void);
void OLED_ON(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCnAndAsciiStr(u8 x, u8 y, const char *ch, u8 TextSize);


#endif  
	 
