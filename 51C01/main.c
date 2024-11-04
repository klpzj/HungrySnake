/****************************************************************
 * �ļ���: main.c
 * ����: ̰������Ϸ������ʵ��
 ****************************************************************/

#include "main.h" 
#include "Queue.h"

/* �������ݶ��� */
// LED������ɨ�����ݱ�
UINT8 code LED_ROW[32] = {
    0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,  // �ϰ벿����ѡ��
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // �м����
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // �м����
    0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80   // �°벿����ѡ��
};

// ��ʼ����ͼ������
UINT8 code START_IMG[32] = {
    0xFF,0xFF,0x7F,0xC0,0xBF,0x9F,0xDF,0xB7,
    0xDF,0x9F,0xDF,0xC0,0xDF,0xFF,0x0F,0xE0,
    0xE7,0xCF,0xF7,0xDF,0xE7,0xCF,0x83,0x83,
    0xFB,0xBF,0xF3,0x9F,0x05,0xC0,0xFF,0xFF
};

// ������ʾ����ͼ������
UINT8 code SCORE_IMG[32] = {
    0x00,0x00,0x00,0x20,0x04,0x40,0x0A,0x00,
    0xC2,0x0C,0x24,0x12,0x28,0x12,0xC6,0x4C,
    0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,
    0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00
};

// ����ת����
UINT8 code directionTable[4][2] = {
    {1,3},  // ��
    {2,0},  // ��
    {3,1},  // ��
    {0,2}   // ��
};

// �ƶ�������
UINT8 code stepTable[4][2] = {
    {2,1},  // ����
    {1,2},  // ����
    {0,1},  // ����
    {1,0}   // ����
};

// ������ʾ��������
UINT8 code scoreTable[10][5] = {
    {0x07,0x05,0x05,0x05,0x07},  // 0
    {0x04,0x04,0x04,0x04,0x04},  // 1
    {0x07,0x04,0x07,0x01,0x07},  // 2
    {0x07,0x04,0x07,0x04,0x07},  // 3
    {0x05,0x05,0x07,0x04,0x04},  // 4
    {0x07,0x01,0x07,0x04,0x07},  // 5
    {0x07,0x01,0x07,0x05,0x07},  // 6
    {0x07,0x04,0x04,0x04,0x04},  // 7
    {0x07,0x05,0x07,0x05,0x07},  // 8
    {0x07,0x05,0x07,0x04,0x07}   // 9
};

// ���ֲ��Ŷ�ʱ�����ݱ�
UINT8 code th1Table[2][21] = {
    {0xF8,0xF9,0xFA,0xFA,0xFB,0xfa,0xfa,0xFC,0xFC,0xFD,0xFD,0xFD,0xFD,
     0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF},
    {0x8B,0x5B,0x14,0x67,0x04,0x67,0x67,0x43,0xAC,0x09,0x33,0x82,0xC7,
     0x05,0x21,0x56,0x84,0x9A,0xC1,0xE3,0x02}
};

// �������ݱ�
UINT8 code musicTable[4][4] = {
    {101,103,105,137},  // ��ʼ����
    {111,113,225,225},  // �Ե�ʳ����Ч
    {101,101,137,137},  // ��Ϸ��ʼ��Ч
    {107,105,103,131}   // ��Ϸ������Ч
};

/* ȫ�ֱ������� */
Queue Snake;                    // �������
UINT8 foodPosition;            // ʳ��λ��
UINT8 snakeTailPosition;       // ��βλ��
UINT8 snakeTailDirection;      // ��β����
UINT8 snakeHeadPosition;       // ��ͷλ��
UINT8 snakeHeadDirection;      // ��ͷ����
UINT8 ground[32];             // ��Ϸ��������
UINT8 psw0;                   // ״̬��0
UINT8 psw1;                   // ״̬��1
UINT8 score;                  // ����
UINT8 th1, tl1;              // ��ʱ��1��ֵ

