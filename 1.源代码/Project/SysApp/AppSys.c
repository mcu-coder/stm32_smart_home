/*********************************************************************************
  *Copyright(C),2020-2021, SIRI
  *FileName:  AppSys.c
  *Author:  SIRI
  *Version: v1.0.1.202211215
  *Date:  20221/12/15
  *Description:  
  *Others:  
  *Function List:  
  ||主要函数列表，每条记录应包含函数名及功能简要说明||
     1.…………
     2.…………
  *History:  
  |修改历史记录列表，每条修改记录应包含修改日期、修改者及修改内容简介||
     1.Date:
       Author:
       Modification:
     2.…………
**********************************************************************************/
#include "stm32f10x.h"

#include "SysApp/AppSys.h"
#include "SysApp/AppParam.h"


#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#ifndef NULL
#define NULL  0
#endif

void Key2_short_press_up_handle( void );
void Key2_long_press_down_handle( void );
void TurnONLight( void );
void TurnOffLight( void );
void TurnOnAirCondit( void );
void TurnOffAirCondit( void );
void SetAutoMode( void );
void SetManualMode( void );
void OpenCurtain( void );
void CloseCurtain( void );


static void drv_Sensor_Handle(SysParam_t *psSysParamHandle);


/*--------------------------------------------------------------------------------*/
/* 断言 */
void assert_failed(uint8_t* file, uint32_t line)
{
	DEBUG_LOG("file path:%s, line: %d", file, line);
}

static u32 u32SysTick = 0;

#define CMD_NUM 	10

/* 指令列表 */
CmdHandle_t CmdList[CMD_NUM] = 
{

	"开灯",			TurnONLight,
	"关灯",			TurnOffLight,

	"开风扇",			TurnOnAirCondit,
	"关风扇",			TurnOffAirCondit,
	"开窗帘",			OpenCurtain,
	"关窗帘",			CloseCurtain,
	"自动模式",			SetAutoMode,
	"手动模式",			SetManualMode,
	"热点配网",			Key2_short_press_up_handle,
	"一键配网",			Key2_long_press_down_handle,
};

void TurnONLight( void )
{
	if (!LIGHT_SWUTCH_STAT)
	LIGHT_SWITCH_ON;
}

void TurnOffLight( void )
{
	if (LIGHT_SWUTCH_STAT)
	LIGHT_SWITCH_OFF;
}
void TurnOnAirCondit( void )
{
	if (!AIRCONDI_SWUTCH_STAT)
		AIRCONDI_SWITCH_ON;
}

void TurnOffAirCondit( void )
{
	if (AIRCONDI_SWUTCH_STAT)
	AIRCONDI_SWITCH_OFF;
}

void OpenCurtain( void )
{
	if (SysParamHandle.u8CurtainFlg == 0)
	{
		//Motor_PreSet (Status_CW, 5000, 2);
		LED_TIP_ON;
		MotorCwStep(1000);
		
	}
	SysParamHandle.u8CurtainFlg = 1;
}

void CloseCurtain( void )
{
	if (SysParamHandle.u8CurtainFlg == 1)
	{
		//Motor_PreSet (Status_CCW, 5000, 2);
		LED_TIP_OFF;
		MotorCCwStep(1000);
		
	}
	SysParamHandle.u8CurtainFlg = 0;
}
void SetAutoMode( void )
{
	SysParamHandle.eMode = AutoMode;
}

void SetManualMode( void )
{
	SysParamHandle.eMode = ManualMode;
}



/*--------------------------------------------------------------------------------*/
/*
  *Function:  GizwitsDataEventHandle
  * Description：  //页面-主页时间处理
  *Input:  p_SysParamHandle, 系统参数句柄
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
**/
static void GizwitsDataEventHandle ( int code, void *pvParam )
{
	DEBUG_LOG("**********GizwitsDataEventHandle***********\n");
	dataPoint_t *psCurrentDP = (dataPoint_t *)pvParam;
	
}


/*--------------------------------------------------------------------------------*/
/*
  *Function:  KeyEventHandle
  * Description：  //页面-主页时间处理
  *Input:  p_SysParamHandle, 系统参数句柄
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
**/
static void KeyEventHandle( u8 u8_code)
{
	DEBUG_LOG("**********KeyEventHandle***********\n");
	DEBUG_LOG("key_code: %d\n", u8_code);
	/* 按键事件 - 置位 */
	SetBit(SysParamHandle.u8EventReg, KEY_EVENT_BIT);
	SysParamHandle.u8KeyCode = u8_code;
}



