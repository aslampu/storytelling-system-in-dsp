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
#include	"mug.1.h"
//#include    "mug2.1.h"
#include    "mug2.2.h"
#include    "QDMA.h"
#include	"vm3224k.h"
#include    <stdio.h>
#include    <math.h>

#pragma 	DATA_SECTION ( ary2_imgCamera,".sdram" )
#pragma 	DATA_SECTION ( ary2_imgFrame,".sdram" )
//#pragma 	DATA_SECTION ( ary2_imgOne,".sdram" )
//#pragma 	DATA_SECTION ( ary2_imgTwo,".sdram" )
//#pragma 	DATA_SECTION ( ary3_yuv2rgbTable,".sdram" )

short				ary2_imgCamera[XLCD][YLCD];
unsigned short 		ary2_imgFrame[XLCD][YLCD]; 
unsigned short     	ary3_yuv2rgbTable[64][32][32];
float				ary2_rgb2hsvTable[NUM_RGB][3];
//unsigned short		ary_hsv2rgbTable[NUM_RGB];
short   	  	    ary2_rgb2labTable[NUM_RGB][3];
//unsigned short  	ary_lab2rgbTable[NUM_RGB];
float				ary2_rotationTable[360][4];


/* Adjust */
int decideLAB_apply = 0;
unsigned short ary2_imgInputModified[200][200];
/* Adjust */

float			PI = 3.1415926;
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

float           weightingS = 14;
float			weightingD = 26;
unsigned short 	displacementThreshold = 50;
//int				imgSizeScale = 20;
int imgSizeScale = 0;
//int				randomNoise100;
float noiseVariance = 0.005;


float			rotatedAngle = 0;
int				rValue = 1;
int				gValue = 1;
int				bValue = 1;

int				shadowA = 94;
int				shadowB = 62;
int				shadowAOffset = 42;  // Value is offset by -50 in implementation -> Offset [-50,50]
int				shadowBOffset = 47;  // Value is offset by -50 in implementation -> Offset [-50,50]