/*
״̬��˵��:
psw0:(Low �� High)
headDirectionBit 2bits - ͷ������λ
musicPointer     2bit  - ����ָ��
isLengthen      1bit  - �Ƿ�����
isStep          1bit  - �Ƿ��ƶ�
isPause         1bit  - �Ƿ���ͣ
isDrawn         1bit  - �Ƿ��ѻ���

psw1:(Low �� High)
timer0Counter   3bit  - ��ʱ��0����
isMusic        1bit  - �Ƿ񲥷�����
musicBeat      2bit  - ���ֽ���
beatCount      2bit  - ���ļ���

�������:
00 - ��
01 - ��
10 - ��
11 - ��

����λ����:
10 - ��ת
11 - ��ת
00 - ��ת��
01 - ����
*/

/****************************************************************
 * �������ܺ���ʵ��
 ****************************************************************/

/****************************************************************
 * ������: Init
 * ����: ϵͳ��ʼ��
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void Init(void)
{
    /* ��ʼ����ʱ�������� */
    SetTimer0Counter(TIMER0COUNTER);
    
    /* ���ö�ʱ�� */
    TMOD = 0x11;      // ���ö�ʱ��0��1Ϊģʽ1
    TH0 = TH0_N;      // ���ö�ʱ��0��ֵ
    TL0 = TL0_N;
    IE = 0x8a;        // ʹ�ܶ�ʱ��0��1�ж�
    TR0 = 1;          // ������ʱ��0
    
    /* ��ʾ��ʼ���� */
    DrawStartImage();
    SetIsPause(1);
}

/****************************************************************
 * ������: InitGame
 * ����: ��Ϸ��ʼ��
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void InitGame(void)
{
    UINT8 vector;
    
    /* �����Ϸ���� */
    ClearGround();
    
    /* ��ʼ��״̬ */
    SetHeadDirectionBit(0x00);
    SetIsLengthen(0);
    SetIsStep(0);
    SetIsPause(0);
    SetIsDrawn(0);
    score = 0;
    
    /* ��ʼ���߶��� */
    QueueEmpty(&Snake.Head, &Snake.Tail, Snake.Buff, &Snake.HeadBit, &Snake.TailBit);
    
    /* �����ߵĳ�ʼλ�úͷ��� */
    snakeTailPosition = PRESTARTPOSITION;
    snakeTailDirection = 0x00;
    snakeHeadPosition = STARTPOSITION;
    snakeHeadDirection = 0x00;
    ground[14] = 0x01;
    
    /* ����ʳ��λ�� */
    SetFood();
    
    /* ��ʼ��������� */
    vector = GetHeadDirectionBit();
    DataIn(&Snake.Head, Snake.Buff, &Snake.HeadBit, sizeof(Snake.Buff), &vector);
}

/****************************************************************
 * ������: InitScoreImage
 * ����: ��ʼ��������ʾ����
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void InitScoreImage(void)
{
    DrawScoreImage();
    DrawScore();
    SetIsDrawn(1);
}

/****************************************************************
 * ������: InitMusic
 * ����: ��ʼ�����ֲ���
 * ����: 
 *   pointer: �������
 * ����ֵ: ��
 ****************************************************************/
void InitMusic(UINT8 pointer)
{
    TR1 = 0;              // ֹͣ��ʱ��1
    SetIsMusic(1);        // ��������
    SetMusicBeat(0);      // ���ý���
    SetBeatCount(0);      // ���ý��ļ���
    SetMusicPointer(pointer); // �����������
}

/****************************************************************
 * ������: Delay10Us
 * ����: ��ʱ����
 * ����: 
 *   ten_us: ��ʱʱ��(10΢��ı���)
 * ����ֵ: ��
 ****************************************************************/
void Delay10Us(UINT16 ten_us)
{
    while(ten_us--);
}

/****************************************************************
 * λ�ü�����غ���
 ****************************************************************/

/****************************************************************
 * ������: GetX
 * ����: ��ȡλ�õ�X����
 * ����: 
 *   position: λ�ñ���
 * ����ֵ: X����ֵ(0-15)
 ****************************************************************/
UINT8 GetX(UINT8 position)
{
    return position >> 4;
}

/****************************************************************
 * ������: GetY
 * ����: ��ȡλ�õ�Y����
 * ����: 
 *   position: λ�ñ���
 * ����ֵ: Y����ֵ(0-15)
 ****************************************************************/
UINT8 GetY(UINT8 position)
{
    return position & 0x0f;
}

