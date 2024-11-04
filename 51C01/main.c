/****************************************************************
 * 文件名: main.c
 * 功能: 贪吃蛇游戏主程序实现
 ****************************************************************/

#include "main.h" 
#include "Queue.h"

/* 常量数据定义 */
// LED点阵行扫描数据表
UINT8 code LED_ROW[32] = {
    0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,  // 上半部分行选择
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 中间填充
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 中间填充
    0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80   // 下半部分行选择
};

// 开始界面图案数据
UINT8 code START_IMG[32] = {
    0xFF,0xFF,0x7F,0xC0,0xBF,0x9F,0xDF,0xB7,
    0xDF,0x9F,0xDF,0xC0,0xDF,0xFF,0x0F,0xE0,
    0xE7,0xCF,0xF7,0xDF,0xE7,0xCF,0x83,0x83,
    0xFB,0xBF,0xF3,0x9F,0x05,0xC0,0xFF,0xFF
};

// 分数显示界面图案数据
UINT8 code SCORE_IMG[32] = {
    0x00,0x00,0x00,0x20,0x04,0x40,0x0A,0x00,
    0xC2,0x0C,0x24,0x12,0x28,0x12,0xC6,0x4C,
    0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,
    0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00
};

// 方向转换表
UINT8 code directionTable[4][2] = {
    {1,3},  // 上
    {2,0},  // 右
    {3,1},  // 下
    {0,2}   // 左
};

// 移动步进表
UINT8 code stepTable[4][2] = {
    {2,1},  // 上移
    {1,2},  // 右移
    {0,1},  // 下移
    {1,0}   // 左移
};

// 数字显示点阵数据
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

// 音乐播放定时器数据表
UINT8 code th1Table[2][21] = {
    {0xF8,0xF9,0xFA,0xFA,0xFB,0xfa,0xfa,0xFC,0xFC,0xFD,0xFD,0xFD,0xFD,
     0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF},
    {0x8B,0x5B,0x14,0x67,0x04,0x67,0x67,0x43,0xAC,0x09,0x33,0x82,0xC7,
     0x05,0x21,0x56,0x84,0x9A,0xC1,0xE3,0x02}
};

// 音乐数据表
UINT8 code musicTable[4][4] = {
    {101,103,105,137},  // 开始音乐
    {111,113,225,225},  // 吃到食物音效
    {101,101,137,137},  // 游戏开始音效
    {107,105,103,131}   // 游戏结束音效
};

/* 全局变量定义 */
Queue Snake;                    // 蛇身队列
UINT8 foodPosition;            // 食物位置
UINT8 snakeTailPosition;       // 蛇尾位置
UINT8 snakeTailDirection;      // 蛇尾方向
UINT8 snakeHeadPosition;       // 蛇头位置
UINT8 snakeHeadDirection;      // 蛇头方向
UINT8 ground[32];             // 游戏场地数据
UINT8 psw0;                   // 状态字0
UINT8 psw1;                   // 状态字1
UINT8 score;                  // 分数
UINT8 th1, tl1;              // 定时器1初值

/*
状态字说明:
psw0:(Low → High)
headDirectionBit 2bits - 头部方向位
musicPointer     2bit  - 音乐指针
isLengthen      1bit  - 是否增长
isStep          1bit  - 是否移动
isPause         1bit  - 是否暂停
isDrawn         1bit  - 是否已绘制

psw1:(Low → High)
timer0Counter   3bit  - 定时器0计数
isMusic        1bit  - 是否播放音乐
musicBeat      2bit  - 音乐节拍
beatCount      2bit  - 节拍计数

方向编码:
00 - 上
01 - 右
10 - 下
11 - 左

方向位编码:
10 - 右转
11 - 左转
00 - 无转向
01 - 保留
*/

/****************************************************************
 * 基础功能函数实现
 ****************************************************************/

/****************************************************************
 * 函数名: Init
 * 功能: 系统初始化
 * 参数: 无
 * 返回值: 无
 ****************************************************************/
void Init(void)
{
    /* 初始化定时器计数器 */
    SetTimer0Counter(TIMER0COUNTER);
    
    /* 配置定时器 */
    TMOD = 0x11;      // 设置定时器0和1为模式1
    TH0 = TH0_N;      // 设置定时器0初值
    TL0 = TL0_N;
    IE = 0x8a;        // 使能定时器0和1中断
    TR0 = 1;          // 启动定时器0
    
    /* 显示开始界面 */
    DrawStartImage();
    SetIsPause(1);
}

/****************************************************************
 * 函数名: InitGame
 * 功能: 游戏初始化
 * 参数: 无
 * 返回值: 无
 ****************************************************************/
