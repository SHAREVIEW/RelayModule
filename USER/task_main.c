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
	while(1)
	{
		if(MirrorOutPutControlBitState != OutPutControlState)
		{
			MirrorOutPutControlBitState = OutPutControlState;

			ControlAllRelay(MirrorOutPutControlBitState,&OutPutControlBit);
		}

		AllRelayPowerState = GetAllInputState();		//��ȡ�����̵���ͨ���Ƿ��е�Դ����

		AllRelayState = GetAllOutPutState();			//��ȡ�����̵��������״̬

		if(MirrorAllRelayState != AllRelayState)
		{
			MirrorAllRelayState = AllRelayState;

			WriteAllRelayState();						//���̵���״̬�洢��EEPROM��
		}

		if(NeedToReset == 1)							//���յ�����������
		{
			NeedToReset = 0;
			delay_ms(1000);

			__disable_fault_irq();						//����ָ��
			NVIC_SystemReset();
		}
		delay_ms(100);
	}
}


































