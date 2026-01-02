#include "led.h"
#include "delay.h"	   

/**
 * name: LED_Init
 * decription: led 初始化
 */
void LED_Init( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure;
#if 1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能PA端口时钟

	GPIO_InitStructure.GPIO_Pin = LED_TIP_PIN;				//LED0-->PA 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		//IO口速度为50MHz
	GPIO_Init(LED_TIP_PORT, &GPIO_InitStructure);			//根据设定参数初始化GPIOA.4
#if 1
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能PA端口时钟
		GPIO_InitStructure.GPIO_Pin = WARN_LED_PIN;				//LED0-->PA 端口配置
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//开漏输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		//IO口速度为50MHz
		GPIO_Init(WARN_LED_PORT, &GPIO_InitStructure);			//根据设定参数初始化GPIOA.4
#endif
#endif
	LED_TIP_OFF;
	WARN_LED_OFF;
	
}


void BEEP_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能PA端口时钟

	GPIO_InitStructure.GPIO_Pin = BEEP_PIN;				//LED0-->PA 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		//IO口速度为50MHz
	GPIO_Init(BEEP_PORT, &GPIO_InitStructure);			//根据设定参数初始化GPIOA.4
	
	BEEP_OFF;
}




/**
  * @brief  Toggles the specified GPIO pin
  * @param  GPIOx: where x can be (A..G depending on device used) to select the GPIO peripheral 
  * @param  GPIO_Pin: Specifies the pins to be toggled.
  * @retval None
  */
void GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  GPIOx->ODR ^= GPIO_Pin;
}

