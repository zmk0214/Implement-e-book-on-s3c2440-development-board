#include <disp_manager.h>
#include <config.h>
#include <string.h>  // strcmp

/* g_ptDispOprHead ȫ������ͷָ�� */
static PT_DispOpr g_ptDispOprHead;


/**********************************************************************
* �������ƣ� RegisterDispOpr
* ���������� ע��DispOpr�ṹ�嵽g_ptDispOprHeadָ���ȫ������
* ��������� PT_DispOpr ptDispOpr��fb.c�������²��ļ��ṩ�Ľṹ��
* ��������� int
�� * �� �� ֵ�� 0-�ɹ�  
   * ����˵���� ��
* �޸�����           �汾��      �޸���	      �޸�����
 * -----------------------------------------------
* 2020/05/9	  V1.0	  XXXX	      XXXX
***********************************************************************/
int RegisterDispOpr(PT_DispOpr ptDispOpr)
{
    PT_DispOpr ptTmp;
    if(!g_ptDispOprHead)
    {
        g_ptDispOprHead = ptDispOpr;
        g_ptDispOprHead->ptNext = NULL;
    }
    else
    {
        ptTmp = g_ptDispOprHead;
        while (ptTmp->ptNext)
        {
            ptTmp = ptTmp->ptNext;
        }
        ptTmp->ptNext = ptDispOpr;
        ptDispOpr->ptNext = NULL;
    }
    ptTmp = NULL;
    return 0;
}
/**********************************************************************
* �������ƣ� ShowDispOpr
* ���������� ��ӡg_ptDispOprHeadָ���ȫ����������нڵ�
* ��������� void
* ��������� void
�� * �� �� ֵ�� ��  
   * ����˵���� ��
* �޸�����           �汾��      �޸���	      �޸�����
 * -----------------------------------------------
* 2020/05/9	  V1.0	  XXXX	      XXXX
***********************************************************************/

void ShowDispOpr(void)
{
    int i = 0;
    PT_DispOpr ptTmp = g_ptDispOprHead;
    while(ptTmp)
    {
        printf("%02d %s\n", i++, ptTmp->name);
        ptTmp = ptTmp->ptNext;
    }
    ptTmp = NULL;
}

/**********************************************************************
* �������ƣ� GetDispOpr
* ���������� �������ִ�ȫ�������ҵ���Ӧ��DispOpr�ṹ��
* ��������� char* pcName
* ��������� PT_DispOpr
�� * �� �� ֵ�� �ɹ�����ָ��DispOpr��ָ�룬���򷵻�NULL 
   * ����˵���� ��
* �޸�����           �汾��      �޸���	      �޸�����
 * -----------------------------------------------
* 2020/05/9	  V1.0	  XXXX	      XXXX
***********************************************************************/

PT_DispOpr GetDispOpr(char* pcName)
{
    PT_DispOpr ptTmp = g_ptDispOprHead;
    while(ptTmp)
    {
        if(strcmp(ptTmp->name, pcName) == 0)
        {
            return ptTmp;
        }
        ptTmp = ptTmp->ptNext;
    }
    ptTmp = NULL;
    return NULL;
}
/**********************************************************************
* �������ƣ� DisplayInit
* ���������� �����²�(��fb.c)�ĳ�ʼ������������ʾ��ʼ��
* ��������� void
* ��������� int
�� * �� �� ֵ�� ���ص��õĳ�ʼ�������ķ���ֵ 
   * ����˵���� ��
* �޸�����           �汾��      �޸���	      �޸�����
 * -----------------------------------------------
* 2020/05/9	  V1.0	  XXXX	      XXXX
***********************************************************************/

int DisplayInit(void)
{
    int iError;
    iError = FBInit();
    return iError;
}


