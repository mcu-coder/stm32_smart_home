/************************************************************************************
* Description:128*64点阵的OLED显示屏驱动文件，仅适用于惠特自动化(heltec.taobao.com)的SD1306驱动IIC通信方式显示屏
*
* Others: none;
*
* Function List:
*	1. void I2C_Configuration(void) -- 配置CPU的硬件I2C
* 2. void I2C_WriteByte(uint8_t addr,uint8_t data) -- 向寄存器地址写一个byte的数据
* 3. void OLED_WriteCmd(unsigned char I2C_Command) -- 写命令
* 4. void OLED_WriteDat(unsigned char I2C_Data) -- 写数据
* 5. void OLED_Init(void) -- OLED屏初始化
* 6. void OLED_SetPos(unsigned char x, unsigned char y) -- 设置起始点坐标
* 7. void OLED_Fill(unsigned char fill_Data) -- 全屏填充
* 8. void OLED_CLS(void) -- 清屏
* 9. void OLED_ON(void) -- 唤醒
* 10. void OLED_OFF(void) -- 睡眠
* 11. void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize) -- 显示字符串(字体大小有6*8和8*16两种)
* 12. void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N) -- 显示中文(中文需要先取模，然后放到codetab.h中)
* 13. void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]) -- BMP图片
*
* History: none;
*
*************************************************************************************/


#include "stm32f10x.h"

#include "delay.h"
#include "OLED/codetab.h"
#include "OLED/oled.h"
typedef struct OLED_Type
{
	uint16_t u16_size_x;
	uint16_t u16_size_y;
}OLED_Type_t;

OLED_Type_t OLED_Type_Handle = 
{
	.u16_size_x = 128,
	.u16_size_y = 64,
};

/**
**  初始化IIC接口
**/
void OLED_IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//开启GPIOD的时钟
	RCC_APB2PeriphClockCmd(OLED_RCC, ENABLE);
	//设置GPIO的基本参数
	GPIO_InitStruct.GPIO_Pin = OLED_SDA_GPIO_PIN ;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;	 //推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	  //输出速度50MHz

	GPIO_Init(OLED_SDA_GPIO_PORT, &GPIO_InitStruct);
	GPIO_SetBits(OLED_SDA_GPIO_PORT, OLED_SDA_GPIO_PIN);

	//开启GPIOD的时钟
	RCC_APB2PeriphClockCmd(OLED_RCC, ENABLE);
	//设置GPIO的基本参数
	GPIO_InitStruct.GPIO_Pin = OLED_SCL_GPIO_PIN ;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;	 //推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	  //输出速度50MHz
	
	GPIO_Init(OLED_SCL_GPIO_PORT, &GPIO_InitStruct);
	GPIO_SetBits(OLED_SCL_GPIO_PORT, OLED_SCL_GPIO_PIN);

}

/**
**  设置SDA为输出
**/
void OLED_SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= OLED_SDA_GPIO_PIN;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(OLED_SDA_GPIO_PORT, &GPIO_InitStructer);
}


/**
**  设置SDA为输入
**/
void OLED_SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= OLED_SDA_GPIO_PIN;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_Init(OLED_SDA_GPIO_PORT, &GPIO_InitStructer);
}

//开始信号
void OLED_IIC_Start(void)
{
    OLED_SDA_OUT();
    OLED_IIC_SDA=1;
    OLED_IIC_SCL=1;
    delay_us(2);
    OLED_IIC_SDA=0;
    delay_us(2);
    OLED_IIC_SCL=0;
    delay_us(2);
}

void OLED_IIC_Stop(void)
{
    OLED_IIC_SCL=1;
    OLED_IIC_SDA=0;
    delay_us(2);
    OLED_IIC_SDA=1;
    delay_us(2);
}


