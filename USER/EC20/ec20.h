#ifndef _EC20_H_
#define _EC20_H_
#include "stm32f4xx.h"
#include "Graph1/UCGUI_SAMPLE.h"
//#include "BSP/key/bsp_key.h" 

#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8	//�ߵ��ֽڽ����궨��
#define Message_BOX_COUNT_INDEX_SIZE 50
#define TEMP_Message_Storage_from_size 100
#define TEMP_Message_Storage_text_size 400




typedef struct {
uint8_t sign:1;//����������Ӫ�̱�ʶ�����Ƿ���Ч��1��Ч
 int Act;//�������ͱ�ʶ���� 
 char COPS[16] ;//�洢���͹�������Ӫ�̱�ʶ
}Operation_Bus;

struct smsindexlist { 
struct smsindexlist *prev; 
struct smsindexlist *next; 
u16 index; 
};

typedef struct {
//char mem1[2] ;	 //�洢��1������
int used1;		//mem1��ʹ�õĿռ䣬�յ�����Ϣ����
int total1;		//mem1�ܵĿռ�
//char mem2[2] ;	//�洢��2������
int used2;		//mem2��ʹ�õĿռ䣬�յ�����Ϣ����
int total2;		//mem2�ܵĿռ�
//char mem3[2] ;	//�洢��1������
int used3;		//mem3��ʹ�õĿռ䣬�յ�����Ϣ����
int total3;		//mem3�ܵĿռ�
}Preferred_Message_Storage;//����Ϣ������Ϣ������1

typedef struct {
//uint8_t new_SMsms_count;
//uint8_t new_MEsms_count;
uint8_t SELETED_used;//��ǰѡ��Ĵ洢 ��ʹ�õĿռ䣬�յ�����Ϣ����
uint8_t SELETED_total;//��ǰѡ��Ĵ洢 �ܵĿռ�
uint8_t SM_used;//SM ��ʹ�õĿռ䣬�յ�����Ϣ����
uint8_t SM_total;//SM �ܵĿռ�
uint8_t ME_used;//ME ��ʹ�õĿռ䣬�յ�����Ϣ����
uint8_t ME_total;//ME �ܵĿռ�
uint8_t INDEX[Message_BOX_COUNT_INDEX_SIZE];//����Ϣ��������
}Message_BOX_COUNT;//����Ϣ���������2 ��ȫ�ֱ���ʹ��


typedef struct {
	uint8_t alive;//��Ϣ�Ƿ���Ч
	uint8_t mem:2;//��ǰ��Ϣ������������men:1 from SM  2:form ME
	uint16_t index;//��ǰ��Ϣ������
	char stat[12];//��ǰ��Ϣ��״̬��REC�Ѷ���δ��...
	char retime[22];//��Ϣ����ʱ��
	char from[TEMP_Message_Storage_from_size];//����
	char text[TEMP_Message_Storage_text_size] ;//��Ϣ����
}TEMP_Message_Storage;//��ʱ�洢һ����Ϣ������ʾʹ��
//enum EC20_NET_STATUS{
//	EC20_OFF=1,
//	EC20_PW_CHECK=2,
//	EC20_AT_CHECK=4,
//	EC20_IPR=8,
//	EC20_ECHO_OFF=16,
//	EC20_IMEI=32,
//	EC20_READY=64,
//	EC20_CSQ=128,
//	EC20_CREG=256,
//	EC20_CEREG=512,
//	EC20_SET_APN=1024,
//	EC20_ACT_SET=2048,
//	EC20_ACT_CHECK=4096,
//	EC20_OK=8192,
//	EC20_ON=16384
//};



typedef struct {
//unsigned int EC20_OFF : 1;
unsigned int EC20_ATOK : 1;
unsigned int EC20_ATV1_SET : 1;
unsigned int EC20_INIT : 1;
unsigned int EC20_ATE1 : 1;
unsigned int EC20_CMEE : 1;	
unsigned int Net_Reg_INIT : 1;
unsigned int EC20_IMEI : 1;
unsigned int EC20_SIM_READY : 1;
unsigned int EC20_CSQ : 1;
unsigned int EC20_CREG : 1;
unsigned int EC20_CEREG : 1;
unsigned int EC20_SET_APN : 1;
unsigned int EC20_ACT_SET : 1;
unsigned int EC20_ACT_CHECK : 1;
unsigned int EC20_AIR_MODE: 1;
unsigned int IS_EC20_OFF : 1;
unsigned int EC20_CMGF : 2;
unsigned int EC20_CPMS :2;
unsigned int EC20_Delete_SMS : 1;
unsigned int EC20_SMSBOX_INDEX : 1;
unsigned int EC20_CMGS : 1;//SENT SMS
unsigned int EC20_NTP : 1;//
unsigned int EC20_IS_TURNNING_ON : 1;//
unsigned int EC20_IS_TURNNING_OFF : 1;//
}NET_FLAGS;



