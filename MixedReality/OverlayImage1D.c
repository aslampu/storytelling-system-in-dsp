#include <math.h>
#include "Utility.h"

//void OverlayImage1D(float imgAvgL, float imgAvgA, float imgAvgB, float imgStdL, float imgStdA, float imgStdB, Filter *ptr_theFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary2_imgInput[HEIGHT][WIDTH], short	ary2_rgb2labTable[NUM_RGB][3]){
void OverlayImage1D(Filter *ptr_theFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary2_imgInput[HEIGHT][WIDTH]){
	
	/*int i,j;
	int adjustedLValue, adjustedAValue, adjustedBValue;
	float sL,sA,sB,dL,dA,dB;
	//int k,m;
	//unsigned short tmpColor,tmpRed,tmpGreen,tmpBlue;
	//int comLength = 10 * floor((70 + (210/75000) * (ptr_theFilter->ballSize-5000)) / 10);
	*/
	/*int xStart = Min(XLCD, Max(0,floor(ptr_theFilter->xCenter-40)));
	int yStart = Min(YLCD, Max(0,floor(ptr_theFilter->yCenter-40)));
	//DrawBox(ptr_theFilter->xFrom, ptr_theFilter->xTo, ptr_theFilter->yFrom, ptr_theFilter->yTo, ptr_theFilter->boxBorder, ptr_theFilter->boxPadding, ary2_imgFrame);
	for(j=40;j<160;j++){
		if(xStart+j-40 >= XLCD)
			break;
		for(i=30;i<180;i++){
			if(yStart+i-30 >= YLCD)
				break;
			tmpColor = ary2_imgInput[j][i];
			if(((tmpColor & 0xf800) >> 11) < 28 && ((tmpColor & 0x07e0) >> 5) < 58 && (tmpColor & 0x001f) < 28)
				ary2_imgFrame[xStart+j-40][yStart+i-30] = tmpColor;
		}
	}*/

	/*
	int xStart = Min(XLCD, floor(ptr_theFilter->xCenter-100));
	int yStart = Min(YLCD, floor(ptr_theFilter->yCenter-100));
	//DrawBox(ptr_theFilter->xFrom, ptr_theFilter->xTo, ptr_theFilter->yFrom, ptr_theFilter->yTo, ptr_theFilter->boxBorder, ptr_theFilter->boxPadding, ary2_imgFrame);
	for(j=0;j<HEIGHT;j++){
		if(xStart+j >= XLCD || xStart+j < 0)
			continue;
		for(i=0;i<WIDTH;i++){
			if(yStart+i >= YLCD || yStart+i < 0)
				continue;
			//tmpColor = ary2_imgInput[j][i];
			//tmpRed = (tmpColor & 0xf800) >> 11;
			//tmpGreen = (tmpColor & 0x07e0) >> 5;
			//tmpBlue = tmpColor & 0x001f;
			//if(tmpRed != 31 && tmpGreen != 63 && tmpBlue != 31){//not pure white
			if(ary2_imgInput[j][i] != 0){	
				//adjustedLValue = (backgroundStdL / imgStdL) * (ary2_rgb2labTable[ary2_imgInput[j][i]][0] - imgAvgL) + backgroundAvgL;
				//adjustedAValue = (backgroundStdA / imgStdA) * (ary2_rgb2labTable[ary2_imgInput[j][i]][1] - imgAvgA) + backgroundAvgA;
				//adjustedBValue = (backgroundStdB / imgStdB) * (ary2_rgb2labTable[ary2_imgInput[j][i]][2] - imgAvgB) + backgroundAvgB;
				//adjustedLValue = weightingLAB * adjustedLValue + (1.0 - weightingLAB) * ary2_rgb2labTable[ary2_imgInput[j][i]][0];
				//adjustedAValue = weightingLAB * adjustedAValue + (1.0 - weightingLAB) * ary2_rgb2labTable[ary2_imgInput[j][i]][1];
				//adjustedBValue = weightingLAB * adjustedBValue + (1.0 - weightingLAB) * ary2_rgb2labTable[ary2_imgInput[j][i]][2];
				//adjustedLValue = (ptr_theFilter->stdL / imgStdL) * (ary2_rgb2labTable[ary2_imgInput[j][i]][0] - imgAvgL) + ptr_theFilter->avgL;
				//adjustedAValue = (ptr_theFilter->stdA / imgStdA) * (ary2_rgb2labTable[ary2_imgInput[j][i]][1] - imgAvgA) + ptr_theFilter->avgA;
				//adjustedBValue = (ptr_theFilter->stdB / imgStdB) * (ary2_rgb2labTable[ary2_imgInput[j][i]][2] - imgAvgB) + ptr_theFilter->avgB;	
				
				sL = 1 - weightingS + weightingS * (backgroundStdL / imgStdL); 
				sA = 1 - weightingS + weightingS * (backgroundStdA / imgStdA); 
				sB = 1 - weightingS + weightingS * (backgroundStdB / imgStdB); 
				dL = weightingD * (backgroundAvgL - imgAvgL);
				dA = weightingD * (backgroundAvgA - imgAvgA);
				dB = weightingD * (backgroundAvgB - imgAvgB);
				
				adjustedLValue = sL * (ary2_rgb2labTable[ary2_imgInput[j][i]][0] - imgAvgL) + imgAvgL + dL;
				adjustedAValue = sA * (ary2_rgb2labTable[ary2_imgInput[j][i]][1] - imgAvgA) + imgAvgA + dA;
				adjustedBValue = sB * (ary2_rgb2labTable[ary2_imgInput[j][i]][2] - imgAvgB) + imgAvgB + dB;

				ary2_imgFrame[xStart+j][yStart+i] = Lab2RGB(adjustedLValue,adjustedAValue,adjustedBValue);
				//ary2_imgFrame[xStart+j][yStart+i] = ary2_imgInput[j][i];
			}
			//-----if(xStart+j>9 && yStart+i>9){//LowPass Filter for almost whole image
				tmpRed=0;
				tmpGreen=0;
				tmpBlue=0;
				for(k=0;k<10;k++){
					for(m=0;m<10;m++){
						tmpColor = ary2_imgFrame[xStart+j-k][yStart+i-m];
						tmpRed += ((tmpColor & 0xf800) >> 11);
						tmpGreen += ((tmpColor & 0x07e0) >> 5);
						tmpBlue += (tmpColor & 0x001f);
					}
				}
				ary2_imgFrame[xStart+j][yStart+i] = (((short)(tmpRed/100)&0x0f8)<<8)|(((short)(tmpGreen/100)&0x0fc)<<3)|(((short)(tmpBlue/100)&0x0f8)>>3);
			}---------//
		}
	}
	*/
	int i,j;
	int xStart = Min(XLCD, floor(ptr_theFilter->xCenter-100));
	int yStart = Min(YLCD, floor(ptr_theFilter->yCenter-100));
	for(j=0;j<HEIGHT;j++){
		if(xStart+j >= XLCD || xStart+j < 0)
			continue;
		for(i=0;i<WIDTH;i++){
			if(yStart+i >= YLCD || yStart+i < 0)
				continue;
			
			if(ary2_imgInput[j][i] != 0){	
				ary2_imgFrame[xStart+j][yStart+i] = ary2_imgInput[j][i];
			}
		}
	}

}

