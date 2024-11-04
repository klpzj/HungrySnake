/****************************************************************
 * �ļ���: main.h
 * ����: ̰������Ϸ����Ҫͷ�ļ���������������ͺ�������
 ****************************************************************/

#ifndef __MAIN_H__ 
#define __MAIN_H__ 

#ifdef __cplusplus 
extern "C" { 
#endif 

/* ����ͷ�ļ� */
#include <reg51.h> 

/* �������� */
#define STARTPOSITION      0x07    // �ߵ���ʼλ��
#define PRESTARTPOSITION   0xf7    // �ߵ�Ԥ��ʼλ��
#define TIMER0MILISECOND  200     // ��ʱ��0�ĺ�����
#define TIMER0COUNTER     (TIMER0MILISECOND / 50)  // ��ʱ������ֵ

/* ��ʱ����ֵ */
#define TH0_N             0x3c    // ��ʱ��0��λ��ֵ
#define TL0_N             0xb0    // ��ʱ��0��λ��ֵ

/* IO�ڶ��� */
sbit L0 = P1^0;    // ��������0
sbit L1 = P1^1;    // ��������1
sbit L2 = P1^2;    // ��������2
sbit L3 = P1^3;    // ��������3
sbit SER = P3^4;   // 595������������
sbit RCLK = P3^5;  // 595�洢�Ĵ���ʱ��
sbit SRCLK = P3^6; // 595��λ�Ĵ���ʱ��
sbit BUZ = P3^7;   // ���������

/* ���Ͷ��� */
typedef unsigned char UINT8; 
typedef unsigned int  UINT16; 	

/* ���нṹ�嶨�� */
typedef struct {
    UINT8 *Head;     // ����ͷָ��
    UINT8 *Tail;     // ����βָ��
    UINT8 HeadBit;   // ͷ��λ�ñ��
    UINT8 TailBit;   // β��λ�ñ��
    UINT8 Buff[16];  // ���л�����
} Queue;

/* �������� */
// ��ʼ������
void Init(void);                    // ϵͳ��ʼ��
void InitGame(void);                // ��Ϸ��ʼ��
void InitScoreImage(void);          // ������ʾ��ʼ��
void InitMusic(UINT8 pointer);      // ���ֳ�ʼ��

// ��ʱ����
void Delay10Us(UINT16 ten_us);      // 10΢����ʱ

// ����������
void KeyCatch(void);                // ��Ϸ�а�������
void KeyCatchMid(void);             // �м�״̬��������

// ��ʾ��غ���
void hc595_write_data(UINT8 dat1, UINT8 dat2, UINT8 dat3, UINT8 dat4); // 595����д��
void Display(void);                 // ��ʾˢ��
void DrawScore(void);               // ���Ʒ���
void DrawScoreImage(void);          // ���Ʒ�������
void DrawStartImage(void);          // ���ƿ�ʼ����

// ��Ϸ�߼�����
void Step(void);                    // �ƶ�һ��
void SetFood(void);                 // ����ʳ��
void ClearGround(void);             // �����Ϸ����

// λ�ü��㺯��
UINT8 GetNextPosition(UINT8 position, UINT8 direction);  // ��ȡ��һλ��
UINT8 GetPosition(UINT8 x, UINT8 y);                    // ��ȡλ�ñ���
UINT8 GetX(UINT8 position);                             // ��ȡX����
UINT8 GetY(UINT8 position);                             // ��ȡY����
UINT8 GetIsCollide(void);                               // �����ײ
UINT8 Rand(void);                                       // ���������

// �жϴ�����
void Timer0Routine(void);           // ��ʱ��0�ж�
void Timer1Routine(void);           // ��ʱ��1�ж�

// ״̬��ȡ����
UINT8 GetIsDrawn(void);
UINT8 GetIsPause(void);
UINT8 GetIsStep(void);
UINT8 GetIsLengthen(void);
UINT8 GetMusicPointer(void);
UINT8 GetHeadDirectionBit(void);
UINT8 GetBeatCount(void);
UINT8 GetMusicBeat(void);
UINT8 GetIsMusic(void);
UINT8 GetTimer0Counter(void);

// ״̬���ú���
void SetIsDrawn(UINT8 tmp);
void SetIsPause(UINT8 tmp);
void SetIsStep(UINT8 tmp);
void SetIsLengthen(UINT8 tmp);
void SetMusicPointer(UINT8 tmp);
void SetHeadDirectionBit(UINT8 tmp);
void SetBeatCount(UINT8 tmp);
void SetMusicBeat(UINT8 tmp);
void SetIsMusic(UINT8 tmp);
void SetTimer0Counter(UINT8 tmp);

#ifdef __cplusplus 
} 
#endif 
#endif