u8 EC20_INIT(void);
u8 EC20_Net_Reg_INIT(void);
u8 EC20_ATOK(void);
u8 EC20_CPIN_Check(void);
u8 EC20_GSN_Check(void);
u8 EC20_CMEE_Check(void);
u8 EC20_ATV1_Check(void);
u8 EC20_Get_Csq(void);
u8 EC20_Get_CREG(void);
u8 EC20_Get_CGREG(void);
u8 EC20_Get_COPS(void);
u8 EC20_SET_CMGF(u8 mode);
//u8 EC20_GET_CPMS(void);
u8 EC20_SET_CPMS(u8 mode);
u8 EC20_Power_Off(void);
u8 EC20_Power_on(void);
u8 EC20_ARIMODE_CHECK(void);
u8 EC20_AIR_Mode_On(void);
u8 EC20_AIR_Mode_Off(void);
u8 EC20_Read1SMS(u8 mem ,u16 index,uint8_t tmp_sms_idx);
u8 EC20_Delete_SMS(u8 mem ,u8 delflag,u16 index);
u8 EC20_UPDATA_SMSBOX_INDEX(u8 mem );
u16 pick_up_index(const char *str, u8 *num);
u8 EC20_SENT_SMS(const char* des,const char* text);
u8 EC20_SyncLocalTime(u8 updatalocaltime);
u8 EC20_SET_CSCS(u8 mode);
u8 EC20_ECHO_ON(void);
void EC20PWFoot_Config(void);
void EXTI_RI_FOOT_PIN_Config(void);
//u8 EC20_Pro(void);
//u8 Net_Data_Cheak_Pro(void);
//u8 EC20_Http_Set_Url(char *url,uint16_t len);
//u8 EC20_Set_Post_Buf(char *sendstr,uint16_t len);
//u8 EC20_Clr_Post_Buf(void);
//u8 Http_Post_Data(void);
//u8 Get_Net_Status(void);
//u8 Http_Post_Data(char *buf,u16 len);

#define EC20PWFoot_INT_GPIO_PORT                GPIOC
#define EC20PWFoot_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define EC20PWFoot_INT_GPIO_PIN                 GPIO_Pin_13
//#define EC20PWFoot_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOC
//#define EC20PWFoot_INT_EXTI_PINSOURCE           EXTI_PinSource13
//#define EC20PWFoot_INT_EXTI_LINE                EXTI_Line13
//#define EC20PWFoot_INT_EXTI_IRQ                 EXTI15_10_IRQn

//#define EC20PWFoot_IRQHandler                   EXTI15_10_IRQHandler

#define	EC20_PW_ON 		digitalLo(EC20PWFoot_INT_GPIO_PORT,EC20PWFoot_INT_GPIO_PIN) //ec20 ��
#define	EC20_PW_OFF 	digitalHi(EC20PWFoot_INT_GPIO_PORT,EC20PWFoot_INT_GPIO_PIN)	 //ec20 ��
#define	EC20_PW_GOIP_CHECK 	GPIO_ReadInputDataBit(EC20PWFoot_INT_GPIO_PORT,EC20PWFoot_INT_GPIO_PIN) //ec20 ��



//#define EC20RIFOOT_INT_GPIO_PORT                GPIOB
//#define EC20RIFOOT_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOB
//#define EC20RIFOOT_INT_GPIO_PIN                 GPIO_Pin_10
//#define EC20RIFOOT_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOB
//#define EC20RIFOOT_INT_EXTI_PINSOURCE           EXTI_PinSource10
//#define EC20RIFOOT_INT_EXTI_LINE              	EXTI_Line10
//#define EC20RIFOOT_INT_EXTI_IRQ                 EXTI15_10_IRQn

//#define EC20RIFOOT_IRQHandler                   EXTI15_10_IRQHandler

#endif