/*
*   返回1--应答出错
*   放回0--应答正确
*/
u8 OLED_IIC_Wait_Ask(void)
{
    int count=0;
    OLED_SDA_IN();
    OLED_IIC_SCL=1;
    delay_us(2);
    while(OLED_READ_SDA)
    {
        count++;
        if(count>250)
        {
            OLED_IIC_Stop();
            return 1;
        }   
    }
    OLED_IIC_SCL=0;
    delay_us(2);
    return 0;
}

//写一个字节
void OLED_IIC_WriteByte(u8 data)
{
    u8 i;
    OLED_SDA_OUT();
    for(i=0;i<8;i++)
    {
        OLED_IIC_SCL=0;
        delay_us(2);
        if(data & 0x80)     //MSB,从高位开始一位一位传输
            OLED_IIC_SDA=1;
        else
            OLED_IIC_SDA=0;
        OLED_IIC_SCL=1;
        delay_us(2);
        OLED_IIC_SCL=0;
        data<<=1;

    }
}


u8 OLED_IIC_ReadByte(void)
{
    u8 data,i;
    OLED_IIC_SDA=1;
    delay_us(2);
    for(i=0;i<8;i++)
    {
        data<<=1;
        OLED_IIC_SCL=0;
        delay_us(2);
        OLED_IIC_SCL=1;
        delay_us(2);
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))
            data=data | 0x01;
        else 
            data=data & 0xFE;

    }
    OLED_IIC_SCL=0;
    delay_us(2);
    return data;

}


void OLED_WriteCmd(u8 command)
{
    OLED_IIC_Start();
    OLED_IIC_WriteByte(0x78);//OLED地址
    OLED_IIC_Wait_Ask();
    OLED_IIC_WriteByte(0x00);//寄存器地址
    OLED_IIC_Wait_Ask();
    OLED_IIC_WriteByte(command);
    OLED_IIC_Wait_Ask();
    OLED_IIC_Stop();
}


void OLED_WriteDat(u8 data)
{
    OLED_IIC_Start();
    OLED_IIC_WriteByte(0x78);//OLED地址
    OLED_IIC_Wait_Ask();
    OLED_IIC_WriteByte(0x40);//寄存器地址
    OLED_IIC_Wait_Ask();
    OLED_IIC_WriteByte(data);
    OLED_IIC_Wait_Ask();
    OLED_IIC_Stop();
}

void OLED_Init(void)
{
	OLED_IIC_Init();
	
    delay_ms(100); //这里的延时很重要

    OLED_WriteCmd(0xAE); //display off
    OLED_WriteCmd(0x20); //Set Memory Addressing Mode    
    OLED_WriteCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    OLED_WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    OLED_WriteCmd(0xc8); //Set COM Output Scan Direction
    OLED_WriteCmd(0x00); //---set low column address
    OLED_WriteCmd(0x10); //---set high column address
    OLED_WriteCmd(0x40); //--set start line address
    OLED_WriteCmd(0x81); //--set contrast control register
    OLED_WriteCmd(0xff); //亮度调节 0x00~0xff
    OLED_WriteCmd(0xa1); //--set segment re-map 0 to 127
    OLED_WriteCmd(0xa6); //--set normal display
    OLED_WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
    OLED_WriteCmd(0x3F); //
    OLED_WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    OLED_WriteCmd(0xd3); //-set display offset
    OLED_WriteCmd(0x00); //-not offset
    OLED_WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    OLED_WriteCmd(0xf0); //--set divide ratio
    OLED_WriteCmd(0xd9); //--set pre-charge period
    OLED_WriteCmd(0x22); //
    OLED_WriteCmd(0xda); //--set com pins hardware configuration
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xdb); //--set vcomh
    OLED_WriteCmd(0x20); //0x20,0.77xVcc
    OLED_WriteCmd(0x8d); //--set DC-DC enable
    OLED_WriteCmd(0x14); //
    OLED_WriteCmd(0xaf); //--turn on oled panel
}


 /**
  * @brief  OLED_ON，将OLED从休眠中唤醒
  * @param  无
    * @retval 无
  */
