/* ======================================================================== */
/*            Copyright (C) 2008 - 2009 ND-Tech. Co., Ltd.                  */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
/* Project  : LogicalOperation                  Created  : 03-MAR-2009      */
/* File     : main.c                            Contact  : info@nd-tech.com */
/* ======================================================================== */
/* You are free to use or modify this code to the following restrictions:   */
/*          Acknowledge ND Tech. Co. Ltd.                                   */
/*          Or, put "Parts of code by ND Tech. Co., Ltd."                   */
/*          Or, leave this header as it is.                                 */
/* in somewhere in your code.                                               */
/* ======================================================================== */

#include	"vm3224k.h"
#include	"images.h"

#define		CE2CTL		*(volatile int *)(0x01800010)
// Definitions for async access(change as you wish)
#define		WSU			(2<<28)	 // Write Setup : 0-15
#define		WST			(8<<22)  // Write Strobe: 0-63
#define		WHD			(2<<20)	 // Write Hold  : 0-3
#define		RSU			(2<<16)	 // Read Setup  : 0-15
#define		TA			(3<<14)	 // Turn Around : 0-3
#define		RST			(8<<8)	 // Read Strobe : 0-63
#define		RHD			(2<<0)	 // Read Hold   : 0-3
#define		MTYPE		(2<<4)

/* EDMA Registers */
#define		PaRAM_OPT	0	// Options
#define		PaRAM_SRC	1	// Source Address
#define		PaRAM_CNT	2	// Frame count, Element count
#define		PaRAM_DST	3	// Destination Address
#define		PaRAM_IDX	4	// Frame index, Element index
#define		PaRAM_RDL	5	// Element count reload, Link address
#define		EDMA_CIPR	*(volatile int *)0x01A0FFE4	// EDMA Channel interrupt pending low register
#define		EDMA_CIER	*(volatile int *)0x01A0FFE8	// EDMA Channel interrupt enable low register
#define		EDMA_ER		*(volatile int *)0x01A0FFF0	// EDMA Event low register
#define		EDMA_EER	*(volatile int *)0x01A0FFF4	// EDMA Event enable low register
#define		EDMA_ECR	*(volatile int *)0x01A0FFF8	// EDMA Event clear low register
#define		EDMA_ESR	*(volatile int *)0x01A0FFFC	// EDMA Event set low register

#define 	PRI			(2<<29)	// 1:High priority, 2:Low priority
#define 	ESIZE		(1<<27)	// 0:32bit, 1:16bit, 2:8bit, 3:reserved
#define 	DS2			(0<<26)	// 1:2-Dimensional
#define 	SUM			(0<<24)	// 0:no update, 1:increment, 2:decrement, 3:by index
#define 	DD2			(1<<23)	// 1:2-Dimensional
#define 	DUM			(1<<21)	// 0:no update, 1:increment, 2:decrement, 3:by index
#define 	TCINT		(1<<20)	// 0:disable, 1:enable
#define 	TCC			(9<<16)	// 4 bit code
#define 	LINK		(0<<1)	// 0:disable, 1:enable
#define 	FS			(1<<0)	// 0:element, 1:frame
#define 	OptionField_0 (PRI|ESIZE|DS2|SUM|DD2|DUM|TCINT|TCC|LINK|FS)

#define 	DD2_1		(0<<23)	// 1:2-Dimensional
#define 	DUM_1		(0<<21)	// 0:no update, 1:increment, 2:decrement, 3:by index
#define 	DS2_1		(1<<26)	// 1:2-Dimensional
#define 	SUM_1		(1<<24)	// 0:no update, 1:increment, 2:decrement, 3:by index
#define 	TCC_1		(10<<16)// 4 bit code
#define 	OptionField_1 (PRI|ESIZE|DS2_1|SUM_1|DD2_1|DUM_1|TCINT|TCC_1|LINK|FS)

