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

#include    "Utility.h"
#include    "InputImage.h"
#include    "TeaPot.h"
#include    "AcrylicPaint.h"
#include    "QDMA.h"
#include	"vm3224k.h"
#include    <stdio.h>

#pragma 	DATA_SECTION ( ary2_imgCamera,".sdram" )
#pragma 	DATA_SECTION ( ary2_imgFrame,".sdram" )
#pragma 	DATA_SECTION ( ary2_imgOne,".sdram" )
#pragma 	DATA_SECTION ( ary2_imgTwo,".sdram" )
#pragma 	DATA_SECTION ( ary3_yuv2rgbTable,".sdram" )

short				ary2_imgCamera[XLCD][YLCD];
unsigned short 		ary2_imgFrame[XLCD][YLCD]; 
unsigned short     	ary3_yuv2rgbTable[64][32][32];
unsigned short		ary3_rgb2hsvTable[NUM_RGB][3];
//int  	     		ary2_rgb2labTable[NUM_RGB][3];

void main()
{
	//Initialize
	int	i=-1, j=-1, k=-1, y0=-1, y1=-1, v0=-1, u0=-1;
	FILE *outputRGBData, *outputHueData;
	int ok=0;
	Filter rFilter, gFilter, bFilter;

	PLL6713();	// Initialize C6713 PLL	
	CE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPE);
	vm3224init();
	vm3224rate(3);
	vm3224bl(15);
	
    for (k=0;k<64;k++)
    for (i=0;i<32;i++)
    for (j=0;j<32;j++) ary3_yuv2rgbTable[k][i][j] = ybr_565(k<<2,i<<3,j<<3);


	/*RGB2HSV(j,&(ary3_rgb2hsvTable[0][0]),&(ary3_rgb2hsvTable[0][1]),&(ary3_rgb2hsvTable[0][2]));
	RGB2HSV(j,&(ary3_rgb2hsvTable[j][0]),&(ary3_rgb2hsvTable[j][1]),&(ary3_rgb2hsvTable[j][2]));
	RGB2HSV(j,&(ary3_rgb2hsvTable[j][0]),&(ary3_rgb2hsvTable[j][1]),&(ary3_rgb2hsvTable[j][2]));
	RGB2HSV(j,&(ary3_rgb2hsvTable[j][0]),&(ary3_rgb2hsvTable[j][1]),&(ary3_rgb2hsvTable[j][2]));
	RGB2HSV(j,&(ary3_rgb2hsvTable[j][0]),&(ary3_rgb2hsvTable[j][1]),&(ary3_rgb2hsvTable[j][2]));
	RGB2HSV(j,&(ary3_rgb2hsvTable[65535][0]),&(ary3_rgb2hsvTable[65535][1]),&(ary3_rgb2hsvTable[65535][2]));*/

    for (j=0;j<NUM_RGB;j++){ 
    	//ary2_rgb2hueTable[j] = RGB2Hue(j);
		RGB2HSV(j,&(ary3_rgb2hsvTable[j][0]),&(ary3_rgb2hsvTable[j][1]),&(ary3_rgb2hsvTable[j][2]));
		//RGB2Lab(j,&(ary2_rgb2labTable[j][0]),&(ary2_rgb2labTable[j][1]),&(ary2_rgb2labTable[j][2]));
	}

	QDMA_CNT 	= (239<<16)|320;
	QDMA_IDX 	= 0x0000<<16;
	
	InitializeFilter(rColor, &rFilter);
	InitializeFilter(gColor, &gFilter);
	InitializeFilter(bColor, &bFilter);

	//Read input video
	while (1) {
		outputRGBData = fopen("C:/CCStudio_v3.1/MCHproj/MixedReality0404/MixedReality/outputRGBData.raw", "wb");
		if(!outputRGBData)
			printf("Cannot open outputRGBData");
		/*outputHueData = fopen("C:/CCStudio_v3.1/MCHproj/MixedReality0404/MixedReality/outputHueData.raw", "wb");
		if(!outputHueData)
			printf("Cannot open outputHueData");*/
		
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
			ary2_imgFrame[j][i] = ary3_yuv2rgbTable[y0][u0][v0];
			ary2_imgFrame[j][i+1] = ary3_yuv2rgbTable[y1][u0][v0];
		}
		
		//Call track function, which modify the ary2_imgFrame array passed by a pointer
		
		TrackBall(&rFilter, ary2_imgFrame, ary3_rgb2hsvTable);
		TrackBall(&bFilter, ary2_imgFrame, ary3_rgb2hsvTable);

		switch(rFilter.ballFound * 4 + gFilter.ballFound * 2 + bFilter.ballFound){
			case 1: //only find blue one
				OverlayImage1D(&bFilter, ary2_imgFrame, ary2_imgTwo);
				break;		
			case 4: //only find green one
				OverlayImage1D(&rFilter, ary2_imgFrame, ary2_imgTwo);
				break;
			case 5: //find both green and blue ones
				OverlayImage2D(&rFilter, &bFilter, ary2_imgFrame, ary2_imgTwo);	
				break;
			default:
				;
		}

		//Output Synthesized Frames
		for(i=0;i<1000000;i++) if(EDMA_CIPR&0x200) break;
		VM3224ADDH = 0x0000;		
		EDMA_CIPR = 0x200;			
		QDMA_SRC	= (int)ary2_imgFrame;
		QDMA_DST 	= (int)&VM3224DATA;		
		QDMA_S_OPT 	= OptionField_1;
		
		if(ok){
			for (j=0;j<XLCD;j++)
			for (i=0;i<YLCD;i++) {
				fputc( (int)(((ary2_imgFrame[j][i]&0xF800)>>11) / 31.0 * 255.0), outputRGBData);
				fputc( (int)(((ary2_imgFrame[j][i]&0x7E0)>>5) / 63.0 * 255.0), outputRGBData);
				fputc( (int)(((ary2_imgFrame[j][i]&0x1F)) / 31.0 * 255.0), outputRGBData);
				//fputc( (int)(ary3_rgb2hsvTable[ary2_imgFrame[j][i]][0]), outputHueData);
				//fputc( (int)(ary3_rgb2hsvTable[ary2_imgFrame[j][i]][1]), outputHueData);
				//fputc( (int)(ary3_rgb2hsvTable[ary2_imgFrame[j][i]][2]), outputHueData);			
				//fprintf(outputHueData, "%d ", ary3_rgb2hsvTable[ary2_imgFrame[j][i]][0]);
				//fprintf(outputHueData, "%d ", ary3_rgb2hsvTable[ary2_imgFrame[j][i]][1]);
				//fprintf(outputHueData, "%d ", ary3_rgb2hsvTable[ary2_imgFrame[j][i]][2]);
			}
		}
		fclose(outputRGBData);
		//fclose(outputHueData);
	}
}

