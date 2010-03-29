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
#include    "QDMA.h"
#include    "Utility.h"

#pragma 	DATA_SECTION ( ary2_imgCamera,".sdram" )
#pragma 	DATA_SECTION ( ary2_imgFrame,".sdram" )

short		ary3_rgbTable[64][32][32];
short		ary2_imgCamera[XLCD][YLCD];
short 		ary2_imgFrame[XLCD][YLCD];

void main()
{
	//Initialize
	int	i=-1, j=-1, k=-1, y0=-1, y1=-1, v0=-1, u0=-1;
	Filter rFormerFilter,rLatterFilter;
	//Filter gFormerFilter, gLatterFilter;
	Filter bFormerFilter, bLatterFilter;

	PLL6713();	// Initialize C6713 PLL	
	CE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPE);
	vm3224init();
	vm3224rate(3);
	vm3224bl(15);

    for (k=0;k<64;k++)
    for (i=0;i<32;i++)
    for (j=0;j<32;j++) ary3_rgbTable[k][i][j] = ybr_565(k<<2,i<<3,j<<3);

	QDMA_CNT 	= (239<<16)|320;
	QDMA_IDX 	= 0x0000<<16;
	
	InitializeFilter(rColor, &rFormerFilter);
	InitializeFilter(rColor, &rLatterFilter);
	InitializeFilter(bColor, &bFormerFilter);
	InitializeFilter(bColor, &bLatterFilter);

	//Read input video
	while (1) {
		//Get Input Frames
		for(i=0;i<1000000;i++) if(EDMA_CIPR&0x200) break;		
		VM3224ADDH = 0x08000;		
		for(i=0;i<200;i++);		
		EDMA_CIPR = 0x200;	
		QDMA_SRC	= (int)&VM3224DATA;
		QDMA_DST 	= (int)ary2_imgCamera;			
		QDMA_S_OPT 	= OptionField_0;

		for (j=0;j<XLCD;j++)
		for (i=0;i<YLCD;i+=2){
			y0 = (ary2_imgCamera[j][i]>>8) & 0x0ff;
			u0 =  ary2_imgCamera[j][i] & 0x0ff;
			y1 = (ary2_imgCamera[j][i+1]>>8) & 0x0ff;
			v0 =  ary2_imgCamera[j][i+1] & 0x0ff;
			y0 =  y0>>2;
			y1 =  y1>>2;
			u0 =  u0>>3;
			v0 =  v0>>3;
			ary2_imgFrame[j][i] = ary3_rgbTable[y0][u0][v0];
			ary2_imgFrame[j][i+1] = ary3_rgbTable[y1][u0][v0];
		}
		
		//Call track function, which modify the ary2_imgFrame array passed by a pointer
		TrackBall(&rFormerFilter, &bLatterFilter, ary2_imgFrame);
		//TrackBall(&gFormerFilter, &gLatterFilter, ary2_imgFrame);
		TrackBall(&bFormerFilter, &bLatterFilter, ary2_imgFrame);
		if(rLatterFilter.ballFound || bLatterFilter.ballFound)
		//Include resize and rotation
		//OverlayImage2D(&rLatterFilter, &bLatterFilter, ary2_imgFrame);
		
		//Output Synthesized Frames
		for(i=0;i<1000000;i++) if(EDMA_CIPR&0x200) break;
		VM3224ADDH = 0x0000;		
		EDMA_CIPR = 0x200;	
		QDMA_SRC	= (int)ary2_imgFrame;
		QDMA_DST 	= (int)&VM3224DATA;		
		QDMA_S_OPT 	= OptionField_1;				
	}
}

