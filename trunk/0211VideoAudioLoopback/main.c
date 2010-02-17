/* ======================================================================== */
/*            Copyright (C) 2008 - 2009 ND-Tech. Co., Ltd.                  */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
/* Project  : VideoAudioLoopback                Created  : 03-MAR-2009      */
/* File     : main.c                            Contact  : info@nd-tech.com */
/* ======================================================================== */
/* You are free to use or modify this code to the following restrictions:   */
/*          Acknowledge ND Tech. Co. Ltd.                                   */
/*          Or, put "Parts of code by ND Tech. Co., Ltd."                   */
/*          Or, leave this header as it is.                                 */
/* in somewhere in your code.                                               */
/* ======================================================================== */

#include	"c6000.h"
#include	"dsxio.h"
#include	"vm3224k.h"
#include	"dsk6713.h"
#include	"dsk6713_aic23.h"
//#include    "log.h"

// Definitions for async access(change as you wish)
#define		WSU			(2<<28)	 // Write Setup : 0-15
#define		WST			(8<<22)  // Write Strobe: 0-63
#define		WHD			(2<<20)	 // Write Hold  : 0-3
#define		RSU			(2<<16)	 // Read Setup  : 0-15
#define		TA			(3<<14)	 // Turn Around : 0-3
#define		RST			(8<<8)	 // Read Strobe : 0-63
#define		RHD			(2<<0)	 // Read Hold   : 0-3
#define		MTYPEA		(2<<4)

#pragma 	DATA_SECTION ( lcd,".sdram" )
#pragma 	DATA_SECTION ( cam,".sdram" )
#pragma 	DATA_SECTION ( past,".sdram" )
#pragma 	DATA_SECTION ( now,".sdram" )
#pragma 	DATA_SECTION ( future,".sdram" )     

short       past[240][320];
short       now[240][320];
short       future[240][320];
short		lcd[240][320];
short		cam[240][320];
short		rgb[64][32][32];
int			flag=0;
int         move=0;
int			stepIn=0;
unsigned short 		mask=0xffff;
DSK6713_AIC23_CodecHandle hCodec;
enum colorID {ALL_Color, R_Color, G_Color, B_Color};

extern cregister volatile unsigned int IER;
extern cregister volatile unsigned int CSR;
//extern LOG_Obj LOG_Message;

DSK6713_AIC23_Config config = {
    0x0017, // 0 DSK6713_AIC23_LEFTINVOL  Left line input channel volume
    0x0017, // 1 DSK6713_AIC23_RIGHTINVOL Right line input channel volume
    0x0079, // 2 DSK6713_AIC23_LEFTHPVOL  Left channel headphone volume
    0x0079, // 3 DSK6713_AIC23_RIGHTHPVOL Right channel headphone volume
    0x0015, // 4 DSK6713_AIC23_ANAPATH    Analog audio path control
    0x0000, // 5 DSK6713_AIC23_DIGPATH    Digital audio path control
    0x0000, // 6 DSK6713_AIC23_POWERDOWN  Power down control
    0x0043, // 7 DSK6713_AIC23_DIGIF      Digital audio interface format
    0x0001, // 8 DSK6713_AIC23_SAMPLERATE Sample rate control
    0x0001  // 9 DSK6713_AIC23_DIGACT     Digital interface activation
};

void PLL6713()
{
	int i;
	// CPU Clock Input : 50MHz
	
	*(volatile int *)(0x01b7c100) = *(volatile int *)(0x01b7c100) & 0xfffffffe;
	for(i=0;i<4;i++);
	*(volatile int *)(0x01b7c100) = *(volatile int *)(0x01b7c100) | 0x08;
	*(volatile int *)(0x01b7c114) = 0x08001;  // 50MHz/2 = 25MHz
 	*(volatile int *)(0x01b7c110) = 0x0c;     // 25MHz * 12 = 300MHz
	*(volatile int *)(0x01b7c118) = 0x08000;  // SYSCLK1 = 300MHz/1 = 300MHz
	*(volatile int *)(0x01b7c11c) = 0x08001;  // SYSCLK2 = 300MHz/2 = 150MHz // Peripheral Clock
	*(volatile int *)(0x01b7c120) = 0x08003;  // SYSCLK3 = 300MHz/4 =  75MHz // SDRAM Clock
	for(i=0;i<4;i++);
	*(volatile int *)(0x01b7c100) = *(volatile int *)(0x01b7c100) & 0xfffffff7;
	for(i=0;i<4;i++);
	*(volatile int *)(0x01b7c100) = *(volatile int *)(0x01b7c100) | 0x01;
}

