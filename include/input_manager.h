#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H

#include <sys/time.h>
#include <pthread.h>
#include <config.h>

#define INPUT_EVENT_UP 0
#define INPUT_EVENT_DOWN 1
#define INPUT_EVENT_EXIT 2
#define INPUT_EVENT_UNKNOWN -1

#define INPUT_EVENT_TYPE_STDIN 0
#define INPUT_EVENT_TYPE_TOUCHSCREEN 1


typedef struct InputEvent{
    struct timeval time;
    int type;
    int code;

}T_InputEvent, *PT_InputEvent;

typedef struct InputOpr{
    char * name;
	pthread_t tThreadID;
    int (*DeviceInit)(void);
    int (*DeviceExit)(void);
    int (*GetInputEvent)(PT_InputEvent ptInputEvent);
    struct InputOpr * ptNext;

}T_InputOpr, *PT_InputOpr;


int RegisterInputOpr(PT_InputOpr ptInputOpr);
void ShowInputOpr(void);
int InputInit(void);
int AllInputDevicesInit(void);
int GetInputEvent(PT_InputEvent ptInputEvent);

int StdinInit(void);
int TouchScreenInit(void);



#endif /* _INPUT_MANAGER_H */