void main()
{
	//Initialize
	int	i=-1, j=-1, k=-1, y0=-1, y1=-1, v0=-1, u0=-1;
	Filter rFilter, gFilter, bFilter, combinedFilter;
	//short check=0;
	//int imgSizeScale = 0;
	int imgSize = 0;
	//int imgSize1 = 100, imgSize4 = 100, imgSize5 = 100;
	//int tmpSize1 = 0, tmpSize4  = 0, tmpSize5 = 0;
	int labTeaPotNumber = 0;
	int L,a,b;
	float avgTeaPotL = 0;
	float avgTeaPotA = 0;
	float avgTeaPotB = 0;
	float stdTeaPotL = 0;
	float stdTeaPotA = 0;
	float stdTeaPotB = 0;
	int labAcrylicPaintNumber = 0;
	float avgAcrylicPaintL = 0;
	float avgAcrylicPaintA = 0;
	float avgAcrylicPaintB = 0;
	float stdAcrylicPaintL = 0;
	float stdAcrylicPaintA = 0;
	float stdAcrylicPaintB = 0;
	int xTrackCenter = XLCD / 2;
	int yTrackCenter = YLCD / 2;
	int trackRange = YLCD / 2;
	FILE *outputRGBData;
	int ok=0;

	//FILE *outputRGBData, *outputHueData,*outputLData, *outputaData, *outputbData;
	//int ok=0, check = 0;
	//unsigned int testLAB;
	//int tmpL,tmpA,tmpB;
	//short scaleFactor100=100;

	PLL6713();	// Initialize C6713 PLL	
	CE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPE);
	vm3224init();
	vm3224rate(3);
	vm3224bl(15);
	
    for (k=0;k<64;k++)
    for (i=0;i<32;i++)
    for (j=0;j<32;j++) ary3_yuv2rgbTable[k][i][j] = ybr_565(k<<2,i<<3,j<<3);
	
	/*outputLData = fopen("C:/CCStudio_v3.1/MCHproj/MixedReality0404/MixedReality/outputLData.dat", "wb");
	if(!outputLData) printf("Cannot open outputLData");
	outputaData = fopen("C:/CCStudio_v3.1/MCHproj/MixedReality0404/MixedReality/outputaData.dat", "wb");
	if(!outputaData) printf("Cannot open outputLData");
	outputbData = fopen("C:/CCStudio_v3.1/MCHproj/MixedReality0404/MixedReality/outputbData.dat", "wb");
	if(!outputbData) printf("Cannot open outputbData");*/


	// Generates the roation look-up table
	for (i = 0; i < 360; ++i) {
		ary2_rotationTable[i][0] = cos(i * PI / 180);
		ary2_rotationTable[i][1] = -sin(i * PI / 180);
		ary2_rotationTable[i][2] = sin(i * PI / 180);
		ary2_rotationTable[i][3] = cos(i * PI / 180);
	}

    for (j=0;j<NUM_RGB;j++){ 
		RGB2HSV(j,&(ary2_rgb2hsvTable[j][0]),&(ary2_rgb2hsvTable[j][1]),&(ary2_rgb2hsvTable[j][2]));
		//ary_hsv2rgbTable[j] = HSV2RGB(ary2_rgb2hsvTable[j][0], ary2_rgb2hsvTable[j][1], ary2_rgb2hsvTable[j][2]);
		RGB2Lab(j,&(ary2_rgb2labTable[j][0]),&(ary2_rgb2labTable[j][1]),&(ary2_rgb2labTable[j][2]));
		//ary_lab2rgbTable[j] = Lab2RGB(((0&0x0ff)<<24|(ary2_rgb2labTable[j][0]&0x0ff)<<16|(ary2_rgb2labTable[j][1]&0x0ff)<<8|ary2_rgb2labTable[j][2]&0x0ff));
		//ary_rgb2labTable[j] = RGB2Lab(j);
		/*if((ary_rgb2labTable[j] & 0x00800000) >> 23)//negative
			check = (ary_rgb2labTable[j] | 0xffffffffff00ffff) >>16;
			//fprintf(outputLData, "%d: %d\n", j, (int)((ary_rgb2labTable[j] | 0xffffffffff00ffff) >>16));
		else
			//check = (ary_rgb2labTable[j] & 0x00ff0000) >>16;
			fprintf(outputLData, "%d: %d\n", j, (int)((ary_rgb2labTable[j] & 0x00ff0000) >>16));
		if((ary_rgb2labTable[j] & 0x00008000) >> 15)//negative
			//check = (ary_rgb2labTable[j] | 0xffffffffffff00ff) >>8;
			fprintf(outputaData, "%d: %d\n", j, (int)((ary_rgb2labTable[j] | 0xffffffffffff00ff) >>8));
		else
			//check = (ary_rgb2labTable[j] & 0x0000ff00) >>8;
			fprintf(outputaData, "%d: %d\n", j, (int)((ary_rgb2labTable[j] & 0x0000ff00) >>8));
		if((ary_rgb2labTable[j] & 0x00000080) >> 7)//negative
			//check = (ary_rgb2labTable[j] | 0xffffffffffffff00);
			fprintf(outputbData, "%d: %d\n", j, (int)((ary_rgb2labTable[j] | 0xffffffffffffff00)));
		else
			//check = (ary_rgb2labTable[j] & 0x000000ff);
			fprintf(outputbData, "%d: %d\n", j, (int)((ary_rgb2labTable[j] & 0x000000ff)));
		*/
	}

	QDMA_CNT 	= (239<<16)|320;
	QDMA_IDX 	= 0x0000<<16;
	
	
	for(j=0; j<HEIGHT; j++){
		for(i=0; i<WIDTH;i++){
			//compute input images' Lab
			//if(ary2_imgEleven[j][i] != 65535){
			if(ary2_imgEleven[j][i] != 65535){
				labTeaPotNumber++;
				L = ary2_rgb2labTable[ary2_imgEleven[j][i]][0];
				a = ary2_rgb2labTable[ary2_imgEleven[j][i]][1];
				b = ary2_rgb2labTable[ary2_imgEleven[j][i]][2];
			}else{
				L = 0;
				a = 0;
				b = 0;
			}

			avgTeaPotL += L;
			avgTeaPotA += a;
			avgTeaPotB += b;
			stdTeaPotL += L * L;
			stdTeaPotA += a * a;
			stdTeaPotB += b * b;	
			
			//if(ary2_imgEleven[j][i] != 65535){
			if(ary2_imgEleven[j][i] != 65535){
				labAcrylicPaintNumber++;
				L = ary2_rgb2labTable[ary2_imgEleven[j][i]][0];
				a = ary2_rgb2labTable[ary2_imgEleven[j][i]][1];
				b = ary2_rgb2labTable[ary2_imgEleven[j][i]][2];
			}else{
				L = 0;
				a = 0;
				b = 0;
			}

			avgAcrylicPaintL += L;
			avgAcrylicPaintA += a;
			avgAcrylicPaintB += b;
			stdAcrylicPaintL += L * L;
			stdAcrylicPaintA += a * a;
			stdAcrylicPaintB += b * b;		
		}
	}

	avgTeaPotL = avgTeaPotL/labTeaPotNumber;
	avgTeaPotA = avgTeaPotA/labTeaPotNumber;
	avgTeaPotB = avgTeaPotB/labTeaPotNumber;
	stdTeaPotL = sqrt(stdTeaPotL/labTeaPotNumber - avgTeaPotL * avgTeaPotL);
	stdTeaPotA = sqrt(stdTeaPotA/labTeaPotNumber - avgTeaPotA * avgTeaPotA);
	stdTeaPotB = sqrt(stdTeaPotB/labTeaPotNumber - avgTeaPotB * avgTeaPotB);

	avgAcrylicPaintL = avgAcrylicPaintL/labAcrylicPaintNumber;
	avgAcrylicPaintA = avgAcrylicPaintA/labAcrylicPaintNumber;
	avgAcrylicPaintB = avgAcrylicPaintB/labAcrylicPaintNumber;
	stdAcrylicPaintL = sqrt(stdAcrylicPaintL/labAcrylicPaintNumber - avgAcrylicPaintL * avgAcrylicPaintL);
	stdAcrylicPaintA = sqrt(stdAcrylicPaintA/labAcrylicPaintNumber - avgAcrylicPaintA * avgAcrylicPaintA);
	stdAcrylicPaintB = sqrt(stdAcrylicPaintB/labAcrylicPaintNumber - avgAcrylicPaintB * avgAcrylicPaintB);	

	InitializeFilter(rColor, &rFilter);
	InitializeFilter(gColor, &gFilter);
	InitializeFilter(bColor, &bFilter);
	
	//fclose(outputLData);
	//fclose(outputaData);
	//fclose(outputbData);
	
	//Read input video

	while (1) {
		//randomNoise100 = rand() % 100;
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
		//TrackBall(&gFilter, ary2_imgFrame, ary2_rgb2hsvTable, ary2_rgb2labTable);
		//TrackBall(&bFilter, ary2_imgFrame, ary2_rgb2hsvTable, ary2_rgb2labTable);
		TrackBall2D(xTrackCenter, yTrackCenter, trackRange, &gFilter, &bFilter, ary2_imgFrame, ary2_rgb2hsvTable, ary2_rgb2labTable);
		//DebugBall(&rFilter, ary2_imgFrame, ary2_rgb2hsvTable);
		//DebugBall(&bFilter, ary2_imgFrame, ary2_rgb2hsvTable);

		//Compute Rotation and choose the coresponding image
		//Resize Image
		//Labequalize Image
		switch(gFilter.ballFound * 2 + bFilter.ballFound){
			case 0: // found nothing
				xTrackCenter = XLCD/2;
				yTrackCenter = YLCD/2;
				trackRange = YLCD/2;

				//-----Adjust
				/*
				for(i = 0; i < 200; i++){
					for(j = 0; j < 200; j++){
					
						if(ary2_imgEleven[i][j] != 0)
							ary2_imgFrame[30+i][40+j] = ary2_imgEleven[i][j];
					
					}
				}
				*/
				//-----Adjust



				break;
			case 1: //only find blue one
				//tmpSize1 = imgSize1;
				//xTrackCenter = floor(2 * bFilter.xCenter-xTrackCenter);
				//yTrackCenter = floor(2 * bFilter.yCenter-yTrackCenter);
				xTrackCenter = floor(bFilter.xCenter);
				yTrackCenter = floor(bFilter.yCenter);
				trackRange = floor(sqrt(bFilter.ballSize));
				imgSize = Guard((trackRange / bFilter.quantifiedLevel) * bFilter.quantifiedLevel, 30,100)* imgSizeScale / 50;
				bFilter.scaleFactor = imgSize;
				//if(tmpSize1 != imgSize1)

				/*Adjust*/
				if(decideLAB_apply == 1){
					applyLAB(avgTeaPotL, avgTeaPotA, avgTeaPotB, stdTeaPotL, stdTeaPotA, stdTeaPotB,ary2_imgEleven, ary2_imgInputModified, ary2_rgb2labTable);
				}
			
				//scaleImage(imgSize, ary2_imgEleven, ary2_imgInput);
				scaleImage(imgSize, ary2_imgInputModified, ary2_imgInput, ary2_rotationTable[0]);
				/*Adjust*/
			
				
				DrawShadow1D(&bFilter, ary2_imgFrame);
				
				//OverlayImage1D(avgTeaPotL, avgTeaPotA, avgTeaPotB, stdTeaPotL, stdTeaPotA, stdTeaPotB, &bFilter, ary2_imgFrame, ary2_imgInput, ary2_rgb2labTable);
				OverlayImage1D(&bFilter, ary2_imgFrame, ary2_imgInput);

				//OverlayImage1D(&bFilter, ary2_imgFrame, ary2_imgSeven);
				//DrawShadow1D(&bFilter, ary2_imgFrame);
				break;
			case 2://only find green
				xTrackCenter = floor(gFilter.xCenter);
				yTrackCenter = floor(gFilter.yCenter);
				trackRange = floor(sqrt(gFilter.ballSize));
				imgSize = Guard((trackRange / gFilter.quantifiedLevel) * gFilter.quantifiedLevel, 30,100)* imgSizeScale / 50;	
				gFilter.scaleFactor = imgSize;


				/*Adjust*/
				if(decideLAB_apply == 1){
					applyLAB(avgTeaPotL, avgTeaPotA, avgTeaPotB, stdTeaPotL, stdTeaPotA, stdTeaPotB,ary2_imgEleven, ary2_imgInputModified, ary2_rgb2labTable);
				}

				//scaleImage(imgSize, ary2_imgEleven, ary2_imgInput);
				scaleImage(imgSize, ary2_imgInputModified, ary2_imgInput, ary2_rotationTable[0]);
				/*Adjust*/


			

				
				DrawShadow1D(&gFilter, ary2_imgFrame);
				
				//OverlayImage1D(avgAcrylicPaintL, avgAcrylicPaintA, avgAcrylicPaintB, stdAcrylicPaintL, stdAcrylicPaintA, stdAcrylicPaintB, &gFilter, ary2_imgFrame, ary2_imgInput, ary2_rgb2labTable);
				OverlayImage1D(&gFilter,ary2_imgFrame, ary2_imgInput);
				//OverlayImage1D(&rFilter, ary2_imgFrame, ary2_imgFive);
				//DrawShadow1D(&gFilter, ary2_imgFrame);
				break;
			case 3://find both green and blue
				xTrackCenter = floor((bFilter.xCenter + gFilter.xCenter) / 2);
				yTrackCenter = floor((bFilter.yCenter + gFilter.yCenter) / 2);
				trackRange = floor(sqrt(bFilter.ballSize) + sqrt(gFilter.ballSize));
				rotatedAngle = (180 / PI) * Guard(atan2((bFilter.xCenter-gFilter.xCenter),(bFilter.yCenter-gFilter.yCenter)),-2 * PI,2 * PI);
				if(rotatedAngle < 0)
					rotatedAngle += 360;
				imgSize = Guard((trackRange / gFilter.quantifiedLevel) * gFilter.quantifiedLevel, 30,100)* imgSizeScale / 50;	
				//imgSize = Min(100, floor(((bFilter.ballSize - bFilter.lowerBound) / bFilter.upperBound) * bFilter.quantifiedLevel) * bFilter.quantifiedLevel + floor((rFilter.ballSize - rFilter.lowerBound) / rFilter.upperBound) * rFilter.quantifiedLevel) * rFilter.quantifiedLevel)
				imgSize = imgSize * 3.0 / 4;
				
				combinedFilter.scaleFactor = imgSize;
				combinedFilter.ballFound = 1;
		 		combinedFilter.ballColor = -1;
				combinedFilter.ballSize = trackRange;
				combinedFilter.xCenter = xTrackCenter;
				combinedFilter.yCenter = yTrackCenter;
				combinedFilter.hThreshold = 0;
				combinedFilter.hBias = 0;
				combinedFilter.lowerBound = 0;
				combinedFilter.upperBound = 0;
				combinedFilter.quantifiedLevel = 0;
				combinedFilter.boxPadding = 0;
				combinedFilter.boxBorder = 0;

				/*Adjust*/
				if(decideLAB_apply == 1){
					applyLAB(avgTeaPotL, avgTeaPotA, avgTeaPotB, stdTeaPotL, stdTeaPotA, stdTeaPotB,ary2_imgEleven, ary2_imgInputModified, ary2_rgb2labTable);
				}
				//scaleImage(imgSize, ary2_imgEleven, ary2_imgInput);
				
				scaleImage(imgSize, ary2_imgInputModified, ary2_imgInput, ary2_rotationTable[(int)rotatedAngle]);
				/*Adjust*/
				DrawShadow1D(&combinedFilter, ary2_imgFrame);
				OverlayImage1D(&combinedFilter,ary2_imgFrame, ary2_imgInput);

				/*Need rewrite OverlayImage2D()*/
				
				//OverlayImage2D(&gFilter, &bFilter, ary2_imgFrame, ary2_imgInput);	
				//OverlayImage1D(ary2_imgFrame, ary2_imgInput);
				//OverlayImage2D(&rFilter, &bFilter, ary2_imgFrame, ary2_imgSeven);
				break;	
			/*case 4: //only find red one
				imgSize = Min(100, (rFilter.quantifiedLevel * (rFilter.ballSize - rFilter.lowerBound) / rFilter.upperBound) * rFilter.quantifiedLevel + 50);
				rFilter.scaleFactor = imgSize;
				scaleImage(imgSize, ary2_imgEight, ary2_imgInput);
				
				DrawShadow1D(&rFilter, ary2_imgFrame);
				
				
				OverlayImage1D(avgAcrylicPaintL, avgAcrylicPaintA, avgAcrylicPaintB, stdAcrylicPaintL, stdAcrylicPaintA, stdAcrylicPaintB, &rFilter, ary2_imgFrame, ary2_imgInput, ary2_rgb2labTable);
				//OverlayImage1D(&rFilter, ary2_imgFrame, ary2_imgFive);
				//DrawShadow1D(&rFilter, ary2_imgFrame);
				break;
			case 5: //find both red and blue ones
				//imgSize = Min(100, floor(((bFilter.ballSize - bFilter.lowerBound) / bFilter.upperBound) * bFilter.quantifiedLevel) * bFilter.quantifiedLevel + floor((rFilter.ballSize - rFilter.lowerBound) / rFilter.upperBound) * rFilter.quantifiedLevel) * rFilter.quantifiedLevel)/2;
				imgSize = 100;
				scaleImage(imgSize, ary2_imgSeven, ary2_imgInput);
				OverlayImage2D(&rFilter, &bFilter, ary2_imgFrame, ary2_imgInput);	
				//OverlayImage2D(&rFilter, &bFilter, ary2_imgFrame, ary2_imgSeven);
				break;*/
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
				//fprintf(outputHueData, "%f ", ary2_rgb2hsvTable[ary2_imgFrame[j][i]][0]);
				//fprintf(outputHueData, "%f ", ary2_rgb2hsvTable[ary2_imgFrame[j][i]][1]);
				//fprintf(outputHueData, "%f ", ary2_rgb2hsvTable[ary2_imgFrame[j][i]][2]);
			}
		}
		fclose(outputRGBData);
		//fclose(outputHueData);
	}
}