/****************************************************************
 * ������: GetPosition
 * ����: ���������ȡλ�ñ���
 * ����: 
 *   x: X����(0-15)
 *   y: Y����(0-15)
 * ����ֵ: λ�ñ���
 ****************************************************************/
UINT8 GetPosition(UINT8 x, UINT8 y)
{
    return (x << 4) + y;
}

/****************************************************************
 * ������: GetNextPosition
 * ����: ������һ��λ��
 * ����: 
 *   position: ��ǰλ��
 *   direction: �ƶ�����
 * ����ֵ: ��һ��λ�õı���
 ****************************************************************/
UINT8 GetNextPosition(UINT8 position, UINT8 direction)
{
    UINT8 x = GetX(position);
    UINT8 y = GetY(position);
    
    /* �����µ�X���� */
    x = x + stepTable[direction][0] - 1;
    if(x == 0x10) x = 0x00;
    if(x == 0xff) x = 0x0f;
    
    /* �����µ�Y���� */
    y = y + stepTable[direction][1] - 1;
    if(y == 0x10) y = 0x00;
    if(y == 0xff) y = 0x0f;
    
    return GetPosition(x, y);
}

/****************************************************************
 * ״̬��������ʵ��
 * ˵������Щ��������psw0��psw1����״̬�ֵĲ�ͬλ
 ****************************************************************/

/* ״̬��ȡ���� */
UINT8 GetIsDrawn(void)          { return ((psw0 >> 7) & 0x01); }
UINT8 GetIsPause(void)          { return ((psw0 >> 6) & 0x01); }
UINT8 GetIsStep(void)           { return ((psw0 >> 5) & 0x01); }
UINT8 GetIsLengthen(void)       { return ((psw0 >> 4) & 0x01); }
UINT8 GetMusicPointer(void)     { return ((psw0 >> 2) & 0x03); }
UINT8 GetHeadDirectionBit(void) { return (psw0 & 0x03); }
UINT8 GetBeatCount(void)        { return ((psw1 >> 6) & 0x03); }
UINT8 GetMusicBeat(void)        { return ((psw1 >> 4) & 0x03); }
UINT8 GetIsMusic(void)          { return ((psw1 >> 3) & 0x01); }
UINT8 GetTimer0Counter(void)    { return (psw1 & 0x07); }

/* ״̬���ú��� */
void SetIsDrawn(UINT8 tmp)          { psw0 &= (~(0x01 << 7)); psw0 |= (tmp << 7); }
void SetIsPause(UINT8 tmp)          { psw0 &= (~(0x01 << 6)); psw0 |= (tmp << 6); }
void SetIsStep(UINT8 tmp)           { psw0 &= (~(0x01 << 5)); psw0 |= (tmp << 5); }
void SetIsLengthen(UINT8 tmp)       { psw0 &= (~(0x01 << 4)); psw0 |= (tmp << 4); }
void SetMusicPointer(UINT8 tmp)     { psw0 &= (~(0x03 << 2)); psw0 |= (tmp << 2); }
void SetHeadDirectionBit(UINT8 tmp) { psw0 &= (~(0x03)); psw0 |= (tmp); }
void SetBeatCount(UINT8 tmp)        { psw1 &= (~(0x03 << 6)); psw1 |= (tmp << 6); }
void SetMusicBeat(UINT8 tmp)        { psw1 &= (~(0x03 << 4)); psw1 |= (tmp << 4); }
void SetIsMusic(UINT8 tmp)          { psw1 &= (~(0x01 << 3)); psw1 |= (tmp << 3); }
void SetTimer0Counter(UINT8 tmp)    { psw1 &= (~(0x07)); psw1 |= (tmp); }

/****************************************************************
 * ��Ϸ�߼���غ���
 ****************************************************************/

/****************************************************************
 * ������: GetIsCollide
 * ����: ����Ƿ�����ײ
 * ����: ��
 * ����ֵ: 
 *   1: ������ײ
 *   0: δ������ײ
 ****************************************************************/
