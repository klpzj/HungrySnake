/****************************************************************
 * 文件名: Queue.c
 * 功能: 实现队列的基本操作
 ****************************************************************/

#include "Queue.h"

/****************************************************************
 * 函数名: DataIn
 * 功能: 将数据写入队列
 * 参数: 
 *   Head: 队列头指针的指针
 *   HBuff: 队列缓冲区
 *   HeadBit: 头部位置标记
 *   Len: 缓冲区长度
 *   HData: 要写入的数据
 * 返回值: 无
 ****************************************************************/
void DataIn(UINT8 **Head, UINT8 *HBuff, UINT8 *HeadBit, UINT8 Len, UINT8 *HData)
{
    UINT8 tmp;
    
    /* 将数据左移到正确位置并写入 */
    tmp = *HData;
    tmp <<= *HeadBit;
    **Head |= tmp; 

    /* 更新头部位置 */
    *HeadBit += 2;  // 每次移动2位
    if(*HeadBit >= 8)
    {
        *HeadBit = 0;
        (*Head)++;
    }

    /* 处理缓冲区循环 */
    if(*Head == HBuff + Len)
    {
        *Head = HBuff;
    }
} 

/****************************************************************
 * 函数名: DataOut
 * 功能: 从队列读出数据
 * 参数: 
 *   Head: 队列头指针的指针
 *   Tail: 队列尾指针的指针
 *   HBuff: 队列缓冲区
 *   HeadBit: 头部位置标记
 *   TailBit: 尾部位置标记
 *   Len: 缓冲区长度
 *   Data: 读出数据的存储位置
 * 返回值: 
 *   1: 成功读出数据
 *   0: 队列为空
 ****************************************************************/
UINT8 DataOut(UINT8 **Head, UINT8 **Tail, UINT8 *HBuff, 
              UINT8 *HeadBit, UINT8 *TailBit, UINT8 Len, UINT8 *Data)
{
    UINT8 back = 0;
    UINT8 tmp;

    /* 检查队列是否为空 */
    if(*Head != *Tail || *TailBit < *HeadBit)
    {
        /* 读取数据 */
        tmp = **Tail >> *TailBit;
        tmp &= 0x03;  // 只取最低2位
        *Data = tmp;
        
        /* 清除已读取的位 */
        tmp = 0x03 << *TailBit;
        tmp = ~tmp;
        **Tail &= tmp;
        
        /* 更新尾部位置 */
        *TailBit += 2;  // 每次移动2位
        if(*TailBit >= 8)
        {
            *TailBit = 0;
            (*Tail)++;
            if(*Tail == HBuff + Len)
            {
                *Tail = HBuff;
            }
        }
        
        back = 1;
    }

    return back;
}

/****************************************************************
 * 函数名: QueueEmpty
 * 功能: 初始化队列为空
 * 参数: 
 *   Head: 队列头指针的指针
 *   Tail: 队列尾指针的指针
 *   HBuff: 队列缓冲区
 *   HeadBit: 头部位置标记
 *   TailBit: 尾部位置标记
 * 返回值: 无
 ****************************************************************/
void QueueEmpty(UINT8 **Head, UINT8 **Tail, UINT8 *HBuff, 
                UINT8 *HeadBit, UINT8 *TailBit)
{
    UINT8 i;
    
    /* 初始化指针和位置标记 */
    *Head = HBuff;
    *Tail = HBuff;
    *HeadBit = 0;
    *TailBit = 0;
    
    /* 清空缓冲区 */
    for(i = 0; i < 16; i++)
    {
        *(HBuff + i) = 0x00;
    }
}