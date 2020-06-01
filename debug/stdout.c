#include <debug_manager.h>
#include <stdio.h>
#include <string.h>  // strlen

static int StdoutDebugPrint(const char*);

static T_DebugOpr g_tStdoutDebugOpr = {
    .name = "stdout",
    .isCanUse = 1,
    .DebugPrint = StdoutDebugPrint,
};
// Notice: ע��Stdoutû�ж���DebugInit/DebugExit������ʱҪ�ж�
static int StdoutDebugPrint(const char* pcData)
{
    printf(pcData);
    return strlen(pcData);
}

int StdoutInit(void)
{
    return RegisterDebugOpr(&g_tStdoutDebugOpr);
}

