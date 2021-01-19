/*
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *
 * S5PC110 - LCD Driver for U-Boot
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/types.h>

#define LCD_BGCOLOR		0x1428A0
#define g_offset		20

static unsigned int gFgColor = 0xFF;
static unsigned int gLeftColor = LCD_BGCOLOR;

#define Inp32(_addr)		readl(_addr)
#define Outp32(addr, data)	(*(volatile u32 *)(addr) = (data))
#define Delay(_a)		udelay(_a*1000)

#include <regs.h>

/*
    GPH0_4 - led1
    GPH0_5 - led2 
    GPH0_6 - led3 
*/

#define LED_GPIO_ADDRESS      (0xE0200C00)    //GPH0
void LED_Set(int on)
{
    Outp32(LED_GPIO_ADDRESS,(Inp32(LED_GPIO_ADDRESS)  &= (~0xFFFF0000)));
    Outp32(LED_GPIO_ADDRESS,(Inp32(LED_GPIO_ADDRESS)  |= 0x11110000));

//    Outp32(LED_GPIO_ADDRESS+4,(Inp32(LED_GPIO_ADDRESS+4) &= (~0x000000F0)));
    Outp32(LED_GPIO_ADDRESS+4,(Inp32(LED_GPIO_ADDRESS+4) |= 0x000000F0));
}


#define PWM_GPIO_ADDRESS      (0xE02000A0)    //GPD0
#define PWM_GPIO_BIT          (2)     		  //GPD0.2
#define LCDM_PWM_HIGH         Outp32(PWM_GPIO_ADDRESS+4,(Inp32(PWM_GPIO_ADDRESS+4) |= (1<<PWM_GPIO_BIT)))

#define MODE_GPIO_ADDRESS     (0xE0200C00)    //GPH0
#define MODE_GPIO_BIT         (7)     		  //GPH0.7
#define LCDM_MODE_HIGH        Outp32(MODE_GPIO_ADDRESS+4,(Inp32(MODE_GPIO_ADDRESS+4) |= (1<<MODE_GPIO_BIT)))

#define LCD_WIDTH       800
#define LCD_HEIGHT      480

void Init_LTE480WV_backlight(void)
{
#if 0
/*
	GPD0_2 - pwm 
	GPH0_7 - mode 
*/

    int temp;

    Delay(10);

	LCDM_PWM_HIGH;
	
	LCDM_MODE_HIGH;	
#endif
}

void LCD_Initialize_LTE480WV(void)
{
#if 0	
    u32 uFbAddr = CFG_LCD_FBUFFER;

    u32 i,uTmpReg;
    u32* pBuffer = (u32*)uFbAddr;

	//printf("LCD_Initialize_LTE480WV\n");
    uTmpReg = Inp32(0xE0107008);        // need to be changed later (09.01.23 WJ.Kim)   
    uTmpReg = (uTmpReg & ~(0x3)) | 0x2 ;
    Outp32(0xE0107008, uTmpReg);

    uTmpReg = Inp32(0xf800027c);
    uTmpReg = (uTmpReg & ~(0x3)) | 0 ;
    Outp32(0xf800027c,uTmpReg);

    Outp32(0xE0200120, 0x22222222); //set GPF0 as LVD_HSYNC,VSYNC,VCLK,VDEN,VD[3:0]
    Outp32(0xE0200128,0x0);         //set pull-up,down disable
    Outp32(0xE0200140, 0x22222222); //set GPF1 as VD[11:4]
    Outp32(0xE0200148,0x0);         //set pull-up,down disable
    Outp32(0xE0200160, 0x22222222); //set GPF2 as VD[19:12]
    Outp32(0xE0200168,0x0);         //set pull-up,down disable
    Outp32(0xE0200180, 0x00002222); //set GPF3 as VD[23:20]
    Outp32(0xE0200188,0x0);         //set pull-up,down disable

    Outp32(0xe02000A0, 0x11111111); //set GPF0 as LVD_HSYNC,VSYNC,VCLK,VDEN,VD[3:0]
    Outp32(0xe02000A8,0x0);         //set pull-up,down disable
    Outp32(0xe02000A4,(1<<3));

    Outp32(0xE0200040, 0x11111111); //set GPF0 as LVD_HSYNC,VSYNC,VCLK,VDEN,VD[3:0]
    Outp32(0xE0200048,0x0);         //set pull-up,down disable
    Outp32(0xE0200044,0);

    Outp32(0xE02000c0, 0x11111111); //set GPF0 as LVD_HSYNC,VSYNC,VCLK,VDEN,VD[3:0]
    Outp32(0xE02000c8,0x0);         //set pull-up,down disable
    Outp32(0xE02000c4,0);

    Outp32(0xE0200C00, 0x11111111); //set GPF0 as LVD_HSYNC,VSYNC,VCLK,VDEN,VD[3:0]
    Outp32(0xE0200C08,0x0);         //set pull-up,down disable
    Outp32(0xE0200C04,0);
    Delay(50);
    //Outp32(0xE0200C04,0xffffffff);
    Outp32(0xE0200C04,0xffffffff & ~((1<<4)|(1<<5)));

    Outp32(0xf8000004, 0x60);
    Outp32(0xf8000010, 0xe0e0305);
    Outp32(0xf8000014, 0x3103020);
    Outp32(0xf8000170, 0x0);
//    Outp32(0xf8000018, 0x17fd55);
    Outp32(0xf8000018, 0xefb1f );
    Outp32(0xf8000000, 0x0);
    Outp32(0xf8000000, 0x254);
    Outp32(0xf8000130, 0x20);
    Outp32(0xf8000020, 0x0);
    Outp32(0xf8000024, 0x0);
    Outp32(0xf8000028, 0x0);
    Outp32(0xf800002c, 0x0);
    Outp32(0xf8000030, 0x0);
    Outp32(0xf8000034, 0x0);
    Outp32(0xf8000180, 0x0);
    Outp32(0xf8000184, 0x0);
    Outp32(0xf8000188, 0x0);
    Outp32(0xf800018c, 0x0);
    Outp32(0xf8000190, 0x0);
    Outp32(0xf8000140, 0x0);
    Outp32(0xf8000148, 0x0);
    Outp32(0xf8000150, 0x0);
    Outp32(0xf8000158, 0x0);
    Outp32(0xf8000058, 0x0);
    Outp32(0xf8000208, 0x0);
    Outp32(0xf800020c, 0x0);
    Outp32(0xf8000068, 0x0);
    Outp32(0xf8000210, 0x0);
    Outp32(0xf8000214, 0x0);
    Outp32(0xf8000078, 0x0);
    Outp32(0xf8000218, 0x0);
    Outp32(0xf800021c, 0x0);
    Outp32(0xf8000088, 0x0);
    Outp32(0xf8000220, 0x0);
    Outp32(0xf8000224, 0x0);
    Outp32(0xf8000260, 0x1);
    Outp32(0xf8000048, 0x100200);
    Outp32(0xf8000200, 0xffffff);
    Outp32(0xf8000204, 0xffffff);
    Outp32(0xf8000034, 0x0);
    Outp32(0xf8000020, 0x802c);

    Outp32(0xf80000a0, uFbAddr + 0x00000000);
    Outp32(0xf80000d0, uFbAddr + 0x00400800);
    Outp32(0xf80000a4, uFbAddr + 0x00000000);
    Outp32(0xf80000d4, uFbAddr + 0x00400800);
    Outp32(0xf80020a0, uFbAddr + 0x00000000);
    Outp32(0xf80020d0, uFbAddr + 0x00400800);

    Outp32(0xf8000100, 0x1558);
    Outp32(0xf8000040, 0x0);
    Outp32(0xf8000044, 0x2aaaff);
    Outp32(0xf8000020, 0x802d);
    Outp32(0xf8000034, 0x1);
    Outp32(0xf8000020, 0x802d);
    Outp32(0xf8000034, 0x1);
    Outp32(0xf8000000, 0x257);
//    Outp32(0xf8000000, 0x57); //===> MPLL should be 667 !!!!

    Outp32(0xf80001a4, 0x3);

    Outp32(0xe0107008,0x2); //syscon output path
    Outp32(0xe0100204,0x700000); //syscon fimdclk = mpll

//	Init_LTE480WV_backlight();
#endif
}

