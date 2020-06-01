#ifndef _DEBUG_MANAGER_H
#define _DEBUG_MANAGER_H
#include <pthread.h>
#include <config.h>

#define	APP_EMERG	"<0>"	/* system is unusable			*/
#define	APP_ALERT	"<1>"	/* action must be taken immediately	*/
#define	APP_CRIT	"<2>"	/* critical conditions			*/
#define	APP_ERR	    "<3>"	/* error conditions			*/
#define	APP_WARNING	"<4>"	/* warning conditions			*/
#define	APP_NOTICE	"<5>"	/* normal but significant condition	*/
#define	APP_INFO	"<6>"	/* informational			*/
#define	APP_DEBUG	"<7>"	/* debug-level messages			*/

#define DEFAULT_DBGLEVEL  4

typedef struct DebugOpr{
    char *name;
    int  isCanUse;
    int (*DebugInit)(void);
    int (*DebugExit)(void);
    int (*DebugPrint)(const char *);
    struct DebugOpr *ptNext;
}T_DebugOpr, *PT_DebugOpr;


int SetDbgChannel(char *strRecvBuf);
int SetDbgLevel(char *strRecvBuf);
PT_DebugOpr GetDebugOpr(char* pcName);
void ShowDebugOpr(void);
int RegisterDebugOpr(PT_DebugOpr ptDebugOpr);
int DebugInit(void);
int DebugPrint(const char *pcFormat, ...);
int InitDebugChanel(void);

int StdoutInit(void);
int NetPrintInit(void);


#endif /* _DISP_MANAGER_H */


