/****************************************************************
 * 文件名: main.h
 * 功能: 贪吃蛇游戏的主要头文件，包含基础定义和函数声明
 ****************************************************************/

#ifndef __MAIN_H__ 
#define __MAIN_H__ 

#ifdef __cplusplus 
extern "C" { 
#endif 

/* 包含头文件 */
#include <reg51.h> 

/* 常量定义 */
#define STARTPOSITION      0x07    // 蛇的起始位置
#define PRESTARTPOSITION   0xf7    // 蛇的预起始位置
#define TIMER0MILISECOND  200     // 定时器0的毫秒数
#define TIMER0COUNTER     (TIMER0MILISECOND / 50)  // 定时器计数值

/* 定时器初值 */
#define TH0_N             0x3c    // 定时器0高位初值
#define TL0_N             0xb0    // 定时器0低位初值

/* IO口定义 */
sbit L0 = P1^0;    // 按键输入0
sbit L1 = P1^1;    // 按键输入1
sbit L2 = P1^2;    // 按键输入2
sbit L3 = P1^3;    // 按键输入3
sbit SER = P3^4;   // 595串行数据输入
sbit RCLK = P3^5;  // 595存储寄存器时钟
sbit SRCLK = P3^6; // 595移位寄存器时钟
sbit BUZ = P3^7;   // 蜂鸣器输出

/* 类型定义 */
typedef unsigned char UINT8; 
typedef unsigned int  UINT16; 	

/* 队列结构体定义 */
typedef struct {
    UINT8 *Head;     // 队列头指针
    UINT8 *Tail;     // 队列尾指针
    UINT8 HeadBit;   // 头部位置标记
    UINT8 TailBit;   // 尾部位置标记
    UINT8 Buff[16];  // 队列缓冲区
} Queue;

/* 函数声明 */
// 初始化函数
void Init(void);                    // 系统初始化
void InitGame(void);                // 游戏初始化
void InitScoreImage(void);          // 分数显示初始化
void InitMusic(UINT8 pointer);      // 音乐初始化

// 延时函数
void Delay10Us(UINT16 ten_us);      // 10微秒延时

// 按键处理函数
void KeyCatch(void);                // 游戏中按键捕获
void KeyCatchMid(void);             // 中间状态按键捕获

// 显示相关函数
void hc595_write_data(UINT8 dat1, UINT8 dat2, UINT8 dat3, UINT8 dat4); // 595数据写入
void Display(void);                 // 显示刷新
void DrawScore(void);               // 绘制分数
void DrawScoreImage(void);          // 绘制分数界面
void DrawStartImage(void);          // 绘制开始界面

// 游戏逻辑函数
void Step(void);                    // 移动一步
void SetFood(void);                 // 设置食物
void ClearGround(void);             // 清空游戏场地

// 位置计算函数
UINT8 GetNextPosition(UINT8 position, UINT8 direction);  // 获取下一位置
UINT8 GetPosition(UINT8 x, UINT8 y);                    // 获取位置编码
UINT8 GetX(UINT8 position);                             // 获取X坐标
UINT8 GetY(UINT8 position);                             // 获取Y坐标
UINT8 GetIsCollide(void);                               // 检测碰撞
UINT8 Rand(void);                                       // 随机数生成

// 中断处理函数
void Timer0Routine(void);           // 定时器0中断
void Timer1Routine(void);           // 定时器1中断

// 状态获取函数
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

// 状态设置函数
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