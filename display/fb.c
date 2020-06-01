#include <config.h>
#include <disp_manager.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/mman.h>

/* 函数声明 */
static int FBDeviceInit(void);
static int FBShowPixel(int iX, int iY, unsigned int dwColor);
static int FBCleanScreen(unsigned int dwBkgdColor);

/* 全局变量 */
static int g_fd;
static unsigned int g_dwLineWidth ;
static unsigned int g_dwPixelWidth;
static unsigned int g_dwScreenSize;

static unsigned char* g_pucFbMem;
static struct fb_var_screeninfo g_tFBVar;
static struct fb_fix_screeninfo g_tFBFix;

static T_DispOpr g_tFBOpr = {
    .name = "fb",
    .DeviceInit  = FBDeviceInit,
    .ShowPixel   = FBShowPixel,
    .CleanScreen = FBCleanScreen,
};
/**********************************************************************
* 函数名称： FBDeviceInit
* 功能描述： LCD显示的初始化，包括打开设备、获得LCD参数
* 输入参数： void
* 输出参数： int
　 * 返 回 值： 0-成功  -1-失败 
   * 其它说明： 无
* 修改日期           版本号      修改人         修改内容
 * -----------------------------------------------
* 2020/05/9       V1.0    XXXX        XXXX
***********************************************************************/

static int FBDeviceInit(void)
{
    g_fd = open(FB_DEVICE_NAME, O_RDWR); 
	if(g_fd < 0)
	{
		DBG_PRINTF("can't open /dev/fb0\n");
		return -1;
	}
	if(ioctl(g_fd, FBIOGET_VSCREENINFO, &g_tFBVar))
	{
		DBG_PRINTF("can't get var\n");
		return -1;
	}
	if(ioctl(g_fd, FBIOGET_FSCREENINFO, &g_tFBFix))
	{
		DBG_PRINTF("can't get fix\n");
		return -1;
	}
	g_dwLineWidth  = g_tFBVar.xres * g_tFBVar.bits_per_pixel / 8;
	g_dwPixelWidth = g_tFBVar.bits_per_pixel / 8;
	g_dwScreenSize = g_tFBVar.xres * g_tFBVar.yres * g_tFBVar.bits_per_pixel / 8;
    g_pucFbMem = (unsigned char*)mmap(NULL, g_dwScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, g_fd, 0);
	if(g_pucFbMem == (unsigned char*)-1)
	{
		DBG_PRINTF("can't map g_pucFbMem\n");
		return -1;
	}
    g_tFBOpr.iXres = g_tFBVar.xres;
    g_tFBOpr.iYres = g_tFBVar.yres;
    g_tFBOpr.iBpp  = g_tFBVar.bits_per_pixel;
    return 0;
}
/**********************************************************************
* 函数名称： FBShowPixel
* 功能描述： 根据不同bpp，显示LCD某像素点(X,Y)
* 输入参数： int iX, int iY, unsigned int dwColor
             int iX: LCD屏幕上的X坐标
             int iY: LCD屏幕上的Y坐标
             int dwColor: RGB颜色值:0x00RRGGBB
* 输出参数： int
　 * 返 回 值： 0-成功  -1-失败 
   * 其它说明： 无
* 修改日期           版本号      修改人         修改内容
 * -----------------------------------------------
* 2020/05/9       V1.0    XXXX        XXXX
***********************************************************************/

static int FBShowPixel(int iX, int iY, unsigned int dwColor)
{
    unsigned char*  pucPen8pp   = NULL;
	unsigned short* pwPen16pp   = NULL;
	unsigned int*   pdwPen32pp  = NULL;
	unsigned short  wColor16bpp;   /* 565 */
	unsigned int ired;
    unsigned int igreen;
    unsigned int iblue;
    if ((iX >= g_tFBVar.xres) || (iY >= g_tFBVar.yres))
    {
        DBG_PRINTF("out of region\n");
        return -1;
    }
    pucPen8pp  = g_pucFbMem + iY*g_dwLineWidth + iX*g_dwPixelWidth;
	pwPen16pp  = (unsigned short*)pucPen8pp;
	pdwPen32pp = (unsigned int*)pucPen8pp;
	switch(g_tFBVar.bits_per_pixel)
	{
		case 8:
		{
			*pucPen8pp = (unsigned char)dwColor;
			break;
		}
		case 16:
		{
			ired   = (dwColor >> (16+3)) & 0x1f;
			igreen = (dwColor >> (8+2)) & 0x3f;
			iblue  = (dwColor >> 3) & 0x1f;
			wColor16bpp = ((ired << 11) | (igreen << 5) | iblue);
			*pwPen16pp  = wColor16bpp;
			break;
		}
		case 32:
		{
			*pdwPen32pp = dwColor;
			break;
		}
		default:
		{
			DBG_PRINTF("can't surport %dbpp\n",g_tFBVar.bits_per_pixel);
			return -1;
		}
	}
    return 0;
}
/**********************************************************************
* 函数名称： FBCleanScreen
* 功能描述： 将LCD清屏为某种颜色
* 输入参数： unsigned int dwBackColor: RGB颜色值:0x00RRGGBB
* 输出参数： int
　 * 返 回 值： 0-成功  -1-失败 
   * 其它说明： 无
* 修改日期           版本号      修改人         修改内容
 * -----------------------------------------------
* 2020/05/9       V1.0    XXXX        XXXX
***********************************************************************/

static int FBCleanScreen(unsigned int dwBackColor)
{
    unsigned char  *pucFB;
	unsigned short *pwFB16bpp;
	unsigned int   *pdwFB32bpp;
	unsigned short  wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;
	int i = 0;
    pucFB      = g_pucFbMem;
	pwFB16bpp  = (unsigned short *)pucFB;
	pdwFB32bpp = (unsigned int *)pucFB;
    switch (g_tFBVar.bits_per_pixel)
	{
		case 8:
		{
			memset(g_pucFbMem, dwBackColor, g_dwScreenSize);
			break;
		}
		case 16:
		{
			iRed   = (dwBackColor >> (16+3)) & 0x1f;
			iGreen = (dwBackColor >> (8+2)) & 0x3f;
			iBlue  = (dwBackColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			while (i < g_dwScreenSize)
			{
				*pwFB16bpp	= wColor16bpp;
				pwFB16bpp++;
				i += 2;
			}
			break;
		}
		case 32:
		{
			while (i < g_dwScreenSize)
			{
				*pdwFB32bpp	= dwBackColor;
				pdwFB32bpp++;
				i += 4;
			}
			break;
		}
		default :
		{
			DBG_PRINTF("can't support %d bpp\n", g_tFBVar.bits_per_pixel);
			return -1;
		}
	}
	return 0;
}
/**********************************************************************
* 函数名称： FBInit
* 功能描述： 调用disp_manager.c提供的注册函数进行初始化
* 输入参数： void
* 输出参数： int
　 * 返 回 值： 注册函数的返回值 
   * 其它说明： 无
* 修改日期           版本号      修改人         修改内容
 * -----------------------------------------------
* 2020/05/9       V1.0    XXXX        XXXX
***********************************************************************/

int FBInit(void)
{
	return RegisterDispOpr(&g_tFBOpr);
}





