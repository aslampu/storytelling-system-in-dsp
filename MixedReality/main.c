/* ======================================================================== */
/*            Copyright (C) 2008 - 2009 ND-Tech. Co., Ltd.                  */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
/* File     : main.c                            Created  : 13-Mar-2008      */
/* Version  : 2.0.0.0                           Contach  : info@nd-tech.com */
/* ======================================================================== */
/* You are free to use or modify this code to the following restrictions:   */
/*          Acknowledge ND Tech. Co. Ltd.                                   */
/*          Or, put "Parts of code by ND Tech. Co., Ltd."                   */
/*          Or, leave this header as it is.                                 */
/* in somewhere in your code.                                               */
/* ======================================================================== */

#include	"vm3224k.h"
#include    "qdma.h"
#include    "utility.h"

#pragma 	DATA_SECTION ( cam,".sdram" )
#pragma 	DATA_SECTION ( tmp,".sdram" )

short		rgb[64][32][32];
short		cam[XLCD][YLCD];
short 		tmp[XLCD][YLCD];

void main()
{
	//Initialize
	int	i=-1, j=-1, k=-1, y0=-1, y1=-1, v0=-1, u0=-1;
	Filter R_Former,R_Latter;
	//Filter G_Former, G_Latter;
	Filter B_Former, B_Latter;

	PLL6713();	// Initialize C6713 PLL	
	CE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPE);
	vm3224init();
	vm3224rate(3);
	vm3224bl(15);

    for (k=0;k<64;k++)
    for (i=0;i<32;i++)
    for (j=0;j<32;j++) rgb[k][i][j] = ybr_565(k<<2,i<<3,j<<3);

	QDMA_CNT 	= (239<<16)|320;
	QDMA_IDX 	= 0x0000<<16;
	
	//Read input video
	while (1) {
		//Get Input Frames
		for(i=0;i<1000000;i++) if(EDMA_CIPR&0x200) break;		
		VM3224ADDH = 0x08000;		
		for(i=0;i<200;i++);		
		EDMA_CIPR = 0x200;	
		QDMA_SRC	= (int)&VM3224DATA;
		QDMA_DST 	= (int)cam;			
		QDMA_S_OPT 	= OptionField_0;

		for (j=0;j<XLCD;j++)
		for (i=0;i<YLCD;i+=2){
			y0 = (cam[j][i]>>8) & 0x0ff;
			u0 = cam[j][i] & 0x0ff;
			y1 = (cam[j][i+1]>>8) & 0x0ff;
			v0 = cam[j][i+1] & 0x0ff;
			y0 = y0>>2;
			y1 = y1>>2;
			u0 = u0>>3;
			v0 = v0>>3;
			tmp[j][i]=rgb[y0][u0][v0];
			tmp[j][i+1]=rgb[y1][u0][v0];
		}

		//Call track function, which modified the tmp array passed by a pointer
		track(tmp, &R_Former, &R_Latter); //R_Color = 0
		//track(tmp, &G_Former, &G_Latter); //G_Color = 1
		track(tmp, &B_Former, &B_Latter); //B_Color = 2
		if(R_Latter.Found || B_Latter.Found)
			overlayImage2D(tmp, &R_Latter, &B_Latter);//Include resize and rotation
		
		//Output Synthesized Frames
		for(i=0;i<1000000;i++) if(EDMA_CIPR&0x200) break;
		VM3224ADDH = 0x0000;		
		EDMA_CIPR = 0x200;	
		QDMA_SRC	= (int)tmp;
		QDMA_DST 	= (int)&VM3224DATA;		
		QDMA_S_OPT 	= OptionField_1;				
	}
}

