#include <disp_manager.h>
#include <config.h>
#include <string.h>  // strcmp

/* g_ptDispOprHead 全局链表头指针 */
static PT_DispOpr g_ptDispOprHead;


/**********************************************************************
* 函数名称： RegisterDispOpr
* 功能描述： 注册DispOpr结构体到g_ptDispOprHead指向的全局链表
* 输入参数： PT_DispOpr ptDispOpr，fb.c或其他下层文件提供的结构体
* 输出参数： int
　 * 返 回 值： 0-成功  
   * 其它说明： 无
* 修改日期           版本号      修改人	      修改内容
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
* 函数名称： ShowDispOpr
* 功能描述： 打印g_ptDispOprHead指向的全局链表的所有节点
* 输入参数： void
* 输出参数： void
　 * 返 回 值： 无  
   * 其它说明： 无
* 修改日期           版本号      修改人	      修改内容
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
* 函数名称： GetDispOpr
* 功能描述： 根据名字从全局链表找到对应的DispOpr结构体
* 输入参数： char* pcName
* 输出参数： PT_DispOpr
　 * 返 回 值： 成功返回指向DispOpr的指针，否则返回NULL 
   * 其它说明： 无
* 修改日期           版本号      修改人	      修改内容
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
* 函数名称： DisplayInit
* 功能描述： 调用下层(如fb.c)的初始化函数进行显示初始化
* 输入参数： void
* 输出参数： int
　 * 返 回 值： 返回调用的初始化函数的返回值 
   * 其它说明： 无
* 修改日期           版本号      修改人	      修改内容
 * -----------------------------------------------
* 2020/05/9	  V1.0	  XXXX	      XXXX
***********************************************************************/

int DisplayInit(void)
{
    int iError;
    iError = FBInit();
    return iError;
}


