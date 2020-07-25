/*
******************************************************************************
* @file    bsp_led.c
* @author  fire
* @version V1.0
* @date    2015-xx-xx
* @brief   ledӦ�ú����ӿ�
******************************************************************************
* @attention
*
* ʵ��ƽ̨:����  STM32 F429 ������  
* ��̳    :http://www.chuxue123.com
* �Ա�    :http://firestm32.taobao.com
*
******************************************************************************
*/
#include "./Bsp/wm8978/bsp_wm8978.h"  
#include "./Bsp/usart/bsp_debug_usart.h"

/**
	*******************************************************************************************************
	*	                     I2C����WM8978���ò��� 
	*******************************************************************************************************
	*/
static void I2C_GPIO_Config(void);
static void I2C_Mode_Configu(void);
static  uint8_t WM8978_I2C_TIMEOUT_UserCallback(void);
static uint8_t WM8978_I2C_WriteRegister(uint8_t RegisterAddr, uint16_t RegisterValue);
static uint16_t wm8978_ReadReg(uint8_t _ucRegAddr);
static uint8_t wm8978_WriteReg(uint8_t _ucRegAddr, uint16_t _usValue);

static __IO uint32_t  WM8978_I2CTimeout = WM8978_I2C_LONG_TIMEOUT;
/*
	wm8978�Ĵ�������
	����WM8978��I2C���߽ӿڲ�֧�ֶ�ȡ��������˼Ĵ���ֵ�������ڴ��У�
	��д�Ĵ���ʱͬ�����»��棬���Ĵ���ʱֱ�ӷ��ػ����е�ֵ��
	�Ĵ���MAP ��WM8978(V4.5_2011).pdf �ĵ�89ҳ���Ĵ�����ַ��7bit�� �Ĵ���������9bit
*/
static uint16_t wm8978_RegCash[] = {
	0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
	0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
	0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
	0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
	0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
	0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
	0x001, 0x001
};

/*****************************************************************
*						           Ӳ��I2C
*****************************************************************/
/**
  * @brief  I2C I/O����
  * @param  ��
  * @retval ��
  */