void InitGame(void)
{
    UINT8 vector;
    
    /* 清空游戏场地 */
    ClearGround();
    
    /* 初始化状态 */
    SetHeadDirectionBit(0x00);
    SetIsLengthen(0);
    SetIsStep(0);
    SetIsPause(0);
    SetIsDrawn(0);
    score = 0;
    
    /* 初始化蛇队列 */
    QueueEmpty(&Snake.Head, &Snake.Tail, Snake.Buff, &Snake.HeadBit, &Snake.TailBit);
    
    /* 设置蛇的初始位置和方向 */
    snakeTailPosition = PRESTARTPOSITION;
    snakeTailDirection = 0x00;
    snakeHeadPosition = STARTPOSITION;
    snakeHeadDirection = 0x00;
    ground[14] = 0x01;
    
    /* 设置食物位置 */
    SetFood();
    
    /* 初始化方向队列 */
    vector = GetHeadDirectionBit();
    DataIn(&Snake.Head, Snake.Buff, &Snake.HeadBit, sizeof(Snake.Buff), &vector);
}

/****************************************************************
 * 函数名: InitScoreImage
 * 功能: 初始化分数显示界面
 * 参数: 无
 * 返回值: 无
 ****************************************************************/
void InitScoreImage(void)
{
    DrawScoreImage();
    DrawScore();
    SetIsDrawn(1);
}

/****************************************************************
 * 函数名: InitMusic
 * 功能: 初始化音乐播放
 * 参数: 
 *   pointer: 音乐序号
 * 返回值: 无
 ****************************************************************/
void InitMusic(UINT8 pointer)
{
    TR1 = 0;              // 停止定时器1
    SetIsMusic(1);        // 启用音乐
    SetMusicBeat(0);      // 重置节拍
    SetBeatCount(0);      // 重置节拍计数
    SetMusicPointer(pointer); // 设置音乐序号
}

/****************************************************************
 * 函数名: Delay10Us
 * 功能: 延时函数
 * 参数: 
 *   ten_us: 延时时间(10微秒的倍数)
 * 返回值: 无
 ****************************************************************/
void Delay10Us(UINT16 ten_us)
{
    while(ten_us--);
}

/****************************************************************
 * 位置计算相关函数
 ****************************************************************/

/****************************************************************
 * 函数名: GetX
 * 功能: 获取位置的X坐标
 * 参数: 
 *   position: 位置编码
 * 返回值: X坐标值(0-15)
 ****************************************************************/
UINT8 GetX(UINT8 position)
{
    return position >> 4;
}

/****************************************************************
 * 函数名: GetY
 * 功能: 获取位置的Y坐标
 * 参数: 
 *   position: 位置编码
 * 返回值: Y坐标值(0-15)
 ****************************************************************/
UINT8 GetY(UINT8 position)
{
    return position & 0x0f;
}

/****************************************************************
 * 函数名: GetPosition
 * 功能: 根据坐标获取位置编码
 * 参数: 
 *   x: X坐标(0-15)
 *   y: Y坐标(0-15)
 * 返回值: 位置编码
 ****************************************************************/
UINT8 GetPosition(UINT8 x, UINT8 y)
{
    return (x << 4) + y;
}

/****************************************************************
 * 函数名: GetNextPosition
 * 功能: 计算下一个位置
 * 参数: 
 *   position: 当前位置
 *   direction: 移动方向
 * 返回值: 下一个位置的编码
 ****************************************************************/
UINT8 GetNextPosition(UINT8 position, UINT8 direction)
{
    UINT8 x = GetX(position);
    UINT8 y = GetY(position);
    
    /* 计算新的X坐标 */
    x = x + stepTable[direction][0] - 1;
    if(x == 0x10) x = 0x00;
    if(x == 0xff) x = 0x0f;
    
    /* 计算新的Y坐标 */
    y = y + stepTable[direction][1] - 1;
    if(y == 0x10) y = 0x00;
    if(y == 0xff) y = 0x0f;
    
    return GetPosition(x, y);
}

/****************************************************************
 * 状态操作函数实现
 * 说明：这些函数操作psw0和psw1两个状态字的不同位
 ****************************************************************/

/* 状态获取函数 */
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

/* 状态设置函数 */
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
 * 游戏逻辑相关函数
 ****************************************************************/

/****************************************************************
 * 函数名: GetIsCollide
 * 功能: 检测是否发生碰撞
 * 参数: 无
 * 返回值: 
 *   1: 发生碰撞
 *   0: 未发生碰撞
 ****************************************************************/
