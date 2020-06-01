#include <input_manager.h>
#include <string.h>  // strcmp
#include <pthread.h>

/* g_ptInputOprHead 全局链表头指针 */
static PT_InputOpr g_ptInputOprHead;

static T_InputEvent g_tInputEvent;
static pthread_mutex_t g_tMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tConVar = PTHREAD_COND_INITIALIZER;
int RegisterInputOpr(PT_InputOpr ptInputOpr)
{
    PT_InputOpr ptTmp;
    if(!g_ptInputOprHead)
    {
        g_ptInputOprHead = ptInputOpr;
        ptInputOpr->ptNext = NULL;
    }
    else
    {
        ptTmp = g_ptInputOprHead;
        while (ptTmp->ptNext)
        {
            ptTmp = ptTmp->ptNext;
        }
        ptTmp->ptNext = ptInputOpr;
        ptInputOpr->ptNext = NULL;
    }
    ptTmp = NULL;
    return 0;
}
static void* InputEventThreadFunction(void* pVoid)
{
	T_InputEvent tInputEvent;
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	GetInputEvent = (int (*)(PT_InputEvent))pVoid;
	
	while (1)
	{
		if(0 == GetInputEvent(&tInputEvent))
		{
			/* 唤醒主线程, 把tInputEvent的值赋给一个全局变量 */
			/* 访问临界资源前，先获得互斥量 */
			pthread_mutex_lock(&g_tMutex);
			g_tInputEvent = tInputEvent;

			/*  唤醒主线程 */
			pthread_cond_signal(&g_tConVar);

			/* 释放互斥量 */
			pthread_mutex_unlock(&g_tMutex);
		}
	}
	
	return NULL;
}
int AllInputDevicesInit(void)
{
	PT_InputOpr ptTmp = g_ptInputOprHead;
	int iError = -1;

	while (ptTmp)
	{
		if (0 == ptTmp->DeviceInit())
		{
			pthread_create(&ptTmp->tThreadID, NULL, InputEventThreadFunction, ptTmp->GetInputEvent);
//			printf("name:%s threadIP:%d\n", ptTmp->name, ptTmp->tThreadID);
			iError = 0;
		}
		ptTmp = ptTmp->ptNext;
	}
	return iError;
}
int GetInputEvent(PT_InputEvent ptInputEvent)
{
	/* 休眠 */
	pthread_mutex_lock(&g_tMutex);
	pthread_cond_wait(&g_tConVar, &g_tMutex);	

	/* 被唤醒后,返回数据 */
//	printf("wake up!\n");
	*ptInputEvent = g_tInputEvent;
	pthread_mutex_unlock(&g_tMutex);
	return 0;
}

void ShowInputOpr(void)
{
    int i = 0;
    PT_InputOpr ptTmp = g_ptInputOprHead;
    while(ptTmp)
    {
//        printf("%02d %s\n", i++, ptTmp->name);
        ptTmp = ptTmp->ptNext;
    }
    ptTmp = NULL;
}


int InputInit(void)
{
    int iError;
    
    iError = StdinInit();
    iError |= TouchScreenInit();
    
    return iError;
}



