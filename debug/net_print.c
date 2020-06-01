#include <config.h>
#include <debug_manager.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define SERVER_PORT 8888
#define PRINT_BUG_SIZE (16*1024)

static int isFull(void);
static int isEmpty(void);
static int putData(char c);
static int GetData(char * pcVal);

static int NetPrintDebugInit(void);
static int NetPrintDebugExit(void);
static int NetPrintDebugPrint(const char*);

static void* NetSendThreadFunction(void* pVoid);
static void* NetRecvThreadFunction(void* pVoid);

static int g_isocketfd;
static struct sockaddr_in g_tSocketServerAddr;
static struct sockaddr_in g_tSocketClientAddr;

static char * g_pcNetPrintBuf;
static int g_iWritePos = 0;
static int g_iReadPos = 0;

static pthread_t g_tSendThreadID;
static pthread_t g_tRecvThreadID;
static pthread_mutex_t g_tNetSendMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tNetSendConVar = PTHREAD_COND_INITIALIZER;
static int iHaveConnected = 0;

static T_DebugOpr g_tNetPrintDebugOpr = {
    .name       = "netprint",
    .isCanUse   = 1,
    .DebugInit  = NetPrintDebugInit,
    .DebugExit  = NetPrintDebugExit,
    .DebugPrint = NetPrintDebugPrint,
};
static int NetPrintDebugInit(void)
{
    g_isocketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(-1 == g_isocketfd)
    {
        printf("create socket fail!\n");
        return -1;
    }
    
    g_tSocketServerAddr.sin_family = AF_INET;
    g_tSocketServerAddr.sin_port   = htons(SERVER_PORT);
    g_tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;
    memset(g_tSocketServerAddr.sin_zero, 0, 8);
    
    if(-1 == bind(g_isocketfd, (const struct sockaddr *)&g_tSocketServerAddr,
                    sizeof(struct sockaddr)) )
    {
        printf("bind fail!\n");
        return -1;
    }

    g_pcNetPrintBuf = malloc(PRINT_BUG_SIZE);
    if(NULL == g_pcNetPrintBuf)
    {
        close(g_isocketfd);
        printf("NetPrintBuf malloc fail!\n");
        return -1;
    }

    /*创建两个线程，一个发送线程，一个接收线程*/
    pthread_create(&g_tSendThreadID, NULL, NetSendThreadFunction, NULL);
    pthread_create(&g_tRecvThreadID, NULL, NetRecvThreadFunction, NULL);
  
    return 0;
}

static int NetPrintDebugExit(void)
{
    close(g_isocketfd);
    free(g_pcNetPrintBuf);
    return 0;
}

static int NetPrintDebugPrint(const char* pcData)
{
    /* 将pcData数据放到环形缓冲区 */
    int i;
    for(i = 0; i < strlen(pcData); ++i)
    {
        if(0 != putData(pcData[i]))
            break;
    }
    /* 如果有客户发来数据，唤醒发送线程 */
    
    pthread_mutex_lock(&g_tNetSendMutex);
	pthread_cond_signal(&g_tNetSendConVar);
//    printf("had valid data");
    pthread_mutex_unlock(&g_tNetSendMutex);
    return i;

    
}

int NetPrintInit(void)
{
    return RegisterDebugOpr(&g_tNetPrintDebugOpr);
}
static void* NetSendThreadFunction(void* pVoid)
{
    char strTmpBuf[512];
    char cVal;
    int i;
    int iAddrLen;
    int iSendLen;
    while(1)
    {
        /* 默认休眠   ,等待有数据时的唤醒  */ 
        pthread_mutex_lock(&g_tNetSendMutex);
        pthread_cond_wait(&g_tNetSendConVar, &g_tNetSendMutex); 
        pthread_mutex_unlock(&g_tNetSendMutex);

        while(iHaveConnected && !isEmpty())
        {
        
           
            /* 被唤醒后,把环形缓冲区的数据发给客户 */
            i = 0;
            /* 把环形缓冲区的数据取出来, 最多取512字节 */
            while((i < 512) && (0 == GetData(&cVal)))
            {
                strTmpBuf[i] = cVal;
                i++;
            }
            iAddrLen = sizeof(struct sockaddr);
            iSendLen = sendto(g_isocketfd, g_pcNetPrintBuf, i, 0,
                          (const struct sockaddr *)&g_tSocketClientAddr, iAddrLen);
        }
    }
    
    return NULL;
   
	
    
}
static void* NetRecvThreadFunction(void* pVoid)
{
    int iAddrLen;
    int iRecvLen;
    char strRecvBuf[1000];
    struct sockaddr_in tSocketClientAddr;

    
    while(1)
    {
        iAddrLen = sizeof(struct sockaddr);
        //DBG_PRINTF("in NetDbgRecvTreadFunction\n");
        iRecvLen = recvfrom(g_isocketfd, strRecvBuf, 999, 0,
                        (struct sockaddr *)&tSocketClientAddr, &iAddrLen);
        //printf("after recvfrom\n");
        if(iRecvLen > 0)
        {
            strRecvBuf[iRecvLen] = '\0';
            DBG_PRINTF("netprint.c get msg: %s\n", strRecvBuf);
            /* 解析数据:
			 * setclient            : 设置接收打印信息的客户端
			 * dbglevel=0,1,2...    : 修改打印级别
			 * stdout=0             : 关闭stdout打印
			 * stdout=1             : 打开stdout打印
			 * netprint=0           : 关闭netprint打印
			 * netprint=1           : 打开netprint打印
			 */
			 if(strcmp(strRecvBuf, "setclient") == 0)
			 {
                g_tSocketClientAddr = tSocketClientAddr;
                printf("success setclient\n");
                iHaveConnected = 1;
             }
             else if(strncmp(strRecvBuf, "dgblevel=", 9) == 0)
             {
                SetDbgLevel(strRecvBuf);
             }
             else
             {
                SetDbgChannel(strRecvBuf);
             }
        }
           

    }
    return NULL;

}

static int isFull(void)
{
    return ( (g_iWritePos + 1)%PRINT_BUG_SIZE == g_iReadPos);
}
static int isEmpty(void)
{
    return (g_iWritePos == g_iReadPos);
}
static int putData(char c)
{
    if(isFull())
        return -1;
    else
    {
        g_pcNetPrintBuf[g_iWritePos] = c;
        g_iWritePos = (g_iWritePos + 1) % PRINT_BUG_SIZE;
        return 0;
    }  
}
static int GetData(char * pcVal)
{
    if(isEmpty())
        return -1;
    else
    {
        *pcVal = g_pcNetPrintBuf[g_iReadPos];
        g_iReadPos = (g_iReadPos + 1) % PRINT_BUG_SIZE;
        return 0;
    }
}