UINT8 GetIsCollide(void)
{
    UINT8 x, y, tmp, headDirectionBit, nextPosition;
    
    /* ��ȡ��һ��λ�� */
    headDirectionBit = GetHeadDirectionBit();
    if((headDirectionBit >> 1) == 1)
        nextPosition = GetNextPosition(snakeHeadPosition, 
            directionTable[snakeHeadDirection][headDirectionBit & 0x01]);
    else
        nextPosition = GetNextPosition(snakeHeadPosition, snakeHeadDirection);
    
    x = GetX(nextPosition);
    y = GetY(nextPosition);
    
    /* ����Ƿ�Ե�ʳ�� */
    if(nextPosition == foodPosition)
    {
        InitMusic(1);    // ���ų�ʳ����Ч
        SetIsLengthen(1);
        return 0;
    }
    
    /* ����Ƿ��������� */
    if(x <= 7)
    {
        tmp = 0x01 << x;
        return ((UINT8)((ground[2 * y] & tmp) >> x) == 1);
    }
    if(x >= 8)
    {
        tmp = 0x01 << (x - 8);
        return ((UINT8)((ground[2 * y + 1] & tmp) >> (x - 8)) == 1);
    }
    return 0;
}

/****************************************************************
 * ������: Rand
 * ����: ���������
 * ����: ��
 * ����ֵ: �����ֵ
 ****************************************************************/
UINT8 Rand(void)
{
    UINT16 a = 1103515245;
    UINT16 c = 12345;
    UINT16 m = 65535;    // 2^16������16λ����
    
    return (UINT8)((a * (TH0 + psw0 + psw1 + snakeTailPosition + 
           snakeTailDirection + snakeHeadPosition + snakeHeadDirection) + c) % m);
}

/****************************************************************
 * ������: Step
 * ����: ִ���ߵ��ƶ�
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void Step(void)
{
    UINT8 x, y, tmp, tailDirectionBit, headDirectionBit;
    
    /* ������ͷ�����λ�� */
    headDirectionBit = GetHeadDirectionBit();
    DataIn(&Snake.Head, Snake.Buff, &Snake.HeadBit, sizeof(Snake.Buff), &headDirectionBit);
    
    if((headDirectionBit >> 1) == 1)
        snakeHeadDirection = directionTable[snakeHeadDirection][headDirectionBit & 0x01];
    
    snakeHeadPosition = GetNextPosition(snakeHeadPosition, snakeHeadDirection);
    
    /* ����Ϸ�����ϻ�������ͷ */
    x = GetX(snakeHeadPosition);
    y = GetY(snakeHeadPosition);
    if(x <= 7)
    {
        tmp = 0x01 << x;
        ground[2 * y] |= tmp;
    }
    if(x >= 8)
    {
        tmp = 0x01 << (x - 8);
        ground[2 * y + 1] |= tmp;
    }
    
    /* �������Ҫ��������ɾ����β */
    if(GetIsLengthen() == 0)
    {
        DataOut(&Snake.Head, &Snake.Tail, Snake.Buff, &Snake.HeadBit, 
                &Snake.TailBit, sizeof(Snake.Buff), &tailDirectionBit);
        
        if((tailDirectionBit >> 1) == 1)
            snakeTailDirection = directionTable[snakeTailDirection][tailDirectionBit & 0x01];
        
        snakeTailPosition = GetNextPosition(snakeTailPosition, snakeTailDirection);
        
        /* ����Ϸ������ɾ������β */
        x = GetX(snakeTailPosition);
        y = GetY(snakeTailPosition);
        if(x <= 7)
        {
            tmp = ~(0x01 << x);
            ground[2 * y] &= tmp;
        }
        if(x >= 8)
        {
            tmp = ~(0x01 << (x - 8));
            ground[2 * y + 1] &= tmp;
        }
    }
}

/****************************************************************
 * ������: SetFood
 * ����: ����Ϸ�������������ʳ��
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void SetFood(void)
{
    UINT8 x, y, tmp, sign = 1;
    
    /* ѭ��ֱ���ҵ���λ�� */
    while(sign)
    {
        foodPosition = Rand();
        x = GetX(foodPosition);
        y = GetY(foodPosition);
        
        if(x <= 7)
        {
            tmp = 0x01 << x;
            if((ground[2 * y] & tmp) == 0)
            {
                ground[2 * y] |= tmp;
                sign = 0;
            }
        }
        if(x >= 8)
        {
            tmp = 0x01 << (x - 8);
            if((ground[2 * y + 1] & tmp) == 0)
            {
                ground[2 * y + 1] |= tmp;
                sign = 0;
            }
        }
    }
}

