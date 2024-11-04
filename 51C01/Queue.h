/****************************************************************
 * �ļ���: Queue.h
 * ����: ���в�����ص�ͷ�ļ�����
 ****************************************************************/

#ifndef __QUEUE_H__
#define __QUEUE_H__

/* ���ͼ��Ͷ��� */
#ifdef __MAIN_H__
#else
#ifndef UINT8
typedef unsigned char UINT8;
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ���в����������� */
// ��ʼ������Ϊ��
extern void QueueEmpty(UINT8 **Head, 
                      UINT8 **Tail, 
                      UINT8 *HBuff, 
                      UINT8 *HeadBit, 
                      UINT8 *TailBit);

// �������
extern void DataIn(UINT8 **Head, 
                  UINT8 *HBuff, 
                  UINT8 *Bit, 
                  UINT8 Len, 
                  UINT8 *HData);

// ���ݳ���
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