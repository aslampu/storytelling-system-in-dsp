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
#include 	"mug.3.h"
#include    "QDMA2.h"
#include	"vm3224k.h"
#include    <math.h>
//#include    <stdio.h>

#pragma 	DATA_SECTION ( ary2_imgCamera,".sdram" )
#pragma 	DATA_SECTION ( ary3_yuv2rgbTable,".sdram" )
#pragma 	DATA_SECTION ( ary2_rgb2labTable,".sdram" )

short				ary2_imgCamera[240][320];
unsigned short 		ary2_imgFrame[240][320]; 
unsigned short     	ary3_yuv2rgbTable[64][32][32];
unsigned short		ary2_rgb2hsvTable[65536][3];
short   	  	    ary2_rgb2labTable[65536][3];
int					ary2_rotationTable[360][4];
/* Adjust */
int decideLAB_apply = 0;
unsigned short ary2_imgInputModified[200][200];
/* Adjust */

unsigned short 	rhThreshold = 0;
unsigned short	rhBias = 36;				
unsigned short 	rLowerBound = 150;
unsigned short 	rUpperBound = 10000;
unsigned short 	rQuantifiedLevel = 10;
unsigned short 	rBoxPadding = 5;		
unsigned short 	rBoxBorder = 2;			

unsigned short	ghThreshold = 110;//0.3055
unsigned short	ghBias = 36;		
unsigned short 	gLowerBound = 100;
unsigned short  gUpperBound = 10000;
unsigned short  gQuantifiedLevel = 10;
unsigned short  gBoxPadding = 5;			
unsigned short  gBoxBorder = 2;				

unsigned short 	bhThreshold = 234;//0.65;
unsigned short	bhBias = 36;			
unsigned short 	bLowerBound = 100;
unsigned short 	bUpperBound = 10000;
unsigned short 	bQuantifiedLevel = 10;
unsigned short 	bBoxPadding = 5;			
unsigned short 	bBoxBorder = 2;

float			backgroundAvgL = 0;
float 			backgroundAvgA = 0;
float			backgroundAvgB = 0;
float			backgroundStdL = 0;
float 			backgroundStdA = 0;
float			backgroundStdB = 0;

float           weightingS = 22;
float			weightingD = 27;

//int				backgroundAvgL = 0;
//int 			backgroundAvgA = 0;
//int				backgroundAvgB = 0;
//int				backgroundStdL = 0;
//int 			backgroundStdA = 0;
//int				backgroundStdB = 0;

//int		        weightingS = 22;
//int				weightingD = 27;
unsigned short 	displacementThreshold = 50;
int				imgSizeScale = 100;
float 			noiseVariance = 0.001;

int				shadowA = 83;
int				shadowB = 50;
int				shadowAOffset = 45;  // Value is offset by -50 in implementation -> Offset [-50,50]
int				shadowBOffset = 45;  // Value is offset by -50 in implementation -> Offset [-50,50]

int				rValue = 1;
int				gValue = 1;
int				bValue = 1;
int				rotationPrecisionScale = 100;
int				quantifiedPrecision = 10000;