/*--------------------------------------------------------------------------------*/
/**
 * name: Sys_timer_handle
 * decription: 处理系统时间片
 * aparam：void
 *
*/
static void Sys_timer_handle( void )
{
	u32SysTick++;
	static u32 warnLed_tick = 0;
	/* 键盘时间片 */
	if (SysParamHandle.p_keyhandle->u32_keyscan_time > 0)
	{
		SysParamHandle.p_keyhandle->u32_keyscan_time--;	
	} 
	if (SysParamHandle.u32SensorTim > 0)
	{
		SysParamHandle.u32SensorTim--;	
	} 
	if (SysParamHandle.u32LcdUpdateTim > 0)
	{
		SysParamHandle.u32LcdUpdateTim--;	
	} 
	if (SysParamHandle.u32SyncDataTim > 0)
	{
		SysParamHandle.u32SyncDataTim--;	
	}
	if (SysParamHandle.u32CycleWarnTim > 0)
	{
		SysParamHandle.u32CycleWarnTim--;	
	}

	if (u32SysTick - warnLed_tick > 500)
	{
		warnLed_tick  = u32SysTick;

		if (SysParamHandle.blWarn == TRUE)
		{
			WARN_LED_FLASH;
		}
		else
		{
			WARN_LED_OFF;
		}
	}
	/* 继电器 */
	gizTimerMs();
}



/**
 * name: Gizwits_Init
 * decription: APP界面初始
 * param: void
*/
void gizwits_dataPoint_Init( void )
{
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));
    
    /** Warning !!! DataPoint Variables Init , Must Within The Data Range **/ 

}

/**
 * name: Gizwits_Init
 * decription: 机智云协议支持相关支持初始化
 * param: void
*/
void Gizwits_Init( void )
{
    uart1_init(9600); 
    gizwitsInit();
    gizwits_dataPoint_Init();
    
    extern GizwitsCallback gfunCallback;
    gfunCallback = GizwitsDataEventHandle;
}

/*--------------------------------------------------------------------------------*/

/**
 * name: Sys_ctl_task
 * decription: 系统控制进程 -- 实际场景条件模拟
 * param: 
 * @psSysParamHandle: 系统操作句柄
*/
void app_Ctl_Task( SysParam_t *p_SysParamHandle )
{
}

/**
 * name: Get_sendor_task
 * decription: 获取传感器数据
 * param: 
 * @p_sensor_data_handle: 传感器操作句柄
*/
void drv_Sensor_Handle( SysParam_t *p_SysParamHandle )
{
 
}


/**
 * name: Server_sync_data
 * decription: 	服务器同步数据，
 *			   	本设计物理通信为Zigbee
 *				通信协议为透传数据传输
 * protocol:	| 帧头-类型 1byte | 人 1byte | 距离 2byte | 外部光强 2byte | 人行灯亮度 1byte | 车行灯亮度 1byte |
 * param: 
 * @psSysParamHandle: 系统操作句柄
*/
void app_SyncData_Task( SysParam_t *p_SysParamHandle )
{
	if (p_SysParamHandle->u32SyncDataTim == 0)
	{
		p_SysParamHandle->u32SyncDataTim = 500;

		p_SysParamHandle->p_DataPoint->valuelight = LIGHT_SWUTCH_STAT;
		p_SysParamHandle->p_DataPoint->valuefan = AIRCONDI_SWUTCH_STAT;
		p_SysParamHandle->p_DataPoint->valuetemperature = p_SysParamHandle->psDHT11DataHandle->temp_int;
		p_SysParamHandle->p_DataPoint->valuehumi = p_SysParamHandle->psDHT11DataHandle->humi_int;

	}
}

/*--------------------------------------------------------------------------------*/
/*
  *Function:  app_GizwitsDataEvent_Handle
  * Description：  //页面-主页时间处理
  *Input:  p_SysParamHandle, 系统参数句柄
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
**/
void app_GizwitsDataEvent_Handle( SysParam_t * p_SysParamHandle )
{
	dataPoint_t	*p_dataPoint = p_SysParamHandle->p_DataPoint;
	if (IsSetBit(p_SysParamHandle->u8EventReg, Gizwits_EVENT_BIT))
	{
		ResetBit(p_SysParamHandle->u8EventReg, Gizwits_EVENT_BIT);
		/* 根据页面 ID 调用事件处理函数 */
		switch (p_SysParamHandle->u8GizwistCode)
		{
			
		}
	}
}

/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
/*
  *Function:  OLED_InitPage
  * Description：  //页面-主页时间处理
  *Input:  p_SysParamHandle, 系统参数句柄
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
**/
void OLED_InitPage(void)
{
	/* 清屏 */
	OLED_CLS();
	
	OLED_ShowCnAndAsciiStr(0, 		 0,		"温度:", 2);
	OLED_ShowCnAndAsciiStr(0, 		 2,		"湿度:", 2);
	OLED_ShowCnAndAsciiStr(0, 		 4,		"光照强度:", 2);
	OLED_ShowCnAndAsciiStr(0, 		 6,		"烟雾浓度:", 2);
	
}

/**
 * name: OLED_UpdateParam
 * decription: OLED 参数更新
 * aparam：void
 *
*/
void app_OledUpdateParam( SysParam_t *p_SysParamHandle )
{
	char str[16] = {0};
	 
}

void app_BspUpdateParam( SysParam_t *p_SysParamHandle )
	
