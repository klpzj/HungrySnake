/****************************************************************
 * 文件名: Queue.h
 * 功能: 队列操作相关的头文件定义
 ****************************************************************/

#ifndef __QUEUE_H__
#define __QUEUE_H__

/* 类型检查和定义 */
#ifdef __MAIN_H__
#else
#ifndef UINT8
typedef unsigned char UINT8;
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 队列操作函数声明 */
// 初始化队列为空
extern void QueueEmpty(UINT8 **Head, 
                      UINT8 **Tail, 
                      UINT8 *HBuff, 
                      UINT8 *HeadBit, 
                      UINT8 *TailBit);

// 数据入队
extern void DataIn(UINT8 **Head, 
                  UINT8 *HBuff, 
                  UINT8 *Bit, 
                  UINT8 Len, 
                  UINT8 *HData);

// 数据出队
extern UINT8 DataOut(UINT8 **Head, 
                    UINT8 **Tail, 
                    UINT8 *HBuff, 
                    UINT8 *TailBit, 
                    UINT8 *HeadBit, 
                    UINT8 Len, 
                    UINT8 *Data);

#ifdef __cplusplus
}
#endif
	
#endif