void main()
{
	//Initialize
	int	i=-1, j=-1, k=-1, y0=-1, y1=-1, v0=-1, u0=-1;
	Filter gFilter, bFilter, combinedFilter;
	int imgSize = 0;
	int rotatedAngle = 0;
	int L,a,b;
	int labMugNumber = 0;
	int avgMugL = 0;
	int avgMugA = 0;
	int avgMugB = 0;
	int stdMugL = 0;
	int stdMugA = 0;
	int stdMugB = 0;
	int xTrackCenter = XLCD / 2;
	int yTrackCenter = YLCD / 2;
	int trackRange = YLCD / 2;
	//FILE *outputRGBData;
	//int ok=0;

	//PLL6713();	// Initialize C6713 PLL	
	//CE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPE);
	ACE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPEA);//For C6416
	vm3224init();
	vm3224rate(3);
	vm3224bl(15);
	
    for (k=0;k<64;k++)
    	for (i=0;i<32;i++)
    		for (j=0;j<32;j++)
    			ary3_yuv2rgbTable[k][i][j] = ybr_565(k<<2,i<<3,j<<3);

	// Generates the roation look-up table
	for (i = 0; i < 360; ++i) {
		ary2_rotationTable[i][0] = cos(i * 3.1415926 / 180) * rotationPrecisionScale;
		ary2_rotationTable[i][1] = -sin(i * 3.1415926 / 180) * rotationPrecisionScale;
		ary2_rotationTable[i][2] = sin(i * 3.1415926 / 180) * rotationPrecisionScale;
		ary2_rotationTable[i][3] = cos(i * 3.1415926 / 180) * rotationPrecisionScale;
	}

    for (j=0;j<NUM_RGB;j++){ 
		RGB2HSV(j,&(ary2_rgb2hsvTable[j][0]),&(ary2_rgb2hsvTable[j][1]),&(ary2_rgb2hsvTable[j][2]));
		RGB2Lab(j,&(ary2_rgb2labTable[j][0]),&(ary2_rgb2labTable[j][1]),&(ary2_rgb2labTable[j][2]));
	}

	QDMA_CNT 	= (239<<16)|320;
	QDMA_IDX 	= 0x0000<<16;
	
	for(j=0; j<HEIGHT; j++){
		for(i=0; i<WIDTH;i++){
			//compute input images' Lab
			if(ary2_imgNine[j][i] != 65535){
				labMugNumber++;
				L = ary2_rgb2labTable[ary2_imgNine[j][i]][0];
				a = ary2_rgb2labTable[ary2_imgNine[j][i]][1];
				b = ary2_rgb2labTable[ary2_imgNine[j][i]][2];
			}else{
				L = 0;
				a = 0;
				b = 0;
			}

			avgMugL += L;
			avgMugA += a;
			avgMugB += b;
			stdMugL += L * L;
			stdMugA += a * a;
			stdMugB += b * b;			
		}
	}

	avgMugL = avgMugL/labMugNumber;
	avgMugA = avgMugA/labMugNumber;
	avgMugB = avgMugB/labMugNumber;
	stdMugL = sqrt(stdMugL/labMugNumber - avgMugL * avgMugL);
	stdMugA = sqrt(stdMugA/labMugNumber - avgMugA * avgMugA);
	stdMugB = sqrt(stdMugB/labMugNumber - avgMugB * avgMugB);

	//InitializeFilter(rColor, &rFilter);
	InitializeFilter(gColor, &gFilter);
	InitializeFilter(bColor, &bFilter);
	
	//Read input video
	while (1) {
		/*outputRGBData = fopen("C:/CCStudio_v3.1/MCHproj/MixedReality0404/MixedReality/outputRGBData.raw", "wb");
		if(!outputRGBData)
			printf("Cannot open outputRGBData");*/
		
		//Get Input Frames C6416 version
		for(i=0;i<1000000;i++) if(EDMA_CIPRL&0x200) break;		
		VM3224ADDH = 0x08000;		
		for(i=0;i<200;i++);		
		EDMA_CIPRL = 0x200;

		//for(i=0;i<1000000;i++) if(EDMA_CIPR&0x200) break;		
		//VM3224ADDH = 0x08000;		
		//for(i=0;i<200;i++);		
		//EDMA_CIPR = 0x200;	
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
		TrackBall2D(xTrackCenter, yTrackCenter, trackRange, &gFilter, &bFilter, ary2_imgFrame, ary2_rgb2hsvTable, ary2_rgb2labTable);

		//Compute Rotation and choose the coresponding image
		//Resize Image
		//Labequalize Image
		switch(gFilter.ballFound * 2 + bFilter.ballFound){
			case 0: // found nothing
				xTrackCenter = XLCD/2;
				yTrackCenter = YLCD/2;
				trackRange = YLCD/2;
				break;
			case 1: //only find blue one
				xTrackCenter = 2 * bFilter.xCenter-xTrackCenter;
				yTrackCenter = 2 * bFilter.yCenter-yTrackCenter;
				trackRange = sqrt(bFilter.ballSize);                                                           			
				imgSize = Guard((trackRange / bFilter.quantifiedLevel) * bFilter.quantifiedLevel, 30,100) * imgSizeScale / 50;
				bFilter.scaleFactor = imgSize;

				/*Adjust*/
				if(decideLAB_apply == 1){
					applyLAB(avgMugL, avgMugA, avgMugB, stdMugL, stdMugA, stdMugB, ary2_imgNine, ary2_imgInputModified, ary2_rgb2labTable);
				}
			
				scaleImage(imgSize, ary2_imgInputModified, ary2_imgInput, ary2_rotationTable[0]);
				/*Adjust*/
							
				DrawShadow1D(&bFilter, ary2_imgFrame);
				
				OverlayImage1D(&bFilter, ary2_imgFrame, ary2_imgInput);
				break;
			case 2://only find green
				xTrackCenter = gFilter.xCenter;
				yTrackCenter = gFilter.yCenter;
				trackRange = sqrt(gFilter.ballSize);
				imgSize = Guard((trackRange / gFilter.quantifiedLevel) * gFilter.quantifiedLevel, 30,100) * imgSizeScale / 50;
				gFilter.scaleFactor = imgSize;

				/*Adjust*/
				if(decideLAB_apply == 1){
					applyLAB(avgMugL, avgMugA, avgMugB, stdMugL, stdMugA, stdMugB, ary2_imgNine, ary2_imgInputModified, ary2_rgb2labTable);
				}

				scaleImage(imgSize, ary2_imgInputModified, ary2_imgInput, ary2_rotationTable[0]);
				/*Adjust*/
			
				DrawShadow1D(&gFilter, ary2_imgFrame);
				
				OverlayImage1D(&gFilter,ary2_imgFrame, ary2_imgInput);
				break;
			case 3://find both green and blue
				xTrackCenter = (bFilter.xCenter + gFilter.xCenter) / 2;
				yTrackCenter = (bFilter.yCenter + gFilter.yCenter) / 2;
				rotatedAngle = (180 * quantifiedPrecision / 31416) * Guard(atan2((bFilter.xCenter-gFilter.xCenter) * quantifiedPrecision,(bFilter.yCenter-gFilter.yCenter) * quantifiedPrecision) * quantifiedPrecision,-62915,62915) / quantifiedPrecision;
				if(rotatedAngle < 0)
					rotatedAngle += 360;			   
				trackRange = sqrt(bFilter.ballSize) + sqrt(gFilter.ballSize);
				imgSize = Guard((trackRange / gFilter.quantifiedLevel) * gFilter.quantifiedLevel, 30,100) * imgSizeScale / 50;	
				combinedFilter.scaleFactor = imgSize;
				combinedFilter.ballFound = 1;
		 		combinedFilter.ballColor = 100;// means no color
				combinedFilter.ballSize = trackRange;
				combinedFilter.xCenter = xTrackCenter;
				combinedFilter.yCenter = yTrackCenter;
				combinedFilter.hThreshold = 0;
				combinedFilter.hBias = 0;
				combinedFilter.lowerBound = 0;
				//combinedFilter.upperBound = 0;
				combinedFilter.quantifiedLevel = 0;
				combinedFilter.boxPadding = 0;
				//combinedFilter.boxBorder = 0;

				/*Adjust*/
				if(decideLAB_apply == 1){
					applyLAB(avgMugL, avgMugA, avgMugB, stdMugL, stdMugA, stdMugB, ary2_imgNine, ary2_imgInputModified, ary2_rgb2labTable);
				}

				scaleImage(imgSize, ary2_imgInputModified, ary2_imgInput, ary2_rotationTable[rotatedAngle]);
				/*Adjust*/

				DrawShadow1D(&combinedFilter, ary2_imgFrame);
				OverlayImage1D(&combinedFilter, ary2_imgFrame, ary2_imgInput);
				break;	
			default:
				;
		}

		//Output Synthesized Frames: C6416 version
		for(i=0;i<1000000;i++) if(EDMA_CIPRL&0x200) break;
		VM3224ADDH = 0x0000;		
		EDMA_CIPRL = 0x200;	

		//for(i=0;i<1000000;i++) if(EDMA_CIPR&0x200) break;
		//VM3224ADDH = 0x0000;		
		//EDMA_CIPR = 0x200;			
		QDMA_SRC	= (int)ary2_imgFrame;
		QDMA_DST 	= (int)&VM3224DATA;		
		QDMA_S_OPT 	= OptionField_1;
		
		/*if(ok){
			for (j=0;j<XLCD;j++)
			for (i=0;i<YLCD;i++) {
				fputc( (int)(((ary2_imgFrame[j][i]&0xF800)>>11) / 31.0 * 255.0), outputRGBData);
				fputc( (int)(((ary2_imgFrame[j][i]&0x7E0)>>5) / 63.0 * 255.0), outputRGBData);
				fputc( (int)(((ary2_imgFrame[j][i]&0x1F)) / 31.0 * 255.0), outputRGBData);		
				//fprintf(outputHueData, "%f ", ary2_rgb2hsvTable[ary2_imgFrame[j][i]][0]);
				//fprintf(outputHueData, "%f ", ary2_rgb2hsvTable[ary2_imgFrame[j][i]][1]);
				//fprintf(outputHueData, "%f ", ary2_rgb2hsvTable[ary2_imgFrame[j][i]][2]);
			}
		}
		fclose(outputRGBData);*/
	}
}