{
//	if (p_SysParamHandle->u16Lightness <= 100)
//		LIGHT_SWITCH_ON;
//	else
//		LIGHT_SWITCH_OFF;
//	if (p_SysParamHandle->fMqValue >= 0.2)
//	{
//		BEEP_ON;
//		WARN_LED_ON;
//	}
//	else
//	{
//		BEEP_OFF;
//		WARN_LED_OFF;
//	}	

	
}


/*--------------------------------------------------------------------------------*/
/*
  *Function:  app_BspInit
  * Description：  //页面-主页时间处理
  *Input:  p_SysParamHandle, 系统参数句柄
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
**/
void app_BspInit( SysParam_t *p_SysParamHandle )
{
	/* debug log uart config */
	DEBUG_UART_Config( );
	DEBUG_LOG("Bsp init\n");
	
	/* 上电延时，等待各个模块运行稳定 */
	delay_ms( 200 );
	
	/* 初始化定时器3， 中断频率 1000hz */
    FML_TIME_Init();
    /*****************************************************************************************************************/
    /* *************************注册系统时间片处理函数************************************************************** */
    /*****************************************************************************************************************/
    FML_TIME_Register(Sys_timer_handle, 1);	//系统时间片，1tick = 1ms
    FML_TIME_Start(Sys_timer_handle); //开启定时 
	/*****************************************************************************************************************/
	/* *************************** 初始化IO设备 ******************************************************************** */
	/*****************************************************************************************************************/
	/* 配置按键引脚 */
	//Key_GPIO_Config(p_SysParamHandle->p_keyhandle);	

	/* LED Fan IO 初始化 */
	LED_Init( );
	BEEP_Init();
	
	Moto_Init();

	/*****************************************************************************************************************/
	/* ************************* 主模块开机运行检测 ***************************************************************** */
	/******************************************************************************************************************/

	/* dht11 初始化 */
	DHT11_Init();
 
}

/*--------------------------------------------------------------------------------*/
/*
  *Function:  app_Process
  * Description：  //页面-主页时间处理
  *Input:  p_SysParamHandle, 系统参数句柄
  *Output:  //输出参数的说明
  *Return:  //函数返回值的说明
  *Others:  //其他说明
**/
void app_Process( SysParam_t *p_SysParamHandle )
{
/***************** 底层驱动 **********************/
	/* 矩阵键盘扫描 - 任务 */
	//drv_Key_Handle( p_SysParamHandle->p_keyhandle );  
	
	/* 传感器数据过去 - 线程 */ 
	drv_Sensor_Handle( p_SysParamHandle );
	
	/* ld3320语音识别模块 - 线程 */ 
	drv_Ld3322_Handle(p_SysParamHandle->psLdHandle);
	
 
	 
	


}

/**
 * name: Key_long_press_Up_handle
 * decription: 短按按键回调函数
*/
void Key1_short_press_up_handle( void )
{
	DEBUG_LOG("Key1 short press\n");
	gizwitsSetMode(WIFI_PRODUCTION_TEST);
}

/**
 * name: Key_long_press_Up_handle
 * decription: 长按按键回调函数
*/
void Key1_long_press_down_handle( void )
{
	DEBUG_LOG("Key1 long press\n");
	gizwitsSetMode(WIFI_RESET_MODE);
}

/**
 * name: Key_long_press_Up_handle
 * decription: 长按按键抬起回调函数
*/
void Key1_long_press_up_handle( void )
{
	DEBUG_LOG("Key1 long press up\n");
}


/**
 * name: Key_long_press_Up_handle
 * decription: 短按按键回调函数
*/
void Key2_short_press_up_handle( void )
{
	DEBUG_LOG("Key2 short press\n");
	LED_TIP_ON;
	delay_ms(800);
	LED_TIP_OFF;
	gizwitsSetMode(WIFI_SOFTAP_MODE);
}

/**
 * name: Key_long_press_Up_handle
 * decription: 长按按键回调函数
*/
void Key2_long_press_down_handle( void )
{
	DEBUG_LOG("Key2 long press\n");
	LED_TIP_ON;
	delay_ms(800);
	LED_TIP_OFF;
	delay_ms(800);
	LED_TIP_ON;
	delay_ms(800);
	LED_TIP_OFF;
	delay_ms(800);
	gizwitsSetMode(WIFI_AIRLINK_MODE);
	LED_TIP_ON;
	delay_ms(200);
	LED_TIP_OFF;
	delay_ms(200);
	LED_TIP_ON;
	delay_ms(200);
	LED_TIP_OFF;
	delay_ms(200);
	LED_TIP_ON;
	delay_ms(200);
	LED_TIP_OFF;
	delay_ms(200);
}

/**
 * name: Key_long_press_Up_handle
 * decription: 长按按键抬起回调函数
*/
void Key2_long_press_up_handle( void )
{
	DEBUG_LOG("Key2 long press up\n");
	LIGHT_OFF;
}

/*--------------------------------------------------------------------------------*/

/****************************************** end file ********************************************/

