#include <input_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

static int StdinDeviceInit(void);
static int StdinDeviceExit(void);
static int GetStdinInputEvent(PT_InputEvent ptInputEvent);

static T_InputOpr g_tStdinOpr = {
    .name = "stdin",
    .DeviceInit = StdinDeviceInit,
    .DeviceExit = StdinDeviceExit,
    .GetInputEvent = GetStdinInputEvent,
};
static int StdinDeviceInit(void)
{
    struct termios ttystate;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);
    
    //turn off canonical mode
    ttystate.c_lflag &= ~ICANON;
    
    //minimum of number input read.
    ttystate.c_cc[VMIN] = 1;
   
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

    return 0;
}
static int StdinDeviceExit(void)
{
    struct termios ttystate;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);
 
    //turn on canonical mode
    ttystate.c_lflag |= ICANON;

    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

    return 0;
}
static int GetStdinInputEvent(PT_InputEvent ptInputEvent)
{

   /* ��������ݾͶ�ȡ����������
	 * ���û������, ���̷���, ���ȴ�
	 */

	/* select, poll ���Բ��� UNIX�����߼���� */

 	char c;
	
		/* �������� */
	ptInputEvent->type = INPUT_EVENT_TYPE_STDIN;
	
	c = fgetc(stdin);  /* ������ֱ�������� */
	gettimeofday(&ptInputEvent->time, NULL);
	
	if (c == 'u')
	{
		ptInputEvent->code = INPUT_EVENT_UP;
	}
	else if (c == 'n')
	{
		ptInputEvent->code = INPUT_EVENT_DOWN;
	}
	else if (c == 'q')
	{
		ptInputEvent->code = INPUT_EVENT_EXIT;
	}
	else
	{
		ptInputEvent->code = INPUT_EVENT_UNKNOWN;
	}		
	return 0;
}

int StdinInit(void)
{
    return RegisterInputOpr(&g_tStdinOpr);
}