/****************************************************************
 * ��ʾ��غ���
 ****************************************************************/

/****************************************************************
 * ������: Display
 * ����: ˢ��LED������ʾ
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void Display(void)
{
    UINT8 i;
    for(i = 0; i < 16; i++)
    {
        /* ��������ѡ���� */
        hc595_write_data(LED_ROW[i], LED_ROW[i+16], 
                        ~ground[i * 2], ~ground[i * 2 + 1]);
        
        /* ��Ӱ */
        hc595_write_data(0x00, 0x00, 0x00, 0x00);
    }           
}

/****************************************************************
 * ������: ClearGround
 * ����: �����Ϸ����
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void ClearGround(void)
{
    UINT8 i;
    for(i = 0; i < 32; i++)
    {
        ground[i] = 0x00;
    }
}

/****************************************************************
 * ������: DrawScore
 * ����: ���Ʒ���
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void DrawScore(void)
{
    UINT8 vector, i;
    
    /* ���ư�λ���� */
    vector = score / 100;
    for(i = 0; i < 5; i++)
    {
        ground[20 + 2 * i] |= scoreTable[vector][i] << 4;
    }
    
    /* ����ʮλ���� */
    vector = (score % 100) / 10;
    for(i = 0; i < 5; i++)
    {
        ground[21 + 2 * i] |= scoreTable[vector][i];
    }
    
    /* ���Ƹ�λ���� */
    vector = score % 10;
    for(i = 0; i < 5; i++)
    {
        ground[21 + 2 * i] |= scoreTable[vector][i] << 4;
    }
}

/****************************************************************
 * ������: DrawScoreImage
 * ����: ���Ʒ�������
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void DrawScoreImage(void)
{
    UINT8 i;
    for(i = 0; i < 32; i++)
    {
        ground[i] = SCORE_IMG[i];
    }
}

/****************************************************************
 * ������: DrawStartImage
 * ����: ���ƿ�ʼ����
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void DrawStartImage(void)
{
    UINT8 i;
    for(i = 0; i < 32; i++)
    {
        ground[i] = START_IMG[i];
    }
}

/****************************************************************
 * ������: hc595_write_data
 * ����: ��74HC595д����ʾ����
 * ����: 
 *   dat1,dat2: ��ѡ������
 *   dat3,dat4: ����ʾ����
 * ����ֵ: ��
 ****************************************************************/
void hc595_write_data(UINT8 dat1, UINT8 dat2, UINT8 dat3, UINT8 dat4)
{
    UINT8 i;
    
    /* д���4������ */
    for(i = 0; i < 8; i++)
    {
        SER = dat4 >> 7;    // ���ȴ����λ
        dat4 <<= 1;
        SRCLK = 0;
        SRCLK = 1;          // �����ؽ���������Ĵ���
    }
    
    /* д���3������ */
    for(i = 0; i < 8; i++)
    {
        SER = dat3 >> 7;
        dat3 <<= 1;
        SRCLK = 0;
        SRCLK = 1;
    }
    
    /* д���2������ */
    for(i = 0; i < 8; i++)
    {
        SER = dat2 >> 7;
        dat2 <<= 1;
        SRCLK = 0;
        SRCLK = 1;
    }
    
    /* д���1������ */
    for(i = 0; i < 8; i++)
    {
        SER = dat1 >> 7;
        dat1 <<= 1;
        SRCLK = 0;
        SRCLK = 1;
    }
    
    /* ������� */
    RCLK = 1;
    Delay10Us(1);
    RCLK = 0;
}

/****************************************************************
 * ����������
 ****************************************************************/