/*
 */
void LCD_turnon(void)
{
	LCD_Initialize_LTE480WV();
}

void LCD_setfgcolor(unsigned int color)
{
	gFgColor = color;
}

void LCD_setleftcolor(unsigned int color)
{
	gLeftColor = color;
}

u32 temp_i;
u32 temp_j;

void LCD_setprogress(int percentage)
{
#if 0
	u32 i, j;
	//u32* pBuffer = (u32*)CFG_LCD_FBUFFER;
	u16* pBuffer = (u32*)CFG_LCD_FBUFFER;

	for (i=0; i < (LCD_HEIGHT/100)*percentage; i++)
	{
		for (j=0; j < LCD_WIDTH; j++)
		{
			*pBuffer++ = gFgColor;
		}
	}

/*
	for (; i < LCD_HEIGHT; i++)
	{
		for (j=0; j < (LCD_WIDTH >> 5); j++)
		{
			*pBuffer++ = gLeftColor;
		}
		for (; j < LCD_WIDTH; j++)
		{
			*pBuffer++ = LCD_BGCOLOR;
		}
	}
*/
#endif
}

void LCD_bg_display(unsigned char *p)
{
#if 0
    u32 i, j;
	u16 cnt =0;
	
    u32* pBuffer = (u32*)CFG_LCD_FBUFFER;

/*
    for (i=0; i < (LCD_HEIGHT/3) - g_offset; i++) 
    {
        for (j=0; j < LCD_WIDTH; j++) 
        {
            *pBuffer++ = 0x000000FF;
        }
    }

    for (i=(LCD_HEIGHT/3) ; i < (LCD_HEIGHT/3)*2; i++) 
    {
        for (j=0; j < LCD_WIDTH; j++)
        {
            *pBuffer++ = 0x00FFFFFF;
        }
    }

    for (i=(LCD_HEIGHT/3)*2; i < LCD_HEIGHT; i++) 
    {
        for (j=0; j < LCD_WIDTH; j++) 
        {
            *pBuffer++ = 0x00FF0000;
        }
    }
*/
    //Delay(100);

    for (i=0; i < (LCD_HEIGHT/2) - g_offset; i++) 
    {
        for (j=0; j < LCD_WIDTH; j++) 
        {
            *pBuffer++ = 0x00FFFFFF;
        }
    }

    for (i=(LCD_HEIGHT/2) - g_offset; i < (LCD_HEIGHT/2) + g_offset; i++) 
    {
        for (j=0; j < LCD_WIDTH; j++) 
        {
			*pBuffer++ = (p[cnt+3]<<24) + (p[cnt+2]<<16) + (p[cnt+1]<<8) + p[cnt];
			cnt++;
        }
	}

    for (i=(LCD_HEIGHT/2)+g_offset; i < LCD_HEIGHT; i++) 
    {
        for (j=0; j < LCD_WIDTH; j++) 
        {
            *pBuffer++ = 0x00FFFFFF;
        }
    }
#endif
}