UINT8 GetIsCollide(void)
{
    UINT8 x, y, tmp, headDirectionBit, nextPosition;
    
    /* 获取下一个位置 */
    headDirectionBit = GetHeadDirectionBit();
    if((headDirectionBit >> 1) == 1)
        nextPosition = GetNextPosition(snakeHeadPosition, 
            directionTable[snakeHeadDirection][headDirectionBit & 0x01]);
    else
        nextPosition = GetNextPosition(snakeHeadPosition, snakeHeadDirection);
    
    x = GetX(nextPosition);
    y = GetY(nextPosition);
    
    /* 检测是否吃到食物 */
    if(nextPosition == foodPosition)
    {
        InitMusic(1);    // 播放吃食物音效
        SetIsLengthen(1);
        return 0;
    }
    
    /* 检测是否碰到蛇身 */
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
 * 函数名: Rand
 * 功能: 生成随机数
 * 参数: 无
 * 返回值: 随机数值
 ****************************************************************/
UINT8 Rand(void)
{
    UINT16 a = 1103515245;
    UINT16 c = 12345;
    UINT16 m = 65535;    // 2^16限制在16位以内
    
    return (UINT8)((a * (TH0 + psw0 + psw1 + snakeTailPosition + 
           snakeTailDirection + snakeHeadPosition + snakeHeadDirection) + c) % m);
}

/****************************************************************
 * 函数名: Step
 * 功能: 执行蛇的移动
 * 参数: 无
 * 返回值: 无
 ****************************************************************/
void Step(void)
{
    UINT8 x, y, tmp, tailDirectionBit, headDirectionBit;
    
    /* 更新蛇头方向和位置 */
    headDirectionBit = GetHeadDirectionBit();
    DataIn(&Snake.Head, Snake.Buff, &Snake.HeadBit, sizeof(Snake.Buff), &headDirectionBit);
    
    if((headDirectionBit >> 1) == 1)
        snakeHeadDirection = directionTable[snakeHeadDirection][headDirectionBit & 0x01];
    
    snakeHeadPosition = GetNextPosition(snakeHeadPosition, snakeHeadDirection);
    
    /* 在游戏场地上绘制新蛇头 */
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
    
    /* 如果不需要增长，则删除蛇尾 */
    if(GetIsLengthen() == 0)
    {
        DataOut(&Snake.Head, &Snake.Tail, Snake.Buff, &Snake.HeadBit, 
                &Snake.TailBit, sizeof(Snake.Buff), &tailDirectionBit);
        
        if((tailDirectionBit >> 1) == 1)
            snakeTailDirection = directionTable[snakeTailDirection][tailDirectionBit & 0x01];
        
        snakeTailPosition = GetNextPosition(snakeTailPosition, snakeTailDirection);
        
        /* 从游戏场地上删除旧蛇尾 */
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
 * 函数名: SetFood
 * 功能: 在游戏场地上随机放置食物
 * 参数: 无
 * 返回值: 无
 ****************************************************************/
void SetFood(void)
{
    UINT8 x, y, tmp, sign = 1;
    
    /* 循环直到找到空位置 */
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
 * 显示相关函数
 ****************************************************************/

/****************************************************************
 * 函数名: Display
 * 功能: 刷新LED点阵显示
 * 参数: 无
 * 返回值: 无
 ****************************************************************/
void Display(void)
{
    UINT8 i;
    for(i = 0; i < 16; i++)
    {
        /* 传送行列选数据 */
        hc595_write_data(LED_ROW[i], LED_ROW[i+16], 
                        ~ground[i * 2], ~ground[i * 2 + 1]);
        
        /* 消影 */
        hc595_write_data(0x00, 0x00, 0x00, 0x00);
    }           
}

/****************************************************************
 * 函数名: ClearGround
 * 功能: 清空游戏场地
 * 参数: 无
 * 返回值: 无
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
 * 函数名: DrawScore
 * 功能: 绘制分数
 * 参数: 无
 * 返回值: 无
 ****************************************************************/
void DrawScore(void)
{
    UINT8 vector, i;
    
    /* 绘制百位数字 */
    vector = score / 100;
    for(i = 0; i < 5; i++)
    {
        ground[20 + 2 * i] |= scoreTable[vector][i] << 4;
    }
    
    /* 绘制十位数字 */
    vector = (score % 100) / 10;
    for(i = 0; i < 5; i++)
    {
        ground[21 + 2 * i] |= scoreTable[vector][i];
    }
    
    /* 绘制个位数字 */
    vector = score % 10;
    for(i = 0; i < 5; i++)
    {
        ground[21 + 2 * i] |= scoreTable[vector][i] << 4;
    }
}

/****************************************************************
 * 函数名: DrawScoreImage
 * 功能: 绘制分数界面
 * 参数: 无
 * 返回值: 无
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
 * 函数名: DrawStartImage
 * 功能: 绘制开始界面
 * 参数: 无
 * 返回值: 无
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
 * 函数名: hc595_write_data
 * 功能: 向74HC595写入显示数据
 * 参数: 
 *   dat1,dat2: 行选择数据
 *   dat3,dat4: 列显示数据
 * 返回值: 无
 ****************************************************************/
void hc595_write_data(UINT8 dat1, UINT8 dat2, UINT8 dat3, UINT8 dat4)
{
    UINT8 i;
    
    /* 写入第4组数据 */
    for(i = 0; i < 8; i++)
    {
        SER = dat4 >> 7;    // 优先传输高位
        dat4 <<= 1;
        SRCLK = 0;
        SRCLK = 1;          // 上升沿将数据送入寄存器
    }
    
    /* 写入第3组数据 */
    for(i = 0; i < 8; i++)
    {
        SER = dat3 >> 7;
        dat3 <<= 1;
        SRCLK = 0;
        SRCLK = 1;
    }
    
    /* 写入第2组数据 */
    for(i = 0; i < 8; i++)
    {
        SER = dat2 >> 7;
        dat2 <<= 1;
        SRCLK = 0;
        SRCLK = 1;
    }
    
    /* 写入第1组数据 */
    for(i = 0; i < 8; i++)
    {
        SER = dat1 >> 7;
        dat1 <<= 1;
        SRCLK = 0;
        SRCLK = 1;
    }
    
    /* 输出数据 */
    RCLK = 1;
    Delay10Us(1);
    RCLK = 0;
}

/****************************************************************
 * 按键处理函数
 ****************************************************************/

/****************************************************************
 * 函数名: KeyCatch
 * 功能: 游戏中的按键处理
 * 参数: 无
 * 返回值: 无
 ****************************************************************/
void KeyCatch(void)
{
    /* 检测上键 */
    P1 = 0xef;
    if (L1 == 0)
    {
        if(snakeHeadDirection == 0x02) SetHeadDirectionBit(0x02); 
        if(snakeHeadDirection == 0x00) SetHeadDirectionBit(0x03); 
    }
    Delay10Us(1);
    
    /* 检测左右键 */
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
    
    /* 检测下键 */
    P1 = 0xbf;
    if (L1 == 0)
    {
        if(snakeHeadDirection == 0x00) SetHeadDirectionBit(0x02); 
        if(snakeHeadDirection == 0x02) SetHeadDirectionBit(0x03); 
    }
    Delay10Us(1);
}

/****************************************************************
 * 函数名: KeyCatchMid
 * 功能: 游戏开始/结束时的按键处理
 * 参数: 无
 * 返回值: 无
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
 * 中断处理函数
 ****************************************************************/

/****************************************************************
 * 函数名: Timer0Routine
 * 功能: 定时器0中断处理
 * 参数: 无
 * 返回值: 无
 ****************************************************************/
void Timer0Routine(void) interrupt 1
{
    UINT8 soundCode;
    
    /* 更新定时器计数 */
    SetTimer0Counter(GetTimer0Counter() - 1);
    TH0 = TH0_N;
    TL0 = TL0_N;
    
    /* 检查是否需要移动蛇 */
    if(GetTimer0Counter() == 0)
    {
        SetTimer0Counter(TIMER0COUNTER);
        SetIsStep(1);
    }
    
    /* 处理音乐播放 */
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
                SetIsMusic(0); // 音乐播放完毕
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
 * 函数名: Timer1Routine
 * 功能: 定时器1中断处理(用于产生音调)
 * 参数: 无
 * 返回值: 无
 ****************************************************************/
void Timer1Routine(void) interrupt 3
{
    BUZ = !BUZ;
    TH1 = th1;
    TL1 = tl1;
}

/****************************************************************
 * 函数名: main
 * 功能: 主函数
 * 参数: 无
 * 返回值: 无
 ****************************************************************/
void main(void)
{
    /* 系统初始化 */
    Init();
    InitMusic(0);
    
    /* 等待开始游戏 */
    while(GetIsPause() == 1)
    {
        KeyCatchMid();
        Display();
    }
    
    /* 主游戏循环 */
    while(1)
    {
        if(GetIsPause() == 0)
        {
            /* 游戏进行中 */
            KeyCatch();
            if(GetIsStep() == 1)
            {
                if(GetIsCollide() == 1)
                {
                    /* 发生碰撞，游戏结束 */
                    InitMusic(3);
                    SetIsPause(1);
                }
                else
                {
                    /* 移动蛇 */
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
            /* 游戏暂停/结束状态 */
            if(GetIsDrawn() == 0)
            {
                InitScoreImage();
            }
            KeyCatchMid();
            Display();
        }
    }
}