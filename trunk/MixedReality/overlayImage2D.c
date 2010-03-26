#include "utility.h"
#include "image.h"
#include "scaleImage.h"
#include "subImage.h"
#include "LAB.h"

#define PERCENT 100

void overlayImage2D(short tmp[][YLCD], Filter *Left_Latter, Filter *Right_Latter){
	//short *rotated_image;
	int j,i,XFrom,XTo,YFrom,YTo,total_number;
	int imageScaleFactor=PERCENT, Left_ScaleFactor=PERCENT, Right_ScaleFactor=PERCENT;
	float comXcenter = (Left_Latter->Xcenter + Right_Latter->Xcenter)/2;
	float comYcenter = (Left_Latter->Ycenter + Right_Latter->Ycenter)/2;
	short rgbAdj;
	float rTemp, gTemp, bTemp;
	int *l, *a, *b;
	float avgL=0,avgA=0,avgB=0;
	float varL=0,varA=0,varB=0;
	//size levelize (quantify)
	if(Left_Latter->Size < Left_Latter->UpperBound){
		Left_ScaleFactor = (floor((Left_Latter->Size * Left_Latter->Level) / Left_Latter->UpperBound) * PERCENT) / Left_Latter->Level;		
	}
	if(Right_Latter->Size < Right_Latter->UpperBound){
		Right_ScaleFactor = (floor((Right_Latter->Size * Right_Latter->Level) / Right_Latter->UpperBound) * PERCENT) / Right_Latter->Level;		
	}
	
	imageScaleFactor = Left_ScaleFactor+Right_ScaleFactor;
	if(imageScaleFactor>PERCENT)
		imageScaleFactor = PERCENT;

	scaleImage(imageScaleFactor);
	
	//select rotated images
	/*if(Left_ScaleFactor == Right_ScaleFactor)
		rotated_image = sub_image; //0 degree
	else if(Left_ScaleFactor > Right_ScaleFactor)
		rotated_image = sub_image; //-90 degree
	else
		rotated_image = sub_image;//90 degree
	*/
    /*orientation = Left_ScaleFactor-Right_ScaleFactor;
	switch(orientation){
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
	XFrom = int_min(XLCD, int_max(0, floor(comXcenter - (subImageHeight / 2))));
	YFrom = int_min(YLCD, int_max(0, floor(comYcenter - (subImageWidth / 2))));
	XTo = int_min(XLCD, int_max(0, floor(comXcenter + (subImageHeight / 2))));
	YTo = int_min(YLCD, int_max(0, floor(comYcenter + (subImageWidth / 2))));
	for(j=XFrom; j<XTo; j++){
		for(i=YFrom; i<YTo; i++){
			rTemp = (float)(((tmp[j][i] & 0xf800) >> 11) * 255 / 31);
			gTemp = (float)(((tmp[j][i] & 0x07e0) >> 5) * 255 / 63);
			bTemp = (float)((tmp[j][i] & 0x001f) * 255 / 31);
			RGB2Lab(rTemp, gTemp, bTemp, l, a, b);
			avgL = avgL + *l;
			avgA = avgA + *a;
			avgB = avgB + *b;
			varL = varL + (*l) * (*l);
			varA = varA + (*a) * (*a);
			varB = varB + (*b) * (*b);
		}
	}
	total_number = (XTo-XFrom)*(YTo-YFrom);
	avgL = avgL/total_number;
	avgA = avgA/total_number;
	avgB = avgB/total_number;
	varL = sqrt(varL/total_number - avgL * avgL);
	varA = sqrt(varA/total_number - avgA * avgA);
	varB = sqrt(varB/total_number - avgB * avgB);

	//subAvgL = 
	//subAvgA =
	//subAvgB =
	//subVarL =
	//subVarA =
	//subVarB =

	for(i = 0; i < subImageWidth; i++){
		for(j = 0; j < subImageHeight; j++){					
			if((sub_image[j][i] - ((short)0xFFFF)) >= (1)){ 
				rTemp = (float)(((sub_image[j][i]&0xF800)>>11) * 255 / 31);
				gTemp = (float)(((sub_image[j][i]&0x7E0)>>5) * 255 / 63);
				bTemp = (float)((sub_image[j][i]&0x1F) * 255 / 31);
						
				RGB2Lab(rTemp,gTemp,bTemp,l,a,b);
				//*l = (varL/subVarL) * (*l - subAvgL) + avgL;
				//*a = (varA/subVarA) * (*l - subAvgA) + avgA;
				//*b = (varB/subVarB) * (*l - subAvgB) + avgB;

				Lab2RGB(*l, *a, *b, &rTemp, &gTemp, &bTemp);
	
				rTemp = (float)rTemp * 31 / 255;
				gTemp = (float)gTemp * 63 / 255;
				bTemp = (float)bTemp * 31 / 255;

				rgbAdj = (((short)rTemp)<<11)|(((short)gTemp)<<5)|((short)bTemp);	
				tmp[int_min(XLCD, int_max(0, floor(j + comXcenter - (subImageHeight / 2))))][int_min(XLCD, int_max(0, floor(i + comYcenter - (subImageWidth / 2))))] = rgbAdj;						
			}
		}
	}
}

