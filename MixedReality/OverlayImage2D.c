#include "Utility.h"
#include <math.h>

void OverlayImage2D(Filter *ptr_leftFilter, Filter *ptr_rightFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary2_imgInput[HEIGHT][WIDTH]){
	int i,j;
	unsigned short tmpColor;
	//short *rotatedImage;
	//int j,i,xFrom,xTo,yFrom,yTo,totalNumber;
	//int imageScaleFactor=PERCENT, leftScaleFactor=PERCENT, rightScaleFactor=PERCENT;
	int comXCenter = floor((ptr_leftFilter->xCenter + ptr_rightFilter->xCenter)/2);
	int comYCenter = floor((ptr_leftFilter->yCenter + ptr_rightFilter->yCenter)/2);
	//int comSize = Max(ptr_leftFilter->ballSize, ptr_rightFilter->ballSize);
	/*70 + (comSize - ptr_leftFilter->lowerBound)/ ptr_leftFilter->quantifiedLevel
	ptr_leftFilter->quantifiedLevel*/
	//int comLength = 10 * floor((70 + (210/75000) * (comSize-5000)) / 10);
	/*int xStart = Min(XLCD, Max(0, comXCenter-40));
	int yStart = Min(YLCD, Max(0, comYCenter-40));
	for(j=40;j<160;j++){
		if(xStart+j > XLCD)
			break;
		for(i=30;i<180;i++){
			if(yStart+i-30 > YLCD)
				break;
			tmpColor = ary2_imgInput[j][i];
			if(((tmpColor & 0xf800) >> 11) < 28 && ((tmpColor & 0x07e0) >> 5) < 58 && (tmpColor & 0x001f) < 28)
				ary2_imgFrame[xStart+j-40][yStart+i-30] = tmpColor;
		}
	}*/

	int xStart = Min(XLCD, comXCenter-100);
	int yStart = Min(YLCD, comYCenter-100);
	for(j=0;j<HEIGHT;j++){
		if(xStart+j > XLCD || xStart+j < 0)
			continue;
		for(i=0;i<WIDTH;i++){
			if(yStart+i > YLCD || yStart+i < 0)
				continue;
			tmpColor = ary2_imgInput[j][i];
			if(((tmpColor & 0xf800) >> 11) < 28 && ((tmpColor & 0x07e0) >> 5) < 58 && (tmpColor & 0x001f) < 28)
				ary2_imgFrame[xStart+j][yStart+i] = tmpColor;
		}
	}
	//short rgbAdj;
	//float rTemp, gTemp, bTemp;
	//int *ptr_lValue, *ptr_aValue, *ptr_bValue;
	//float avgL=0,avgA=0,avgB=0;
	//float varL=0,varA=0,varB=0;
	//quantifiedLevel
	/*if(ptr_leftFilter->ballSize < ptr_leftFilter->upperBound){
		leftScaleFactor = (floor((ptr_leftFilter->ballSize * ptr_leftFilter->quantifiedLevel) / ptr_leftFilter->upperBound) * PERCENT) / ptr_leftFilter->quantifiedLevel;		
	}
	if(ptr_rightFilter->ballSize < ptr_rightFilter->upperBound){
		rightScaleFactor = (floor((ptr_rightFilter->ballSize * ptr_rightFilter->quantifiedLevel) / ptr_rightFilter->upperBound) * PERCENT) / ptr_rightFilter->quantifiedLevel;		
	}
	
	imageScaleFactor = leftScaleFactor+rightScaleFactor;
	if(imageScaleFactor>PERCENT)
		imageScaleFactor = PERCENT;

	scaleImage(imageScaleFactor);
	*/
	//select rotated images
	/*if(leftScaleFactor == rightScaleFactor)
		rotatedImage = ary2_subImage; //0 degree
	else if(leftScaleFactor > rightScaleFactor)
		rotatedImage = ary2_subImage; //-90 degree
	else
		rotatedImage = ary2_subImage;//90 degree
	*/
    /* imgOrientation = leftScaleFactor-rightScaleFactor;
	switch(imgOrientation){
		case   0:
			break;
		case  10:
			break;
		case -10:
			break;
		case  20:
			break;
		case -20;
			break;
		default:
			;
	}*/

	//adjust LAB
	/*xFrom = int_min(XLCD, int_max(0, floor(comXCenter - (subImageHeight / 2))));
	yFrom = int_min(YLCD, int_max(0, floor(comYCenter - (subImageWidth / 2))));
	xTo = int_min(XLCD, int_max(0, floor(comXCenter + (subImageHeight / 2))));
	yTo = int_min(YLCD, int_max(0, floor(comYCenter + (subImageWidth / 2))));
	for(j=xFrom; j<xTo; j++){
		for(i=yFrom; i<yTo; i++){
			rTemp = (float)(((ary2_imgFrame[j][i] & 0xf800) >> 11) * 255 / 31);
			gTemp = (float)(((ary2_imgFrame[j][i] & 0x07e0) >> 5) * 255 / 63);
			bTemp = (float)((ary2_imgFrame[j][i] & 0x001f) * 255 / 31);
			RGB2Lab(rTemp, gTemp, bTemp, ptr_lValue, ptr_aValue, ptr_bValue);
			avgL = avgL + *ptr_lValue;
			avgA = avgA + *ptr_aValue;
			avgB = avgB + *ptr_bValue;
			varL = varL + (*ptr_lValue) * (*ptr_lValue);
			varA = varA + (*ptr_aValue) * (*ptr_aValue);
			varB = varB + (*ptr_bValue) * (*ptr_bValue);
		}
	}
	totalNumber = (xTo-xFrom)*(yTo-yFrom);
	avgL = avgL/totalNumber;
	avgA = avgA/totalNumber;
	avgB = avgB/totalNumber;
	varL = sqrt(varL/totalNumber - avgL * avgL);
	varA = sqrt(varA/totalNumber - avgA * avgA);
	varB = sqrt(varB/totalNumber - avgB * avgB);

	//subAvgL = 
	//subAvgA =
	//subAvgB =
	//subVarL =
	//subVarA =
	//subVarB =

	for(i = 0; i < subImageWidth; i++){
		for(j = 0; j < subImageHeight; j++){					
			if((ary2_subImage[j][i] - ((short)0xFFFF)) >= (1)){ 
				rTemp = (float)(((ary2_subImage[j][i]&0xF800)>>11) * 255 / 31);
				gTemp = (float)(((ary2_subImage[j][i]&0x7E0)>>5) * 255 / 63);
				bTemp = (float)((ary2_subImage[j][i]&0x1F) * 255 / 31);
						
				RGB2Lab(rTemp,gTemp,bTemp,ptr_lValue,ptr_aValue,ptr_bValue);
				*ptr_lValue = (varL/subVarL) * (*ptr_lValue - subAvgL) + avgL;
				*ptr_aValue = (varA/subVarA) * (*ptr_aValue - subAvgA) + avgA;
				*ptr_bValue = (varB/subVarB) * (*ptr_bValue - subAvgB) + avgB;

				Lab2RGB(*ptr_lValue, *ptr_aValue, *ptr_bValue, &rTemp, &gTemp, &bTemp);
	
				rTemp = (float)rTemp * 31 / 255;
				gTemp = (float)gTemp * 63 / 255;
				bTemp = (float)bTemp * 31 / 255;

				rgbAdj = (((short)rTemp)<<11)|(((short)gTemp)<<5)|((short)bTemp);	
				ary2_imgFrame[int_min(XLCD, int_max(0, floor(j + comXCenter - (subImageHeight / 2))))][int_min(YLCD, int_max(0, floor(i + comYCenter - (subImageWidth / 2))))] = rgbAdj;						
			}
		}
	}*/
}

