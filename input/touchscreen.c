#include <input_manager.h>
#include <config.h>
#include <draw.h>

#include <stdlib.h>
#include <tslib.h>

static struct tsdev *g_tTSDev;
static int giXres;
static int giYres;

static int TouchScreenDeviceInit(void);
static int TouchScreenDeviceExit(void);
static int GetTouchScreenInputEvent(PT_InputEvent ptInputEvent);

static T_InputOpr g_tTouchScreenOpr = {
    .name = "touchscreen",
    .DeviceInit = TouchScreenDeviceInit,
    .DeviceExit = TouchScreenDeviceExit,
    .GetInputEvent = GetTouchScreenInputEvent,
};
   
static int TouchScreenDeviceInit(void)
{
    /* 参考自 tslib-1.4 的 ts_print.c */
    
	char *tsdevice=NULL;

    if( (tsdevice = getenv("TSLIB_TSDEVICE")) != NULL ) {
            g_tTSDev = ts_open(tsdevice, 0);
    } else {
        g_tTSDev = ts_open("/dev/event0", 0);
    }
    if (!g_tTSDev) {
		DBG_PRINTF("ts_open error!\n");
		return -1;
	}

	if (ts_config(g_tTSDev)) {
		DBG_PRINTF("ts_config error!\n");
		return -1;
	}

    if (GetDispResolution(&giXres, &giYres))
	{
		return -1;
	}
    return 0;
}
static int TouchScreenDeviceExit(void)
{
    return 0;
}

static int GetTouchScreenInputEvent(PT_InputEvent ptInputEvent)
{
    struct ts_sample tSamp;
	struct ts_sample tSampPressed;
	struct ts_sample tSampReleased;
	int iRet;
	int bStart = 0;
	int iDelta;

	static struct timeval tPreTime;
	

	while (1)
	{
		iRet = ts_read(g_tTSDev, &tSamp, 1); /* 如果无数据则休眠 */
		printf("after ts_read\n");
		if (iRet == 1)
		{
			if ((tSamp.pressure > 0) && (bStart == 0))
			{
				/* 刚按下 */
				/* 记录刚开始压下的点 */
				tSampPressed = tSamp;
				bStart = 1;
			}
			
			if (tSamp.pressure <= 0)
			{
				/* 松开 */
				tSampReleased = tSamp;

				/* 处理数据 */
				if (!bStart)
				{
					return -1;
				}
				else
				{
					iDelta = tSampReleased.x - tSampPressed.x;
					ptInputEvent->time = tSampReleased.tv;
					ptInputEvent->type = INPUT_EVENT_TYPE_TOUCHSCREEN;

                    printf("iDelta:%d\n", iDelta);
					if (iDelta > giXres/10)
					{
						/* 翻到上一页 */
						ptInputEvent->code = INPUT_EVENT_UP;
					}
					else if (iDelta < 0 - giXres/10)
					{
						/* 翻到下一页 */
						ptInputEvent->code = INPUT_EVENT_DOWN;
					}
					else
					{
						ptInputEvent->code = INPUT_EVENT_UNKNOWN;
					}
					return 0;
				}
			}
		}
		else
		{
			return -1;
		}
	}

	return 0;
}

int TouchScreenInit(void)
{
    return RegisterInputOpr(&g_tTouchScreenOpr);
}