#pragma 	DATA_SECTION ( lcd,".sdram" )
#pragma 	DATA_SECTION ( cam,".sdram" )
short		lcd[240][320];
short		cam[240][320];
short		rgb[64][32][32];
int 		*Event;

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

 	b = y + 1772*(u-128)/1000;
	if (b<0) b=0;
	if (b>255) b=255;
	g = y - (344*(u-128) + 714*(v-128))/1000;
	if (g<0) g=0;
	if (g>255) g=255;
	r = y + 1402*(v-128)/1000;
	if (r<0) r=0;
	if (r>255) r=255;
	return ((r&0x0f8)<<8)|((g&0x0fc)<<3)|((b&0x0f8)>>3);
}

void main()
{
	int 	i,j,k,m,n,x,y,y0,y1,v0,u0;
	PLL6713();			// Initialize C6713 PLL	
	CE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPE);
	vm3224init();       // Initialize vm3224k2
	vm3224rate(3);		// Set frame rate
	vm3224bl(15);       // Set backlight

	for (k=0;k<64;k++)  // Create RGB565 lookup table
    for (i=0;i<32;i++)
    for (j=0;j<32;j++) rgb[k][i][j] = ybr_565(k<<2,i<<3,j<<3);

	Event			 = (int *)(0x01a00000 + 24 * 9); 
	Event[PaRAM_OPT] = OptionField_0;  	   
	Event[PaRAM_SRC] = (int)&VM3224DATA; // Source address
	Event[PaRAM_CNT] = (239<<16)|320;	 // 240 frames, 320 elements 
	Event[PaRAM_DST] = (int)cam;  		 // Destination address
	Event[PaRAM_IDX] = 0<<16;            // Address index 0			
	
	Event 			 = (int *)(0x01a00000 + 24 * 10);
	Event[PaRAM_OPT] = OptionField_1;  				
	Event[PaRAM_SRC] = (int)lcd;			
	Event[PaRAM_CNT] = (239<<16) | 320;
	Event[PaRAM_DST] = (int)&VM3224DATA; 
	Event[PaRAM_IDX] = 0<<16;

    m=n=0;
	x=y=2;
	while (1) {
		for(i=0;i<1000000;i++) 
			if(EDMA_CIPR&0x400) break;   // Waiting for EDMA channel 10 transfer complete
		VM3224ADDH = 0x08000;            // Select Cam screen buffer and Set address (1<<15|0x00000)	
		for(i=0; i<100; i++);		
		EDMA_CIPR = 0x200;               // Clear CIP9
		EDMA_ESR = EDMA_ESR|0x200;       // Start channel 9 EDMA transfer
		
		for (j=0;j<240;j++)
		for (i=0;i<320;i+=2) {
			y0 = (cam[j][i]>>8) & 0x0ff;
			u0 = cam[j][i] & 0x0ff;
			y1 = (cam[j][i+1]>>8) & 0x0ff;
			v0 = cam[j][i+1] & 0x0ff;
			y0 = y0>>2;
			y1 = y1>>2;
			u0 = u0>>3;
			v0 = v0>>3;
			lcd[j][i]=rgb[y0][u0][v0];
			lcd[j][i+1]=rgb[y1][u0][v0];
		}

		if ((m+x>240-BALL_SIZE)||(m+x<0)) x = -x;
		if ((n+y>320-BALL_SIZE)||(n+y<0)) y = -y;
		m += x;
		n += y;

		for (j=0;j<52;j++)
		for (i=0;i<52;i++) lcd[j+m][i+n] &= ball[j][i];			
		
		for(i=0;i<1000000;i++) 
			if(EDMA_CIPR&0x200) break;   // Waiting for EDMA channel 9 transfer complete
		VM3224ADDH = 0;                  // Select LCD screen buffer and Set address (0<<15|0x00000)
		EDMA_CIPR  = 0x400;              // Clear CIP10
		EDMA_ESR   = EDMA_ESR|0x400;     // Start channel 10 EDMA transfer	
	}
}