static void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;    
  /*!< WM8978_I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(WM8978_I2C_CLK, ENABLE);  
  /*!< WM8978_I2C_SCL_GPIO_CLK and WM8978_I2C_SDA_GPIO_CLK Periph clock enable */
  RCC_AHB1PeriphClockCmd(WM8978_I2C_SCL_GPIO_CLK | WM8978_I2C_SDA_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 
   
  /*!< GPIO configuration */
  /* Connect PXx to I2C_SCL*/
  GPIO_PinAFConfig(WM8978_I2C_SCL_GPIO_PORT, WM8978_I2C_SCL_SOURCE, WM8978_I2C_SCL_AF);
  /* Connect PXx to I2C_SDA*/
  GPIO_PinAFConfig(WM8978_I2C_SDA_GPIO_PORT, WM8978_I2C_SDA_SOURCE, WM8978_I2C_SDA_AF);  
  
  /*!< Configure WM8978_I2C pins: SCL */   
  GPIO_InitStructure.GPIO_Pin = WM8978_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(WM8978_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure WM8978_I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = WM8978_I2C_SDA_PIN;
  GPIO_Init(WM8978_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  I2C ����ģʽ����
  * @param  ��
  * @retval ��
  */
static void I2C_Mode_Configu(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 

  /* I2C ���� */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;	
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;/* �ߵ�ƽ�����ȶ����͵�ƽ���ݱ仯 SCL ʱ���ߵ�ռ�ձ� */
  I2C_InitStructure.I2C_OwnAddress1 =0x0A; 
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	/* I2C��Ѱַģʽ */
  I2C_InitStructure.I2C_ClockSpeed = WM8978_I2C_Speed;	/* ͨ������ */
  I2C_Init(WM8978_I2C, &I2C_InitStructure);	/* I2C ��ʼ�� */
  I2C_Cmd(WM8978_I2C, ENABLE);  /* ʹ�� I2C */

  I2C_AcknowledgeConfig(WM8978_I2C, ENABLE);
}
 
/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
	* @retval 0:��ʱ
  */
static  uint8_t WM8978_I2C_TIMEOUT_UserCallback(void)
{
  /* Block communication and all processes */
  printf("I2C Timeout error!");
  return 0;
}
/**
  * @brief  Writes a Byte to a given register into the audio codec through the 
            control interface (I2C)
  * @param  RegisterAddr: The address (location) of the register to be written.
  * @param  RegisterValue: the Byte value to be written into destination register.
  * @retval 1 if correct communication and 0 if wrong communication
  */
static uint8_t WM8978_I2C_WriteRegister(uint8_t RegisterAddr, uint16_t RegisterValue)
{	  
  /* Start the config sequence */
  I2C_GenerateSTART(WM8978_I2C, ENABLE);

  /* Test on EV5 and clear it */
  WM8978_I2CTimeout = WM8978_I2C_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(WM8978_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((WM8978_I2CTimeout--) == 0) return WM8978_I2C_TIMEOUT_UserCallback();
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(WM8978_I2C, WM8978_SLAVE_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  WM8978_I2CTimeout = WM8978_I2C_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(WM8978_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((WM8978_I2CTimeout--) == 0) return WM8978_I2C_TIMEOUT_UserCallback();
  }

  /* Transmit the first address for write operation */
	I2C_SendData(WM8978_I2C,((RegisterAddr << 1) & 0xFE) | ((RegisterValue >> 8) & 0x1));
	
  /* Test on EV8 and clear it */
  WM8978_I2CTimeout = WM8978_I2C_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(WM8978_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
  {
    if((WM8978_I2CTimeout--) == 0) return WM8978_I2C_TIMEOUT_UserCallback();
  }
  
  /* Prepare the register value to be sent */
  I2C_SendData(WM8978_I2C, RegisterValue&0xff);
	
  /*!< Wait till all data have been physically transferred on the bus */
  WM8978_I2CTimeout = WM8978_I2C_LONG_TIMEOUT;
  while(!I2C_GetFlagStatus(WM8978_I2C, I2C_FLAG_BTF))
  {
    if((WM8978_I2CTimeout--) == 0) WM8978_I2C_TIMEOUT_UserCallback();
  }
  
  /* End the configuration sequence */
  I2C_GenerateSTOP(WM8978_I2C, ENABLE);

  /* Return the verifying value: 0 (Passed) or 1 (Failed) */
  return 1;  
}

/**
	* @brief  ��cash�ж��ض���wm8978�Ĵ���
	* @param  _ucRegAddr �� �Ĵ�����ַ
	* @retval �Ĵ���ֵ
	*/
static uint16_t wm8978_ReadReg(uint8_t _ucRegAddr)
{
	return wm8978_RegCash[_ucRegAddr];
}


/**
	* @brief  дwm8978�Ĵ���
	* @param  _ucRegAddr�� �Ĵ�����ַ
	* @param  _usValue�� �Ĵ���ֵ
	* @retval 0��д��ʧ��
  *         1��д��ɹ�
	*/
static uint8_t wm8978_WriteReg(uint8_t _ucRegAddr, uint16_t _usValue)
{
	uint8_t res;
	res=WM8978_I2C_WriteRegister(_ucRegAddr,_usValue);
	wm8978_RegCash[_ucRegAddr] = _usValue;
	return res;
}

/**
	* @brief  ����I2C GPIO�������I2C�����ϵ�WM8978�Ƿ�����
	* @param  ��
	* @retval 1,��ʼ���ɹ�;
	*         0,��ʼ��ʧ�ܡ�
	*/
uint8_t wm8978_Init(void)
{
	uint8_t res;
	I2C_GPIO_Config();
	I2C_Mode_Configu();
	res=wm8978_Reset();			/* Ӳ����λWM8978���мĴ�����ȱʡ״̬ */
	return res;
}

/**
	* @brief  �޸����ͨ��1����
	* @param  _ucVolume ������ֵ, 0-63
	* @retval ��
	*/
void wm8978_SetOUT1Volume(uint8_t _ucVolume)
{
	uint16_t regL;
	uint16_t regR;

	if (_ucVolume > VOLUME_MAX)
	{
		_ucVolume = VOLUME_MAX;
	}

	regL = _ucVolume;
	regR = _ucVolume;

	/*
		R52	LOUT1 Volume control
		R53	ROUT1 Volume control
	*/
	/* �ȸ�������������ֵ */
	wm8978_WriteReg(52, regL | 0x00);

	/* ��ͬ�������������������� */
	wm8978_WriteReg(53, regR | 0x100);	/* 0x180��ʾ ������Ϊ0ʱ�ٸ��£���������������ֵġ����ա��� */
}


/**
	* @brief  �޸����ͨ��2����
	* @param  _ucVolume ������ֵ, 0-63
	* @retval ��
	*/
void wm8978_SetOUT2Volume(uint8_t _ucVolume)
{
	uint16_t regL;
	uint16_t regR;

	if (_ucVolume > VOLUME_MAX)
	{
		_ucVolume = VOLUME_MAX;
	}

	regL = _ucVolume;
	regR = _ucVolume;

	/*
		R54	LOUT2 (SPK) Volume control
		R55	ROUT2 (SPK) Volume control
	*/
	/* �ȸ�������������ֵ */
	wm8978_WriteReg(54, regL | 0x00);

	/* ��ͬ�������������������� */
	
	wm8978_WriteReg(55, regR | 0x100);	/* ������Ϊ0ʱ�ٸ��£���������������ֵġ����ա��� */
}


/**
	* @brief  ��ȡ���ͨ��1����
	* @param  ��
	* @retval ��ǰ����ֵ
	*/
uint8_t wm8978_ReadOUT1Volume(void)
{
	return (uint8_t)(wm8978_ReadReg(52) & 0x3F );
}

/**
	* @brief  ��ȡ���ͨ��2����
	* @param  ��
	* @retval ��ǰ����ֵ
	*/
uint8_t wm8978_ReadOUT2Volume(void)
{
	return (uint8_t)(wm8978_ReadReg(54) & 0x3F );
}


/**
	* @brief  �������.
	* @param  _ucMute��ģʽѡ��
	*         @arg 1������
	*         @arg 0��ȡ������
	* @retval ��
	*/
void wm8978_OutMute(uint8_t _ucMute)
{
	uint16_t usRegValue;

	if (_ucMute == 1) /* ���� */
	{
		usRegValue = wm8978_ReadReg(52); /* Left Mixer Control */
		usRegValue |= (1u << 6);
		wm8978_WriteReg(52, usRegValue);

		usRegValue = wm8978_ReadReg(53); /* Left Mixer Control */
		usRegValue |= (1u << 6);
		wm8978_WriteReg(53, usRegValue);

		usRegValue = wm8978_ReadReg(54); /* Right Mixer Control */
		usRegValue |= (1u << 6);
		wm8978_WriteReg(54, usRegValue);

		usRegValue = wm8978_ReadReg(55); /* Right Mixer Control */
		usRegValue |= (1u << 6);
		wm8978_WriteReg(55, usRegValue);
	}
	else	/* ȡ������ */
	{
		usRegValue = wm8978_ReadReg(52);
		usRegValue &= ~(1u << 6);
		wm8978_WriteReg(52, usRegValue);

		usRegValue = wm8978_ReadReg(53); /* Left Mixer Control */
		usRegValue &= ~(1u << 6);
		wm8978_WriteReg(53, usRegValue);

		usRegValue = wm8978_ReadReg(54);
		usRegValue &= ~(1u << 6);
		wm8978_WriteReg(54, usRegValue);

		usRegValue = wm8978_ReadReg(55); /* Left Mixer Control */
		usRegValue &= ~(1u << 6);
		wm8978_WriteReg(55, usRegValue);
	}
}

/**
	* @brief  ��������
	* @param  _ucGain ������ֵ, 0-63
	* @retval ��
	*/
void wm8978_SetMicGain(uint8_t _ucGain)
{
	if (_ucGain > GAIN_MAX)
	{
		_ucGain = GAIN_MAX;
	}

	/* PGA ��������  R45�� R46 
		Bit8	INPPGAUPDATE
		Bit7	INPPGAZCL		�����ٸ���
		Bit6	INPPGAMUTEL		PGA����
		Bit5:0	����ֵ��010000��0dB
	*/
	wm8978_WriteReg(45, _ucGain);
	wm8978_WriteReg(46, _ucGain | (1 << 8));
}


/**
	* @brief  ����Line����ͨ��������
	* @param  _ucGain ������ֵ, 0-7. 7���0��С�� ��˥���ɷŴ�
	* @retval ��
	*/
void wm8978_SetLineGain(uint8_t _ucGain)
{
	uint16_t usRegValue;

	if (_ucGain > 7)
	{
		_ucGain = 7;
	}

	/*
		Mic �����ŵ��������� PGABOOSTL �� PGABOOSTR ����
		Aux �����ŵ������������� AUXL2BOOSTVO[2:0] �� AUXR2BOOSTVO[2:0] ����
		Line �����ŵ��������� LIP2BOOSTVOL[2:0] �� RIP2BOOSTVOL[2:0] ����
	*/
	/*	R47������������R48����������, MIC ������ƼĴ���
		R47 (R48���������ͬ)
		B8		PGABOOSTL	= 1,   0��ʾMIC�ź�ֱͨ�����棬1��ʾMIC�ź�+20dB���棨ͨ���Ծٵ�·��
		B7		= 0�� ����
		B6:4	L2_2BOOSTVOL = x�� 0��ʾ��ֹ��1-7��ʾ����-12dB ~ +6dB  ������˥��Ҳ���ԷŴ�
		B3		= 0�� ����
		B2:0`	AUXL2BOOSTVOL = x��0��ʾ��ֹ��1-7��ʾ����-12dB ~ +6dB  ������˥��Ҳ���ԷŴ�
	*/

	usRegValue = wm8978_ReadReg(47);
	usRegValue &= 0x8F;/* ��Bit6:4��0   1000 1111*/
	usRegValue |= (_ucGain << 4);
	wm8978_WriteReg(47, usRegValue);	/* д����������������ƼĴ��� */

	usRegValue = wm8978_ReadReg(48);
	usRegValue &= 0x8F;/* ��Bit6:4��0   1000 1111*/
	usRegValue |= (_ucGain << 4);
	wm8978_WriteReg(48, usRegValue);	/* д����������������ƼĴ��� */
}

/**
	* @brief  �ر�wm8978������͹���ģʽ
	* @param  ��
	* @retval ��
	*/
void wm8978_PowerDown(void)
{
	wm8978_Reset();			/* Ӳ����λWM8978���мĴ�����ȱʡ״̬ */
}

/**
	* @brief  ����WM8978����Ƶ�ӿ�(I2S)
	* @param  _usStandard : �ӿڱ�׼��I2S_Standard_Phillips, I2S_Standard_MSB �� I2S_Standard_LSB
	* @param  _ucWordLen : �ֳ���16��24��32  �����������õ�20bit��ʽ��
	* @retval ��
	*/
void wm8978_CfgAudioIF(uint16_t _usStandard, uint8_t _ucWordLen)
{
	uint16_t usReg;

	/* WM8978(V4.5_2011).pdf 73ҳ���Ĵ����б� */

	/*	REG R4, ��Ƶ�ӿڿ��ƼĴ���
		B8		BCP	 = X, BCLK���ԣ�0��ʾ������1��ʾ����
		B7		LRCP = x, LRCʱ�Ӽ��ԣ�0��ʾ������1��ʾ����
		B6:5	WL = x�� �ֳ���00=16bit��01=20bit��10=24bit��11=32bit ���Ҷ���ģʽֻ�ܲ��������24bit)
		B4:3	FMT = x����Ƶ���ݸ�ʽ��00=�Ҷ��룬01=����룬10=I2S��ʽ��11=PCM
		B2		DACLRSWAP = x, ����DAC���ݳ�����LRCʱ�ӵ���߻����ұ�
		B1 		ADCLRSWAP = x������ADC���ݳ�����LRCʱ�ӵ���߻����ұ�
		B0		MONO	= 0��0��ʾ��������1��ʾ������������������Ч
	*/
	usReg = 0;
	if (_usStandard == I2S_Standard_Phillips)	/* I2S�����ֱ�׼ */
	{
		usReg |= (2 << 3);
	}
	else if (_usStandard == I2S_Standard_MSB)	/* MSB�����׼(�����) */
	{
		usReg |= (1 << 3);
	}
	else if (_usStandard == I2S_Standard_LSB)	/* LSB�����׼(�Ҷ���) */
	{
		usReg |= (0 << 3);
	}
	else	/* PCM��׼(16λͨ��֡�ϴ������֡ͬ������16λ����֡��չΪ32λͨ��֡) */
	{
		usReg |= (3 << 3);;
	}

	if (_ucWordLen == 24)
	{
		usReg |= (2 << 5);
	}
	else if (_ucWordLen == 32)
	{
		usReg |= (3 << 5);
	}
	else
	{
		usReg |= (0 << 5);		/* 16bit */
	}
	wm8978_WriteReg(4, usReg);


	/*
		R6��ʱ�Ӳ������ƼĴ���
		MS = 0,  WM8978����ʱ�ӣ���MCU�ṩMCLKʱ��
	*/
	wm8978_WriteReg(6, 0x000);
}




/**
	* @brief  ����wm8978��Ƶͨ��
	* @param  _InPath : ��Ƶ����ͨ������
	* @param  _OutPath : ��Ƶ���ͨ������
	* @retval ��
	*/
void wm8978_CfgAudioPath(uint16_t _InPath, uint16_t _OutPath)
{
	uint16_t usReg;

	/* �鿴WM8978�����ֲ�� REGISTER MAP �½ڣ� ��89ҳ */

	if ((_InPath == IN_PATH_OFF) && (_OutPath == OUT_PATH_OFF))
	{
		wm8978_PowerDown();
		return;
	}

	/*
		R1 �Ĵ��� Power manage 1
		Bit8    BUFDCOPEN,  Output stage 1.5xAVDD/2 driver enable
 		Bit7    OUT4MIXEN, OUT4 mixer enable
		Bit6    OUT3MIXEN, OUT3 mixer enable
		Bit5    PLLEN	.����
		Bit4    MICBEN	,Microphone Bias Enable (MICƫ�õ�·ʹ��)
		Bit3    BIASEN	,Analogue amplifier bias control ��������Ϊ1ģ��Ŵ����Ź���
		Bit2    BUFIOEN , Unused input/output tie off buffer enable
		Bit1:0  VMIDSEL, ��������Ϊ��00ֵģ��Ŵ����Ź���
	*/
	usReg = (1 << 3) | (3 << 0);
	if (_OutPath & OUT3_4_ON) 	/* OUT3��OUT4ʹ�������GSMģ�� */
	{
		usReg |= ((1 << 7) | (1 << 6));
	}
	if ((_InPath & MIC_LEFT_ON) || (_InPath & MIC_RIGHT_ON))
	{
		usReg |= (1 << 4);
	}
	wm8978_WriteReg(1, usReg);	/* д�Ĵ��� */

	/*
		R2 �Ĵ��� Power manage 2
		Bit8	ROUT1EN,	ROUT1 output enable �������������ʹ��
		Bit7	LOUT1EN,	LOUT1 output enable �������������ʹ��
		Bit6	SLEEP, 		0 = Normal device operation   1 = Residual current reduced in device standby mode
		Bit5	BOOSTENR,	Right channel Input BOOST enable ����ͨ���Ծٵ�·ʹ��. �õ�PGA�Ŵ���ʱ����ʹ��
		Bit4	BOOSTENL,	Left channel Input BOOST enable
		Bit3	INPGAENR,	Right channel input PGA enable ����������PGAʹ��
		Bit2	INPGAENL,	Left channel input PGA enable
		Bit1	ADCENR,		Enable ADC right channel
		Bit0	ADCENL,		Enable ADC left channel
	*/
	usReg = 0;
	if (_OutPath & EAR_LEFT_ON)
	{
		usReg |= (1 << 7);
	}
	if (_OutPath & EAR_RIGHT_ON)
	{
		usReg |= (1 << 8);
	}
	if (_InPath & MIC_LEFT_ON)
	{
		usReg |= ((1 << 4) | (1 << 2));
	}
	if (_InPath & MIC_RIGHT_ON)
	{
		usReg |= ((1 << 5) | (1 << 3));
	}
	if (_InPath & LINE_ON)
	{
		usReg |= ((1 << 4) | (1 << 5));
	}
	if (_InPath & MIC_RIGHT_ON)
	{
		usReg |= ((1 << 5) | (1 << 3));
	}
	if (_InPath & ADC_ON)
	{
		usReg |= ((1 << 1) | (1 << 0));
	}
	wm8978_WriteReg(2, usReg);	/* д�Ĵ��� */

	/*
		R3 �Ĵ��� Power manage 3
		Bit8	OUT4EN,		OUT4 enable
		Bit7	OUT3EN,		OUT3 enable
		Bit6	LOUT2EN,	LOUT2 output enable
		Bit5	ROUT2EN,	ROUT2 output enable
		Bit4	0,
		Bit3	RMIXEN,		Right mixer enable
		Bit2	LMIXEN,		Left mixer enable
		Bit1	DACENR,		Right channel DAC enable
		Bit0	DACENL,		Left channel DAC enable
	*/
	usReg = 0;
	if (_OutPath & OUT3_4_ON)
	{
		usReg |= ((1 << 8) | (1 << 7));
	}
	if (_OutPath & SPK_ON)
	{
		usReg |= ((1 << 6) | (1 << 5));
	}
	if (_OutPath != OUT_PATH_OFF)
	{
		usReg |= ((1 << 3) | (1 << 2));
	}
	if (_InPath & DAC_ON)
	{
		usReg |= ((1 << 1) | (1 << 0));
	}
	wm8978_WriteReg(3, usReg);	/* д�Ĵ��� */

	/*
		R44 �Ĵ��� Input ctrl

		Bit8	MBVSEL, 		Microphone Bias Voltage Control   0 = 0.9 * AVDD   1 = 0.6 * AVDD
		Bit7	0
		Bit6	R2_2INPPGA,		Connect R2 pin to right channel input PGA positive terminal
		Bit5	RIN2INPPGA,		Connect RIN pin to right channel input PGA negative terminal
		Bit4	RIP2INPPGA,		Connect RIP pin to right channel input PGA amplifier positive terminal
		Bit3	0
		Bit2	L2_2INPPGA,		Connect L2 pin to left channel input PGA positive terminal
		Bit1	LIN2INPPGA,		Connect LIN pin to left channel input PGA negative terminal
		Bit0	LIP2INPPGA,		Connect LIP pin to left channel input PGA amplifier positive terminal
	*/
	usReg = 0 << 8;
	if (_InPath & LINE_ON)
	{
		usReg |= ((1 << 6) | (1 << 2));
	}
	if (_InPath & MIC_RIGHT_ON)
	{
		usReg |= ((1 << 5) | (1 << 4));
	}
	if (_InPath & MIC_LEFT_ON)
	{
		usReg |= ((1 << 1) | (1 << 0));
	}
	wm8978_WriteReg(44, usReg);	/* д�Ĵ��� */


	/*
		R14 �Ĵ��� ADC Control
		���ø�ͨ�˲�������ѡ�ģ� WM8978(V4.5_2011).pdf 31 32ҳ,
		Bit8 	HPFEN,	High Pass Filter Enable��ͨ�˲���ʹ�ܣ�0��ʾ��ֹ��1��ʾʹ��
		BIt7 	HPFAPP,	Select audio mode or application mode ѡ����Ƶģʽ��Ӧ��ģʽ��0��ʾ��Ƶģʽ��
		Bit6:4	HPFCUT��Application mode cut-off frequency  000-111ѡ��Ӧ��ģʽ�Ľ�ֹƵ��
		Bit3 	ADCOSR,	ADC oversample rate select: 0=64x (lower power) 1=128x (best performance)
		Bit2   	0
		Bit1 	ADC right channel polarity adjust:  0=normal  1=inverted
		Bit0 	ADC left channel polarity adjust:  0=normal 1=inverted
	*/
	if (_InPath & ADC_ON)
	{
		usReg = (1 << 3) | (0 << 8) | (4 << 0);		/* ��ֹADC��ͨ�˲���, ���ý���Ƶ�� */
	}
	else
	{
		usReg = 0;
	}
	wm8978_WriteReg(14, usReg);	/* д�Ĵ��� */

	/* �����ݲ��˲�����notch filter������Ҫ�������ƻ�Ͳ����������������Х��.  ��ʱ�ر�
		R27��R28��R29��R30 ���ڿ����޲��˲�����WM8978(V4.5_2011).pdf 33ҳ
		R7�� Bit7 NFEN = 0 ��ʾ��ֹ��1��ʾʹ��
	*/
	if (_InPath & ADC_ON)
	{
		usReg = (0 << 7);
		wm8978_WriteReg(27, usReg);	/* д�Ĵ��� */
		usReg = 0;
		wm8978_WriteReg(28, usReg);	/* д�Ĵ���,��0����Ϊ�Ѿ���ֹ������Ҳ�ɲ��� */
		wm8978_WriteReg(29, usReg);	/* д�Ĵ���,��0����Ϊ�Ѿ���ֹ������Ҳ�ɲ��� */
		wm8978_WriteReg(30, usReg);	/* д�Ĵ���,��0����Ϊ�Ѿ���ֹ������Ҳ�ɲ��� */
	}

	/* �Զ�������� ALC, R32  - 34  WM8978(V4.5_2011).pdf 36ҳ */
	{
		usReg = 0;		/* ��ֹ�Զ�������� */
		wm8978_WriteReg(32, usReg);
		wm8978_WriteReg(33, usReg);
		wm8978_WriteReg(34, usReg);
	}

	/*  R35  ALC Noise Gate Control
		Bit3	NGATEN, Noise gate function enable
		Bit2:0	Noise gate threshold:
	*/
	usReg = (3 << 1) | (7 << 0);		/* ��ֹ�Զ�������� */
	wm8978_WriteReg(35, usReg);

	/*
		Mic �����ŵ��������� PGABOOSTL �� PGABOOSTR ����
		Aux �����ŵ������������� AUXL2BOOSTVO[2:0] �� AUXR2BOOSTVO[2:0] ����
		Line �����ŵ��������� LIP2BOOSTVOL[2:0] �� RIP2BOOSTVOL[2:0] ����
	*/
	/*	WM8978(V4.5_2011).pdf 29ҳ��R47������������R48����������, MIC ������ƼĴ���
		R47 (R48���������ͬ)
		B8		PGABOOSTL	= 1,   0��ʾMIC�ź�ֱͨ�����棬1��ʾMIC�ź�+20dB���棨ͨ���Ծٵ�·��
		B7		= 0�� ����
		B6:4	L2_2BOOSTVOL = x�� 0��ʾ��ֹ��1-7��ʾ����-12dB ~ +6dB  ������˥��Ҳ���ԷŴ�
		B3		= 0�� ����
		B2:0`	AUXL2BOOSTVOL = x��0��ʾ��ֹ��1-7��ʾ����-12dB ~ +6dB  ������˥��Ҳ���ԷŴ�
	*/
	usReg = 0;
	if ((_InPath & MIC_LEFT_ON) || (_InPath & MIC_RIGHT_ON))
	{
		usReg |= (1 << 8);	/* MIC����ȡ+20dB */
	}
	if (_InPath & AUX_ON)
	{
		usReg |= (3 << 0);	/* Aux����̶�ȡ3���û��������е��� */
	}
	if (_InPath & LINE_ON)
	{
		usReg |= (3 << 4);	/* Line����̶�ȡ3���û��������е��� */
	}
	wm8978_WriteReg(47, usReg);	/* д����������������ƼĴ��� */
	wm8978_WriteReg(48, usReg);	/* д����������������ƼĴ��� */

	/* ����ADC�������ƣ�pdf 35ҳ
		R15 ����������ADC������R16����������ADC����
		Bit8 	ADCVU  = 1 ʱ�Ÿ��£�����ͬ����������������ADC����
		Bit7:0 	����ѡ�� 0000 0000 = ����
						   0000 0001 = -127dB
						   0000 0010 = -12.5dB  ��0.5dB ������
						   1111 1111 = 0dB  ����˥����
	*/
	usReg = 0xFF;
	wm8978_WriteReg(15, usReg);	/* ѡ��0dB���Ȼ��������� */
	usReg = 0x1FF;
	wm8978_WriteReg(16, usReg);	/* ͬ�������������� */

	/* ͨ�� wm8978_SetMicGain ��������mic PGA���� */

	/*	R43 �Ĵ���  AUXR �C ROUT2 BEEP Mixer Function
		B8:6 = 0

		B5	 MUTERPGA2INV,	Mute input to INVROUT2 mixer
		B4	 INVROUT2,  Invert ROUT2 output �����������������
		B3:1 BEEPVOL = 7;	AUXR input to ROUT2 inverter gain
		B0	 BEEPEN = 1;	Enable AUXR beep input

	*/
	usReg = 0;
	if (_OutPath & SPK_ON)
	{
		usReg |= (1 << 4);	/* ROUT2 ����, �������������� */
	}
	if (_InPath & AUX_ON)
	{
		usReg |= ((7 << 1) | (1 << 0));
	}
	wm8978_WriteReg(43, usReg);

	/* R49  Output ctrl
		B8:7	0
		B6		DACL2RMIX,	Left DAC output to right output mixer
		B5		DACR2LMIX,	Right DAC output to left output
		B4		OUT4BOOST,	0 = OUT4 output gain = -1; DC = AVDD / 2��1 = OUT4 output gain = +1.5��DC = 1.5 x AVDD / 2
		B3		OUT3BOOST,	0 = OUT3 output gain = -1; DC = AVDD / 2��1 = OUT3 output gain = +1.5��DC = 1.5 x AVDD / 2
		B2		SPKBOOST,	0 = Speaker gain = -1;  DC = AVDD / 2 ; 1 = Speaker gain = +1.5; DC = 1.5 x AVDD / 2
		B1		TSDEN,   Thermal Shutdown Enable  �������ȱ���ʹ�ܣ�ȱʡ1��
		B0		VROI,	Disabled Outputs to VREF Resistance
	*/
	usReg = 0;
	if (_InPath & DAC_ON)
	{
		usReg |= ((1 << 6) | (1 << 5));
	}
	if (_OutPath & SPK_ON)
	{
		usReg |=  ((1 << 2) | (1 << 1));	/* SPK 1.5x����,  �ȱ���ʹ�� */
	}
	if (_OutPath & OUT3_4_ON)
	{
		usReg |=  ((1 << 4) | (1 << 3));	/* BOOT3  BOOT4  1.5x���� */
	}
	wm8978_WriteReg(49, usReg);

	/*	REG 50    (50����������51�������������üĴ�������һ��) WM8978(V4.5_2011).pdf 56ҳ
		B8:6	AUXLMIXVOL = 111	AUX����FM�������ź�����
		B5		AUXL2LMIX = 1		Left Auxilliary input to left channel
		B4:2	BYPLMIXVOL			����
		B1		BYPL2LMIX = 0;		Left bypass path (from the left channel input boost output) to left output mixer
		B0		DACL2LMIX = 1;		Left DAC output to left output mixer
	*/
	usReg = 0;
	if (_InPath & AUX_ON)
	{
		usReg |= ((7 << 6) | (1 << 5));
	}
	if ((_InPath & LINE_ON) || (_InPath & MIC_LEFT_ON) || (_InPath & MIC_RIGHT_ON))
	{
		usReg |= ((7 << 2) | (1 << 1));
	}
	if (_InPath & DAC_ON)
	{
		usReg |= (1 << 0);
	}
	wm8978_WriteReg(50, usReg);
	wm8978_WriteReg(51, usReg);

	/*	R56 �Ĵ���   OUT3 mixer ctrl
		B8:7	0
		B6		OUT3MUTE,  	0 = Output stage outputs OUT3 mixer;  1 = Output stage muted �C drives out VMID.
		B5:4	0
		B3		BYPL2OUT3,	OUT4 mixer output to OUT3  (����)
		B4		0
		B2		LMIX2OUT3,	Left ADC input to OUT3
		B1		LDAC2OUT3,	Left DAC mixer to OUT3
		B0		LDAC2OUT3,	Left DAC output to OUT3
	*/
	usReg = 0;
	if (_OutPath & OUT3_4_ON)
	{
		usReg |= (1 << 3);
	}
	wm8978_WriteReg(56, usReg);

	/* R57 �Ĵ���		OUT4 (MONO) mixer ctrl
		B8:7	0
		B6		OUT4MUTE,	0 = Output stage outputs OUT4 mixer  1 = Output stage muted �C drives outVMID.
		B5		HALFSIG,	0 = OUT4 normal output	1 = OUT4 attenuated by 6dB
		B4		LMIX2OUT4,	Left DAC mixer to OUT4
		B3		LDAC2UT4,	Left DAC to OUT4
		B2		BYPR2OUT4,	Right ADC input to OUT4
		B1		RMIX2OUT4,	Right DAC mixer to OUT4
		B0		RDAC2OUT4,	Right DAC output to OUT4
	*/
	usReg = 0;
	if (_OutPath & OUT3_4_ON)
	{
		usReg |= ((1 << 4) |  (1 << 1));
	}
	wm8978_WriteReg(57, usReg);


	/* R11, 12 �Ĵ��� DAC��������
		R11		Left DAC Digital Volume
		R12		Right DAC Digital Volume
	*/
	if (_InPath & DAC_ON)
	{
		wm8978_WriteReg(11, 255);
		wm8978_WriteReg(12, 255 | 0x100);
	}
	else
	{
		wm8978_WriteReg(11, 0);
		wm8978_WriteReg(12, 0 | 0x100);
	}

	/*	R10 �Ĵ��� DAC Control
		B8	0
		B7	0
		B6	SOFTMUTE,	Softmute enable:
		B5	0
		B4	0
		B3	DACOSR128,	DAC oversampling rate: 0=64x (lowest power) 1=128x (best performance)
		B2	AMUTE,		Automute enable
		B1	DACPOLR,	Right DAC output polarity
		B0	DACPOLL,	Left DAC output polarity:
	*/
	if (_InPath & DAC_ON)
	{
		wm8978_WriteReg(10, 0);
	}
}

/**
	* @brief  �����ݲ��˲�����notch filter������Ҫ�������ƻ�Ͳ����������������Х��
	* @param  NFA0[13:0] and NFA1[13:0]
	* @retval ��
	*/
void wm8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1)
{
	uint16_t usReg;

	/*  page 26
		A programmable notch filter is provided. This filter has a variable centre frequency and bandwidth,
		programmable via two coefficients, a0 and a1. a0 and a1 are represented by the register bits
		NFA0[13:0] and NFA1[13:0]. Because these coefficient values require four register writes to setup
		there is an NFU (Notch Filter Update) flag which should be set only when all four registers are setup.
	*/
	usReg = (1 << 7) | (_NFA0 & 0x3F);
	wm8978_WriteReg(27, usReg);	/* д�Ĵ��� */

	usReg = ((_NFA0 >> 7) & 0x3F);
	wm8978_WriteReg(28, usReg);	/* д�Ĵ��� */

	usReg = (_NFA1 & 0x3F);
	wm8978_WriteReg(29, usReg);	/* д�Ĵ��� */

	usReg = (1 << 8) | ((_NFA1 >> 7) & 0x3F);
	wm8978_WriteReg(30, usReg);	/* д�Ĵ��� */
}


/**
	* @brief  ����WM8978��GPIO1�������0��1
	* @param  _ucValue ��GPIO1���ֵ��0��1
	* @retval ��
	*/
void wm8978_CtrlGPIO1(uint8_t _ucValue)
{
	uint16_t usRegValue;

	/* R8�� pdf 62ҳ */
	if (_ucValue == 0) /* ���0 */
	{
		usRegValue = 6; /* B2:0 = 110 */
	}
	else
	{
		usRegValue = 7; /* B2:0 = 111 */
	}
	wm8978_WriteReg(8, usRegValue);
}


/**
	* @brief  ��λwm8978�����еļĴ���ֵ�ָ���ȱʡֵ
	* @param  ��
	* @retval 1: ��λ�ɹ�
	* 				0����λʧ��
	*/
uint8_t wm8978_Reset(void)
{
	/* wm8978�Ĵ���ȱʡֵ */
	const uint16_t reg_default[] = {
	0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
	0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
	0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
	0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
	0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
	0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
	0x001, 0x001
	};
	uint8_t res;
	uint8_t i;

	res=wm8978_WriteReg(0x00, 0);

	for (i = 0; i < sizeof(reg_default) / 2; i++)
	{
		wm8978_RegCash[i] = reg_default[i];
	}
	return res;
}

/**
	*******************************************************************************************************
	*	                     ����Ĵ����Ǻ�STM32 I2SӲ����ص�
	*******************************************************************************************************
	*/

/*  I2S DMA�ص�����ָ��  */
void (*I2S_DMA_TX_Callback)(void);	//I2S DMA �ص�����
void (*I2S_DMA_RX_Callback)(void);	//I2S DMA RX�ص�����

/**
	* @brief  ����GPIO��������codecӦ��
	* @param  ��
	* @retval ��
	*/
void I2S_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

/**
	* I2S���ߴ�����Ƶ���ݿ���
	* WM8978_LRC    -> PB12/I2S2_WS
	* WM8978_BCLK   -> PD3/I2S2_CK
	* WM8978_ADCDAT -> PC2/I2S2ext_SD
	* WM8978_DACDAT -> PI3/I2S2_SD
	* WM8978_MCLK   -> PC6/I2S2_MCK
	*/	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(WM8978_LRC_GPIO_CLK|WM8978_BCLK_GPIO_CLK| \
                         WM8978_ADCDAT_GPIO_CLK|WM8978_DACDAT_GPIO_CLK| \
	                       WM8978_MCLK_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = WM8978_LRC_PIN;
	GPIO_Init(WM8978_LRC_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = WM8978_BCLK_PIN;
	GPIO_Init(WM8978_BCLK_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = WM8978_ADCDAT_PIN;
	GPIO_Init(WM8978_ADCDAT_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = WM8978_DACDAT_PIN;
	GPIO_Init(WM8978_DACDAT_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = WM8978_MCLK_PIN;
	GPIO_Init(WM8978_MCLK_PORT, &GPIO_InitStructure);
	
	/* Connect pins to I2S peripheral  */
	GPIO_PinAFConfig(WM8978_LRC_PORT,    WM8978_LRC_SOURCE,    WM8978_LRC_AF);
	GPIO_PinAFConfig(WM8978_BCLK_PORT,   WM8978_BCLK_SOURCE,   WM8978_BCLK_AF);
	GPIO_PinAFConfig(WM8978_ADCDAT_PORT, WM8978_ADCDAT_SOURCE, WM8978_ADCDAT_AF);
	GPIO_PinAFConfig(WM8978_DACDAT_PORT, WM8978_DACDAT_SOURCE, WM8978_DACDAT_AF);
	GPIO_PinAFConfig(WM8978_MCLK_PORT,   WM8978_MCLK_SOURCE,   WM8978_MCLK_AF);
}

/**
	* @brief  ֹͣI2S����
	* @param  ��
	* @retval ��
	*/
void I2S_Stop(void)
{
	DMA_Cmd(I2Sx_TX_DMA_STREAM,DISABLE);//�ر�DMA,��������
	DMA_Cmd(I2Sxext_RX_DMA_STREAM,DISABLE);//�ر�DMA,��������
}

/*--------------------------   ��Ƶ���Ų���   --------------------------------*/
/**
	* @brief  ����STM32��I2S���蹤��ģʽ
	* @param  _usStandard : �ӿڱ�׼��I2S_Standard_Phillips, I2S_Standard_MSB �� I2S_Standard_LSB
  * @param  _usWordlen : ���ݸ�ʽ��16bit ����24bit
	* @param  _usAudioFreq : ����Ƶ�ʣ�I2S_AudioFreq_8K��I2S_AudioFreq_16K��I2S_AudioFreq_22K��
  *					I2S_AudioFreq_44K��I2S_AudioFreq_48
	* @retval ��
	*/
void I2Sx_Mode_Config(const uint16_t _usStandard,const uint16_t _usWordLen,const uint32_t _usAudioFreq)
{
	I2S_InitTypeDef I2S_InitStructure;
	uint32_t n = 0;
	FlagStatus status = RESET;
/**
	*	For I2S mode, make sure that either:
	*		- I2S PLL is configured using the functions RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S),
	*		RCC_PLLI2SCmd(ENABLE) and RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY).
	*/
	RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
	RCC_PLLI2SCmd(ENABLE);
	for (n = 0; n < 500; n++)
	{
		status = RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY);
		if (status == 1)break;
	}
	/* �� I2S2 APB1 ʱ�� */
	RCC_APB1PeriphClockCmd(WM8978_CLK, ENABLE);

	/* ��λ SPI2 ���赽ȱʡ״̬ */
	SPI_I2S_DeInit(WM8978_I2Sx_SPI);

	/* I2S2 �������� */
	/* ����I2S����ģʽ */
	I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;		
	/* �ӿڱ�׼ */
	I2S_InitStructure.I2S_Standard = _usStandard;			
	/* ���ݸ�ʽ��16bit */
	I2S_InitStructure.I2S_DataFormat = _usWordLen;			
	/* ��ʱ��ģʽ */
	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;	
	/* ��Ƶ����Ƶ�� */
	I2S_InitStructure.I2S_AudioFreq = _usAudioFreq;			
	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
	I2S_Init(WM8978_I2Sx_SPI, &I2S_InitStructure);
	
	/* ʹ�� SPI2/I2S2 ���� */
	I2S_Cmd(WM8978_I2Sx_SPI, ENABLE);
}

/**
	* @brief  I2Sx TX DMA����,����Ϊ˫����ģʽ,������DMA��������ж�
	* @param  buf0:M0AR��ַ.
	* @param  buf1:M1AR��ַ.
	* @param  num:ÿ�δ���������(�������ֽ����һ����������������Ϊ���ݳ���ΪHalfWord)
	* @retval ��
	*/
void I2Sx_TX_DMA_Init(const uint16_t *buffer0,const uint16_t *buffer1,const uint32_t num)
{  
	NVIC_InitTypeDef   NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
 
  RCC_AHB1PeriphClockCmd(I2Sx_DMA_CLK,ENABLE);//DMA1ʱ��ʹ�� 
	
	DMA_DeInit(I2Sx_TX_DMA_STREAM);
	while (DMA_GetCmdStatus(I2Sx_TX_DMA_STREAM) != DISABLE){}//�ȴ�DMA1_Stream4������ 
		
	DMA_ClearITPendingBit(I2Sx_TX_DMA_STREAM,DMA_IT_FEIF4|DMA_IT_DMEIF4|DMA_IT_TEIF4|DMA_IT_HTIF4|DMA_IT_TCIF4);//���DMA1_Stream4�������жϱ�־

  /* ���� DMA Stream */
  DMA_InitStructure.DMA_Channel = I2Sx_TX_DMA_CHANNEL;  //ͨ��0 SPIx_TXͨ�� 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&WM8978_I2Sx_SPI->DR;//�����ַΪ:(u32)&SPI2->DR
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buffer0;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//�洢��������ģʽ
  DMA_InitStructure.DMA_BufferSize = num;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�洢�����ݳ��ȣ�16λ 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ʹ��ѭ��ģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//�����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //��ʹ��FIFOģʽ        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//����ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//�洢��ͻ�����δ���
  DMA_Init(I2Sx_TX_DMA_STREAM, &DMA_InitStructure);//��ʼ��DMA Stream
		
	DMA_DoubleBufferModeConfig(I2Sx_TX_DMA_STREAM,(uint32_t)buffer0,DMA_Memory_0);//˫����ģʽ����
	DMA_DoubleBufferModeConfig(I2Sx_TX_DMA_STREAM,(uint32_t)buffer1,DMA_Memory_1);//˫����ģʽ����
 
  DMA_DoubleBufferModeCmd(I2Sx_TX_DMA_STREAM,ENABLE);//˫����ģʽ����
 
  DMA_ITConfig(I2Sx_TX_DMA_STREAM,DMA_IT_TC,ENABLE);//������������ж�
	
 	SPI_I2S_DMACmd(WM8978_I2Sx_SPI,SPI_I2S_DMAReq_Tx,ENABLE);//SPI2 TX DMA����ʹ��.
	
	NVIC_InitStructure.NVIC_IRQChannel = I2Sx_TX_DMA_STREAM_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
}

/**
	* @brief  SPIx_TX_DMA_STREAM�жϷ�����
	* @param  ��
	* @retval ��
	*/
void I2Sx_TX_DMA_STREAM_IRQFUN(void)
{      
	if(DMA_GetITStatus(I2Sx_TX_DMA_STREAM,I2Sx_TX_DMA_IT_TCIF)==SET)//DMA������ɱ�־
	{ 
		DMA_ClearITPendingBit(I2Sx_TX_DMA_STREAM,I2Sx_TX_DMA_IT_TCIF);//��DMA������ɱ�׼
    I2S_DMA_TX_Callback();	//ִ�лص�����,��ȡ���ݵȲ����������洦��  
	}   											 
} 
/**
	* @brief  I2S��ʼ����
	* @param  ��
	* @retval ��
	*/
void I2S_Play_Start(void)
{   	  
	DMA_Cmd(I2Sx_TX_DMA_STREAM,ENABLE);//����DMA TX����,��ʼ���� 	
}

/**
	* @brief  �ر�I2S����
	* @param  ��
	* @retval ��
	*/
void I2S_Play_Stop(void)
{   	 
	DMA_Cmd(I2Sx_TX_DMA_STREAM,DISABLE);//�ر�DMA TX����,�������� 
}


/*--------------------------   ¼������   --------------------------------*/
/**
	* @brief  ����STM32��I2S���蹤��ģʽ
	* @param  _usStandard : �ӿڱ�׼��I2S_Standard_Phillips, I2S_Standard_MSB �� I2S_Standard_LSB
  * @param  _usWordlen : ���ݸ�ʽ��16bit ����24bit
	* @param  _usAudioFreq : ����Ƶ�ʣ�I2S_AudioFreq_8K��I2S_AudioFreq_16K��I2S_AudioFreq_22K��
  *					I2S_AudioFreq_44K��I2S_AudioFreq_48
	* @retval ��
	*/
void I2Sxext_Mode_Config(const uint16_t _usStandard, const uint16_t _usWordLen,const uint32_t _usAudioFreq)
{
	I2S_InitTypeDef I2Sext_InitStructure;

	/* I2S2 �������� */
	I2Sext_InitStructure.I2S_Mode = I2S_Mode_MasterTx;			/* ����I2S����ģʽ */
	I2Sext_InitStructure.I2S_Standard = _usStandard;			/* �ӿڱ�׼ */
	I2Sext_InitStructure.I2S_DataFormat = _usWordLen;			/* ���ݸ�ʽ��16bit */
	I2Sext_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;	/* ��ʱ��ģʽ */
	I2Sext_InitStructure.I2S_AudioFreq = _usAudioFreq;			/* ��Ƶ����Ƶ�� */
	I2Sext_InitStructure.I2S_CPOL = I2S_CPOL_Low;
	
	I2S_FullDuplexConfig(WM8978_I2Sx_ext, &I2Sext_InitStructure);
	
	/* ʹ�� SPI2/I2S2 ���� */
	I2S_Cmd(WM8978_I2Sx_ext, ENABLE);
}

/**
	* @brief  I2Sxext RX DMA����,����Ϊ˫����ģʽ,������DMA��������ж�
	* @param  buf0:M0AR��ַ.
	* @param  buf1:M1AR��ַ.
	* @param  num:ÿ�δ���������
	* @retval ��
	*/
void I2Sxext_RX_DMA_Init(const uint16_t *buffer0,const uint16_t *buffer1,const uint32_t num)
{  
	NVIC_InitTypeDef   NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;	
 
  RCC_AHB1PeriphClockCmd(I2Sx_DMA_CLK,ENABLE);//DMA1ʱ��ʹ�� 
	
	DMA_DeInit(I2Sxext_RX_DMA_STREAM);
	while (DMA_GetCmdStatus(I2Sxext_RX_DMA_STREAM) != DISABLE){}//�ȴ�DMA1_Stream3������ 
		
	DMA_ClearITPendingBit(I2Sxext_RX_DMA_STREAM,DMA_IT_FEIF3|DMA_IT_DMEIF3|DMA_IT_TEIF3|DMA_IT_HTIF3|DMA_IT_TCIF3);//���DMA1_Stream3�������жϱ�־

  /* ���� DMA Stream */
  DMA_InitStructure.DMA_Channel = I2Sxext_RX_DMA_CHANNEL;  //ͨ��0 SPIx_TXͨ�� 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&WM8978_I2Sx_ext->DR;//�����ַΪ:(u32)&SPI2->DR
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buffer0;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//���赽�洢��ģʽ
  DMA_InitStructure.DMA_BufferSize = num;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�洢�����ݳ��ȣ�16λ 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ʹ��ѭ��ģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //��ʹ��FIFOģʽ        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//����ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//�洢��ͻ�����δ���
  DMA_Init(I2Sxext_RX_DMA_STREAM, &DMA_InitStructure);//��ʼ��DMA Stream
		
	DMA_DoubleBufferModeConfig(I2Sxext_RX_DMA_STREAM,(uint32_t)buffer0,DMA_Memory_0);//˫����ģʽ����
	DMA_DoubleBufferModeConfig(I2Sxext_RX_DMA_STREAM,(uint32_t)buffer1,DMA_Memory_1);//˫����ģʽ����
 
  DMA_DoubleBufferModeCmd(I2Sxext_RX_DMA_STREAM,ENABLE);//˫����ģʽ����
 
  DMA_ITConfig(I2Sxext_RX_DMA_STREAM,DMA_IT_TC,ENABLE);//������������ж�
	
 	SPI_I2S_DMACmd(WM8978_I2Sx_ext,SPI_I2S_DMAReq_Rx,ENABLE);//SPI2 RX DMA����ʹ��.
	
	NVIC_InitStructure.NVIC_IRQChannel = I2Sxext_RX_DMA_STREAM_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
}


/**
	* @brief  I2Sxext_RX_DMA_STREAM�жϷ�����
	* @param  ��
	* @retval ��
	*/
void I2Sxext_RX_DMA_STREAM_IRQFUN(void)
{      
	if(DMA_GetITStatus(I2Sxext_RX_DMA_STREAM,I2Sxext_RX_DMA_IT_TCIF)==SET)////DMA1_Stream3,������ɱ�־
	{ 
		DMA_ClearITPendingBit(I2Sxext_RX_DMA_STREAM,I2Sxext_RX_DMA_IT_TCIF);
    I2S_DMA_RX_Callback();	//ִ�лص�����,��ȡ���ݵȲ����������洦��  		
	}   											 
}

/**
	* @brief  I2S��ʼ¼��
	* @param  ��
	* @retval ��
	*/
void I2Sxext_Recorde_Start(void)
{   	  
	DMA_Cmd(I2Sxext_RX_DMA_STREAM,ENABLE);//����DMA RX����,��ʼ¼��
}

/**
	* @brief  �ر�I2S¼��
	* @param  ��
	* @retval ��
	*/
void I2Sxext_Recorde_Stop(void)
{   	 
	DMA_Cmd(I2Sxext_RX_DMA_STREAM,DISABLE);//�ر�DMA RX����,����¼��
}

/***************************** (END OF FILE) *********************************/
