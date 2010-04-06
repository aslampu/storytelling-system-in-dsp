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
unsigned short		ary2_rgb2hsvTable[NUM_RGB][3];
int  	     		ary2_rgb2labTable[NUM_RGB][3];
unsigned short  	ary3_lab2rgbTable[NUM_LAB][NUM_LAB][NUM_LAB];

float 			rhThreshold = 0;
float 			rhBias = 36;				
unsigned short 	rLowerBound = 500;
unsigned short 	rUpperBound = 40000;
unsigned short 	rQuantifiedLevel = 10;
unsigned short 	rBoxPadding = 100;		
unsigned short 	rBoxBorder = 2;			

float 			ghThreshold = 110;			
float 			ghBias = 40;			
unsigned short 	gLowerBound = 500;
unsigned short  gUpperBound = 40000;
unsigned short  gQuantifiedLevel = 10;
unsigned short  gBoxPadding = 100;			
unsigned short  gBoxBorder = 2;				

float 			bhThreshold = 234;		
float 			bhBias = 36;			
unsigned short 	bLowerBound = 500;
unsigned short 	bUpperBound = 40000;
unsigned short 	bQuantifiedLevel = 10;
unsigned short 	bBoxPadding = 100;			
unsigned short 	bBoxBorder = 2;	

void main()
{
	//Initialize
	int	i=-1, j=-1, k=-1, y0=-1, y1=-1, v0=-1, u0=-1;
	FILE *outputRGBData, *outputHueData;
	int ok=0;
	Filter rFilter, gFilter, bFilter;
	short scaleFactor100=100;

	PLL6713();	// Initialize C6713 PLL	
	CE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPE);
	vm3224init();
	vm3224rate(3);
	vm3224bl(15);
	
    for (k=0;k<64;k++)
    for (i=0;i<32;i++)
    for (j=0;j<32;j++) ary3_yuv2rgbTable[k][i][j] = ybr_565(k<<2,i<<3,j<<3);

    for (j=0;j<NUM_RGB;j++){ 
    	//ary2_rgb2hueTable[j] = RGB2Hue(j);
		RGB2HSV(j,&(ary2_rgb2hsvTable[j][0]),&(ary2_rgb2hsvTable[j][1]),&(ary2_rgb2hsvTable[j][2]));
		RGB2Lab(j,&(ary2_rgb2labTable[j][0]),&(ary2_rgb2labTable[j][1]),&(ary2_rgb2labTable[j][2]));
	}

	for (k=0;k<NUM_LAB;k++)
    for (i=0;i<NUM_LAB;i++)
    for (j=0;j<NUM_LAB;j++) ary3_lab2rgbTable[k][i][j] = Lab2RGB(k,i,j);	

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
		TrackBall(&rFilter, ary2_imgFrame, ary2_rgb2hsvTable);
		TrackBall(&bFilter, ary2_imgFrame, ary2_rgb2hsvTable);

		//Compute Rotation and choose the coresponding image
		//Resize Image
		//Labequalize Image
		switch(rFilter.ballFound * 4 + gFilter.ballFound * 2 + bFilter.ballFound){
			case 1: //only find blue one
				scaleImage(scaleFactor100, ary2_imgTwo, ary2_imgInput);
				OverlayImage1D(&bFilter, ary2_imgFrame, ary2_imgInput);
				break;		
			case 4: //only find green one
				scaleImage(scaleFactor100, ary2_imgOne, ary2_imgInput);
				OverlayImage1D(&rFilter, ary2_imgFrame, ary2_imgInput);
				break;
			case 5: //find both green and blue ones
				scaleImage(scaleFactor100, ary2_imgTwo, ary2_imgInput);
				OverlayImage2D(&rFilter, &bFilter, ary2_imgFrame, ary2_imgInput);	
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
				//fprintf(outputHueData, "%d ", ary3_rgb2hsvTable[ary2_imgFrame[j][i]][0]);
				//fprintf(outputHueData, "%d ", ary3_rgb2hsvTable[ary2_imgFrame[j][i]][1]);
				//fprintf(outputHueData, "%d ", ary3_rgb2hsvTable[ary2_imgFrame[j][i]][2]);
			}
		}
		fclose(outputRGBData);
		//fclose(outputHueData);
	}
}

