#include "task_main.h"
#include "common.h"
#include "delay.h"
#include "usart.h"
#include "relay.h"
#include "input.h"


TaskHandle_t xHandleTaskMAIN = NULL;

u16 MirrorOutPutControlBitState = 0;
u16 MirrorAllRelayState = 0;


void vTaskMAIN(void *pvParameters)
{
	time_t times_sec = 0;
	
	while(1)
	{
		if(DeviceWorkMode == MODE_AUTO)					//只有在自动模式下才进行策略判断
		{
			if(GetSysTick1s() - times_sec >= 1)
			{
				times_sec = GetSysTick1s();

				AutoLoopRegularTimeGroups();
			}
		}
		
		if(MirrorOutPutControlBitState != OutPutControlState || HaveNewActionCommand == 1)
		{
			MirrorOutPutControlBitState = OutPutControlState;
			HaveNewActionCommand = 0;

			ControlAllRelay(MirrorOutPutControlBitState,&OutPutControlBit);
		}

		if(MirrorAllRelayState != AllRelayState)
		{
			MirrorAllRelayState = AllRelayState;

			WriteAllRelayState();						//将继电器状态存储到EEPROM中
		}

		if(NeedToReset == 1)							//接收到重启的命令
		{
			NeedToReset = 0;
			delay_ms(1000);

			__disable_fault_irq();						//重启指令
			NVIC_SystemReset();
		}
		delay_ms(100);
	}
}

//轮询时间策略
void AutoLoopRegularTimeGroups(void)
{
	u8 i = 0;
	time_t seconds_now = 0;
	time_t seconds_24h = 86400;
	time_t seconds_00h = 0;

	if(GetTimeOK != 0)
	{
		seconds_now = calendar.hour * 3600 + calendar.min * 60 + calendar.sec;	//获取当前时分秒对应的秒数

		for(i = 0; i < TimeGroupNumber / 2; i ++)
		{
			switch(RegularTimeStruct[i].type)
			{
				case TYPE_WEEKDAY:		//周一至周五
					if(calendar.week >= 1 && calendar.week <= 5)		//判断现在是否是工作日
					{
						if(RegularTimeStruct[i].s_seconds > RegularTimeStruct[i].e_seconds)			//起始时间比结束时间早一天
						{
							if((RegularTimeStruct[i].s_seconds <= seconds_now && seconds_now <= seconds_24h) || \
								(seconds_00h <= seconds_now && seconds_now <= RegularTimeStruct[i].e_seconds))
							{
								OutPutControlBit = RegularTimeStruct[i].control_bit;
								OutPutControlState = RegularTimeStruct[i].control_state;

								i = TimeGroupNumber / 2;
							}
						}
						else if(RegularTimeStruct[i].s_seconds < RegularTimeStruct[i].e_seconds)	//起始时间和结束时间是同一天
						{
							if(RegularTimeStruct[i].s_seconds <= seconds_now && seconds_now <= RegularTimeStruct[i].e_seconds)
							{
								OutPutControlBit = RegularTimeStruct[i].control_bit;
								OutPutControlState = RegularTimeStruct[i].control_state;

								i = TimeGroupNumber / 2;
							}
						}
					}
				break;

				case TYPE_WEEKEND:		//周六至周日
					if(calendar.week >= 6 && calendar.week <= 7)		//判断现在是否是周六或周日
					{
						if(RegularTimeStruct[i].s_seconds > RegularTimeStruct[i].e_seconds)			//起始时间比结束时间早一天
						{
							if((RegularTimeStruct[i].s_seconds <= seconds_now && seconds_now <= seconds_24h) || \
								(seconds_00h <= seconds_now && seconds_now <= RegularTimeStruct[i].e_seconds))
							{
								OutPutControlBit = RegularTimeStruct[i].control_bit;
								OutPutControlState = RegularTimeStruct[i].control_state;

								i = TimeGroupNumber / 2;
							}
						}
						else if(RegularTimeStruct[i].s_seconds < RegularTimeStruct[i].e_seconds)	//起始时间和结束时间是同一天
						{
							if(RegularTimeStruct[i].s_seconds <= seconds_now && seconds_now <= RegularTimeStruct[i].e_seconds)
							{
								OutPutControlBit = RegularTimeStruct[i].control_bit;
								OutPutControlState = RegularTimeStruct[i].control_state;

								i = TimeGroupNumber / 2;
							}
						}
					}
				break;

				case TYPE_HOLIDAY:		//节假日
					if((RegularTimeStruct[i].s_year + 2000 <= calendar.w_year && calendar.w_year <= RegularTimeStruct[i].e_year + 2000) && \
						(RegularTimeStruct[i].s_month <= calendar.w_month && calendar.w_month <= RegularTimeStruct[i].e_month) && \
						(RegularTimeStruct[i].s_date <= calendar.w_date && calendar.w_date <= RegularTimeStruct[i].e_date))		//判断现在是否是节假日时间区间内
					{
						if(RegularTimeStruct[i].s_seconds > RegularTimeStruct[i].e_seconds)			//起始时间比结束时间早一天
						{
							if((RegularTimeStruct[i].s_seconds <= seconds_now && seconds_now <= seconds_24h) || \
								(seconds_00h <= seconds_now && seconds_now <= RegularTimeStruct[i].e_seconds))
							{
								OutPutControlBit = RegularTimeStruct[i].control_bit;
								OutPutControlState = RegularTimeStruct[i].control_state;

								i = TimeGroupNumber / 2;
							}
						}
						else if(RegularTimeStruct[i].s_seconds < RegularTimeStruct[i].e_seconds)	//起始时间和结束时间是同一天
						{
							if(RegularTimeStruct[i].s_seconds <= seconds_now && seconds_now <= RegularTimeStruct[i].e_seconds)
							{
								OutPutControlBit = RegularTimeStruct[i].control_bit;
								OutPutControlState = RegularTimeStruct[i].control_state;

								i = TimeGroupNumber / 2;
							}
						}
					}
				break;

				default:

				break;
			}
		}
	}
}
