void OLED_ON(void)
{
    OLED_WriteCmd(0X8D);  //设置电荷泵
    OLED_WriteCmd(0X14);  //开启电荷泵
    OLED_WriteCmd(0XAF);  //OLED唤醒
}


 /**
  * @brief  OLED_SetPos，设置光标
  * @param  x,光标x位置
    *                   y，光标y位置
  * @retval 无
  */
void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
    OLED_WriteCmd(0xb0+y);
    OLED_WriteCmd(((x&0xf0)>>4)|0x10);
    OLED_WriteCmd((x&0x0f)|0x01);
}

 /**
  * @brief  OLED_Fill，填充整个屏幕
  * @param  fill_Data:要填充的数据
    * @retval 无
  */
void OLED_Fill(unsigned char fill_Data)//全屏填充
{
    unsigned char m,n;
    for(m=0;m<8;m++)
    {
        OLED_WriteCmd(0xb0+m);       //page0-page1
        OLED_WriteCmd(0x00);     //low column start address
        OLED_WriteCmd(0x10);     //high column start address
        for(n=0;n<128;n++)
        {
            OLED_WriteDat(fill_Data);
        }
    }
}

void OLED_CLS(void)//清屏
{
    OLED_Fill(0x00);
}

/**
  * @brief  OLED_ShowStr，显示codetab.h中的ASCII字符,有6*8和8*16可选择
  * @param  x,y : 起始点坐标(x:0~127, y:0~7);
    *                   ch[] :- 要显示的字符串; 
    *                   TextSize : 字符大小(1:6*8 ; 2:8*16)
    * @retval 无
  */
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					OLED_WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					OLED_WriteDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					OLED_WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}
	



/**
  * @brief  OLED_ShowCnAndAsciiStr，显示codetab.h中的ASCII字符和中文
  * @param  x,y : 起始点坐标(x:0~127, y:0~7);
    *                  str :- 要显示的字符串; 
    *             		TextSize : 字符大小(1:6*8 ; 2:8*16)
    * @retval 无
  */
void OLED_ShowCnAndAsciiStr(u8 x, u8 y, const char *ch, u8 TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	u8 index = 0;
	switch(TextSize)
	{
		case 1:
			while (ch[j] != '\0')
			{
				c = ch[j] - 32;
				if (x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for (i=0; i<6; i++)
				{
					OLED_WriteDat(F6x8[c][i]);
				}
				x += 6;
				j++;
			}
			break;
		case 2:
		{
			while (ch[j] != '\0')
			{
				if (ch[j] < 127)
				{
					// ascii
					c = ch[j] - 32;
					if (x > 120)
					{
						x = 0;
						y++;
					}
					OLED_SetPos(x,y);
					for (i=0; i<8; i++)
					{
						OLED_WriteDat(F8X16[c*16+i]);
					}
					OLED_SetPos(x,y+1);
					for(i=0;i<8;i++)
					{
						OLED_WriteDat(F8X16[c*16+i+8]);
					}
					x += 8;
					j++;
				}
				else 
				{
					// cn
					for (i=0; i<CnFont16x16_Num; i++)
					{
						if (CnFont_16x16[i].p_CnFontCode[0] == ch[j]
						&&  CnFont_16x16[i].p_CnFontCode[1] == ch[j+1])
						{
							if (x > 112)
							{
								x = 0;
								y++;
							}
							OLED_SetPos(x , y);
							for (index=0; index<16; index++)
							{
								OLED_WriteDat(CnFont_16x16[i].F16x16[index]);
							}
							OLED_SetPos(x, y+1);
							for(; index< 32; index++)
							{
								OLED_WriteDat(CnFont_16x16[i].F16x16[index]);
							}
							break;
						}
					}
					x += 16;
					j += 2;
				}
			}
		}break;
	}
}