/****************************************************************
 * ������: KeyCatch
 * ����: ��Ϸ�еİ�������
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void KeyCatch(void)
{
    /* ����ϼ� */
    P1 = 0xef;
    if (L1 == 0)
    {
        if(snakeHeadDirection == 0x02) SetHeadDirectionBit(0x02); 
        if(snakeHeadDirection == 0x00) SetHeadDirectionBit(0x03); 
    }
    Delay10Us(1);
    
    /* ������Ҽ� */
    P1 = 0xdf;
    if (L0 == 0)
    {
        if(snakeHeadDirection == 0x01) SetHeadDirectionBit(0x02); 
        if(snakeHeadDirection == 0x03) SetHeadDirectionBit(0x03); 
    }
    if (L2 == 0)
    {
        if(snakeHeadDirection == 0x03) SetHeadDirectionBit(0x02); 
        if(snakeHeadDirection == 0x01) SetHeadDirectionBit(0x03); 
    }
    Delay10Us(1);
    
    /* ����¼� */
    P1 = 0xbf;
    if (L1 == 0)
    {
        if(snakeHeadDirection == 0x00) SetHeadDirectionBit(0x02); 
        if(snakeHeadDirection == 0x02) SetHeadDirectionBit(0x03); 
    }
    Delay10Us(1);
}

/****************************************************************
 * ������: KeyCatchMid
 * ����: ��Ϸ��ʼ/����ʱ�İ�������
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void KeyCatchMid(void)
{
    P1 = 0xdf;
    if (L1 == 0)
    {
        SetIsPause(0);
        InitMusic(2);
        InitGame();
    } 
}

/****************************************************************
 * �жϴ�����
 ****************************************************************/

/****************************************************************
 * ������: Timer0Routine
 * ����: ��ʱ��0�жϴ���
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void Timer0Routine(void) interrupt 1
{
    UINT8 soundCode;
    
    /* ���¶�ʱ������ */
    SetTimer0Counter(GetTimer0Counter() - 1);
    TH0 = TH0_N;
    TL0 = TL0_N;
    
    /* ����Ƿ���Ҫ�ƶ��� */
    if(GetTimer0Counter() == 0)
    {
        SetTimer0Counter(TIMER0COUNTER);
        SetIsStep(1);
    }
    
    /* �������ֲ��� */
    if(GetIsMusic() == 1)
    {
        soundCode = musicTable[GetMusicPointer()][GetMusicBeat()];
        
        if(GetBeatCount() == 0)
        {
            TR1 = 0;
            th1 = th1Table[0][(soundCode % 10) + (7 * (soundCode / 100)) - 1];
            tl1 = th1Table[1][(soundCode % 10) + (7 * (soundCode / 100)) - 1];
            TH1 = th1;
            TL1 = tl1;
            TR1 = 1;
        }

        if(GetBeatCount() < ((soundCode / 10) % 10))
        {
            SetBeatCount(GetBeatCount() + 1);
        }
        else 
        {
            SetBeatCount(0);
            if(GetMusicBeat() < 3)
            {
                SetMusicBeat(GetMusicBeat() + 1); 
            }
            else
            {
                SetIsMusic(0); // ���ֲ������
            }
        }
    }
    else
    {
        TR1 = 0;    
        BUZ = 1;
    }
}    

/****************************************************************
 * ������: Timer1Routine
 * ����: ��ʱ��1�жϴ���(���ڲ�������)
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void Timer1Routine(void) interrupt 3
{
    BUZ = !BUZ;
    TH1 = th1;
    TL1 = tl1;
}

/****************************************************************
 * ������: main
 * ����: ������
 * ����: ��
 * ����ֵ: ��
 ****************************************************************/
void main(void)
{
    /* ϵͳ��ʼ�� */
    Init();
    InitMusic(0);
    
    /* �ȴ���ʼ��Ϸ */
    while(GetIsPause() == 1)
    {
        KeyCatchMid();
        Display();
    }
    
    /* ����Ϸѭ�� */
    while(1)
    {
        if(GetIsPause() == 0)
        {
            /* ��Ϸ������ */
            KeyCatch();
            if(GetIsStep() == 1)
            {
                if(GetIsCollide() == 1)
                {
                    /* ������ײ����Ϸ���� */
                    InitMusic(3);
                    SetIsPause(1);
                }
                else
                {
                    /* �ƶ��� */
                    Step();
                    if(GetIsLengthen() == 1)
                    {
                        SetFood();
                        score++;
                    }
                    SetHeadDirectionBit(0x00);
                    SetIsStep(0);
                    SetIsLengthen(0);
                }
            }
            Display();
        }
        else
        {
            /* ��Ϸ��ͣ/����״̬ */
            if(GetIsDrawn() == 0)
            {
                InitScoreImage();
            }
            KeyCatchMid();
            Display();
        }
    }
}