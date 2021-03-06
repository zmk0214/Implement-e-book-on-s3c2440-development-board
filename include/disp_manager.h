#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H
#include <config.h>


typedef struct DispOpr{
    char *name;
    int iXres;
    int iYres;
    int iBpp;
    int (*DeviceInit)(void);
    int (*ShowPixel)(int iPenX, int iPenY, unsigned int dwColor);
    int (*CleanScreen)(unsigned int dwBkgdColor);
    struct DispOpr *ptNext;
}T_DispOpr, *PT_DispOpr;

int RegisterDispOpr(PT_DispOpr ptDispOpr);
void ShowDispOpr(void);
int DisplayInit(void);
int FBInit(void);

#endif /* _DISP_MANAGER_H */

