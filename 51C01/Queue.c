/****************************************************************
 * �ļ���: Queue.c
 * ����: ʵ�ֶ��еĻ�������
 ****************************************************************/

#include "Queue.h"

/****************************************************************
 * ������: DataIn
 * ����: ������д�����
 * ����: 
 *   Head: ����ͷָ���ָ��
 *   HBuff: ���л�����
 *   HeadBit: ͷ��λ�ñ��
 *   Len: ����������
 *   HData: Ҫд�������
 * ����ֵ: ��
 ****************************************************************/
void DataIn(UINT8 **Head, UINT8 *HBuff, UINT8 *HeadBit, UINT8 Len, UINT8 *HData)
{
    UINT8 tmp;
    
    /* ���������Ƶ���ȷλ�ò�д�� */
    tmp = *HData;
    tmp <<= *HeadBit;
    **Head |= tmp; 

    /* ����ͷ��λ�� */
    *HeadBit += 2;  // ÿ���ƶ�2λ
    if(*HeadBit >= 8)
    {
        *HeadBit = 0;
        (*Head)++;
    }

    /* ��������ѭ�� */
    if(*Head == HBuff + Len)
    {
        *Head = HBuff;
    }
} 

/****************************************************************
 * ������: DataOut
 * ����: �Ӷ��ж�������
 * ����: 
 *   Head: ����ͷָ���ָ��
 *   Tail: ����βָ���ָ��
 *   HBuff: ���л�����
 *   HeadBit: ͷ��λ�ñ��
 *   TailBit: β��λ�ñ��
 *   Len: ����������
 *   Data: �������ݵĴ洢λ��
 * ����ֵ: 
 *   1: �ɹ���������
 *   0: ����Ϊ��
 ****************************************************************/
UINT8 DataOut(UINT8 **Head, UINT8 **Tail, UINT8 *HBuff, 
              UINT8 *HeadBit, UINT8 *TailBit, UINT8 Len, UINT8 *Data)
{
    UINT8 back = 0;
    UINT8 tmp;

    /* �������Ƿ�Ϊ�� */
    if(*Head != *Tail || *TailBit < *HeadBit)
    {
        /* ��ȡ���� */
        tmp = **Tail >> *TailBit;
        tmp &= 0x03;  // ֻȡ���2λ
        *Data = tmp;
        
        /* ����Ѷ�ȡ��λ */
        tmp = 0x03 << *TailBit;
        tmp = ~tmp;
        **Tail &= tmp;
        
        /* ����β��λ�� */
        *TailBit += 2;  // ÿ���ƶ�2λ
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
 * ������: QueueEmpty
 * ����: ��ʼ������Ϊ��
 * ����: 
 *   Head: ����ͷָ���ָ��
 *   Tail: ����βָ���ָ��
 *   HBuff: ���л�����
 *   HeadBit: ͷ��λ�ñ��
 *   TailBit: β��λ�ñ��
 * ����ֵ: ��
 ****************************************************************/
void QueueEmpty(UINT8 **Head, UINT8 **Tail, UINT8 *HBuff, 
                UINT8 *HeadBit, UINT8 *TailBit)
{
    UINT8 i;
    
    /* ��ʼ��ָ���λ�ñ�� */
    *Head = HBuff;
    *Tail = HBuff;
    *HeadBit = 0;
    *TailBit = 0;
    
    /* ��ջ����� */
    for(i = 0; i < 16; i++)
    {
        *(HBuff + i) = 0x00;
    }
}