unsigned short	ybr_565(short y,short u,short v)
{
	int		r,g,b;

 	b = y + 1.772*(u-128);
	if (b<0) b=0;
	if (b>255) b=255;
	g = y - 0.344*(u-128) - 0.714*(v-128);
	if (g<0) g=0;
	if (g>255) g=255;
	r = y + 1.402*(v-128);
	if (r<0) r=0;
	if (r>255) r=255;
	return ((r&0x0f8)<<8)|((g&0x0fc)<<3)|((b&0x0f8)>>3);
}

void intr_disable(int num)
{
	IER = IER & ~(1<<num);
}

void intr_select(int vector, int select)
{
	if(vector>INT9) {
		vector -= 32;
		MUXH = (MUXH & ~(0x1f<<vector)) | (select<<vector);
	}
	else
		MUXL = (MUXL & ~(0x1f<<vector)) | (select<<vector);
}

void intr_enable(int num)
{
	IER = IER | (1<<num) | 3;
	CSR = CSR | 0x1;
}

interrupt void	c_int06(void)
{
	flag = 1;
}

interrupt void	c_int15(void)
{
	DXR1 = DRR1;
}

void main()
{
	int i,j,k,y0,y1,v0,u0,DIP_Value;
	PLL6713();			// Initialize C6713 PLL	
	CE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPEA);

	intr_disable(6);
	intr_disable(15);
	intr_select(INT6,6);
	intr_select(INT15,RINT1);

	hCodec = DSK6713_AIC23_openCodec(0, &config);   // Set codec control registers and get codec handle

	vm3224init();       // Initialize vm3224k2
	vm3224rate(3);		// Set frame rate
	vm3224bl(15);       // Set backlight
	
	for (k=0;k<64;k++)  // Create RGB565 lookup table
    for (i=0;i<32;i++)
    for (j=0;j<32;j++) rgb[k][i][j] = ybr_565(k<<2,i<<3,j<<3);
	VM3224CNTL = VM3224CNTL&0xffff | 0x2;

	DSK6713_LED_init();
	DSK6713_DIP_init();

	intr_enable(6);
	intr_enable(15);

	while(1) {
		DIP_Value = !DSK6713_DIP_get(1)*2+!DSK6713_DIP_get(2);
		switch(DIP_Value){
			case ALL_Color:
				mask=0xffff;
				DSK6713_LED_off(1);
				DSK6713_LED_off(2);
				break;
			case R_Color:
				mask = 0xf800;
				DSK6713_LED_off(1);
				DSK6713_LED_on(2);
				break;
			case G_Color:
				mask = 0x07e0;
				DSK6713_LED_on(1);
				DSK6713_LED_off(2);
				break;
			case B_Color:
				mask = 0x001f;
				DSK6713_LED_on(1);
				DSK6713_LED_on(2);
				break;
			default://Something must be wrong
				DSK6713_LED_on(0);
				DSK6713_LED_on(1);
				DSK6713_LED_on(2);
				DSK6713_LED_on(3);
				//LOG_printf(&LOG_Message,"switch_case_exception: output value %d\n", DIP_Value);
		}

		if(DSK6713_DIP_get(0)){
			stepIn = 1;
			move = 1;
			DSK6713_LED_off(0);
			DSK6713_LED_off(3);
			// flag unchanged
		}else{//pressed:0 => stepIn
			DSK6713_LED_on(0);
			stepIn = !DSK6713_DIP_get(3);
			if(stepIn){
				DSK6713_LED_on(3);
			}else{
				move = 1;
				DSK6713_LED_off(3);
			}
		}
				
		if(stepIn && flag && move) {
			VM3224ADDH = 0x08000;	// Select Cam screen buffer and Set address (1<<15|0x00000)
			for (j=0;j<240;j++)
			for (i=0;i<320;i++) cam[j][i]=VM3224DATA;

			for (j=0;j<240;j++)
			for (i=0;i<320;i+=2) {  // Conversion yuv to RGB565
				y0 = (cam[j][i]>>8) & 0x0ff;
				u0 = cam[j][i] & 0x0ff;
				y1 = (cam[j][i+1]>>8) & 0x0ff;
				v0 = cam[j][i+1] & 0x0ff;
				y0 = y0>>2;
				y1 = y1>>2;
				u0 = u0>>3;
				v0 = v0>>3;
								 
				past[j][i]=now[j][i];
				past[j][i+1]=now[j][i+1];
								 
				now[j][i]=future[j][i];
				now[j][i+1]=future[j][i+1];
				     
				future[j][i] = rgb[y0][u0][v0];
				future[j][i+1] = rgb[y1][u0][v0];				
			}
			flag = 0;
			move = 0;
		}
		VM3224ADDH = 0;		    // Select LCD screen buffer and Set address (0<<15|0x00000)
		for (j=0;j<240;j++)
		for (i=0;i<320;i++) VM3224DATA=now[j][i] & mask;
	}
}
