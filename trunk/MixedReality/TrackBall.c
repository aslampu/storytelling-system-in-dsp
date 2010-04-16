#include <math.h>
#include "Utility.h"
																
void TrackBall(Filter *ptr_oldFilter, unsigned short ary2_imgFrame[XLCD][YLCD], float ary2_rgb2hsvTable[NUM_RGB][3], short ary2_rgb2labTable[NUM_RGB][3]){
	int i, j, xFrom, yFrom, xTo, yTo, hueFlag = 0;
	float hTemp, hL, hU, hSwap, sTemp, vTemp;
	int L,a,b;
	int labNumber = 0;
	float avgL = 0, avgA = 0, avgB = 0, stdL = 0, stdA = 0, stdB = 0;
	//int rTemp, gTemp, bTemp;

	Filter newComer;
	InitializeFilter(ptr_oldFilter->ballColor, &newComer);
	newComer.avgL = ptr_oldFilter->avgL;
	newComer.avgA = ptr_oldFilter->avgA;
	newComer.avgB = ptr_oldFilter->avgB;
	newComer.stdL = ptr_oldFilter->stdL;
	newComer.stdA = ptr_oldFilter->stdA;
	newComer.stdB = ptr_oldFilter->stdB;
	
	xFrom = ptr_oldFilter->xFrom;
	xTo = ptr_oldFilter->xTo;
	yFrom = ptr_oldFilter->yFrom;
	yTo = ptr_oldFilter->yTo;

	//Sweep given image matrix for the color ball
	for(j=xFrom;j<xTo;j++){
		for(i=yFrom;i<yTo;i++){
			//hTemp = RGB2HSV(ary2_imgFrame[j][i]);
			hTemp = ary2_rgb2hsvTable[ary2_imgFrame[j][i]][0];
			sTemp = ary2_rgb2hsvTable[ary2_imgFrame[j][i]][1];
			vTemp = ary2_rgb2hsvTable[ary2_imgFrame[j][i]][2];
			//rTemp = (int)((((ary2_imgFrame[j][i] & 0xf800) >> 11) * 255 / 31));
			//gTemp = (int)((((ary2_imgFrame[j][i] & 0x07e0) >> 5) * 255 / 63));
			//bTemp = (int)(((ary2_imgFrame[j][i] & 0x001f) * 255 / 31));

			hL = ptr_oldFilter->hThreshold-ptr_oldFilter->hBias;
			hU = ptr_oldFilter->hThreshold+ptr_oldFilter->hBias;			
			if(hL < 0){
				hSwap = hU;
				hU = hL + 360;
				hL = hSwap;
			}
			if(hU >= 360){
				hSwap = hL;
				hL = hU - 360;
				hU = hSwap;
			}
			if(ptr_oldFilter->ballColor)	//Blue
				//hueFlag = hTemp>hL && hTemp<hU;
				//hueFlag = hTemp>hL && hTemp<hU && rTemp > 50 && rTemp < 90 && gTemp > 95 && gTemp < 130 && bTemp > 130 && bTemp < 180;
				hueFlag = hTemp > hL && hTemp < hU && sTemp > 144 && vTemp > 102;
			else							//Red
				//hueFlag = hTemp<hL || hTemp>hU && rTemp > 190 && rTemp < 225 && gTemp > 100 && gTemp < 150 && bTemp > 60 && bTemp < 120;
				hueFlag = (hTemp < hL || hTemp > hU) && sTemp > 144 && vTemp > 102;
			
			if(hueFlag){
				hueFlag = 0;
				newComer.ballSize++;
				newComer.xCenter += j;
				newComer.yCenter += i;
			}else{
				labNumber++;
				L = ary2_rgb2labTable[ary2_imgFrame[j][i]][0];
				a = ary2_rgb2labTable[ary2_imgFrame[j][i]][1];
				b = ary2_rgb2labTable[ary2_imgFrame[j][i]][2];

				avgL += L;
				avgA += a;
				avgB += b;
				stdL += L * L;
				stdA += a * a;
				stdB += b * b;
			}
		}
	}
	if(newComer.ballSize > newComer.lowerBound){			
		newComer.ballFound = 1;
		newComer.xCenter = newComer.xCenter/newComer.ballSize;
		newComer.yCenter = newComer.yCenter/newComer.ballSize;
		if(pow(newComer.xCenter - ptr_oldFilter->xCenter,2) + pow(newComer.yCenter - ptr_oldFilter->yCenter,2) < 100){
			newComer.xCenter = ptr_oldFilter->xCenter;
			newComer.yCenter = ptr_oldFilter->yCenter;	
		}
		//newComer.xFrom = Min(XLCD, Max(0, floor(ptr_oldFilter->xFrom + newComer.xCenter - ptr_oldFilter->xCenter)));
		//newComer.xTo = Min(XLCD, Max(0, floor(ptr_oldFilter->xTo + newComer.xCenter - ptr_oldFilter->xCenter)));
		//newComer.yFrom = Min(YLCD, Max(0, floor(ptr_oldFilter->yFrom + newComer.yCenter - ptr_oldFilter->yCenter)));
		//newComer.yTo = Min(YLCD, Max(0, floor(ptr_oldFilter->yTo + newComer.yCenter - ptr_oldFilter->yCenter)));
		newComer.xFrom = Min(XLCD, Max(0, floor(ptr_oldFilter->xFrom)));
		newComer.xTo = Min(XLCD, Max(0, floor(ptr_oldFilter->xTo)));
		newComer.yFrom = Min(YLCD, Max(0, floor(ptr_oldFilter->yFrom)));
		newComer.yTo = Min(YLCD, Max(0, floor(ptr_oldFilter->yTo)));
		*ptr_oldFilter = newComer;
	}else		
		InitializeFilter(newComer.ballColor, ptr_oldFilter);
		ptr_oldFilter->avgL = avgL/labNumber;
		ptr_oldFilter->avgA = avgA/labNumber;
		ptr_oldFilter->avgB = avgB/labNumber;
		ptr_oldFilter->stdL = sqrt(stdL/labNumber - ptr_oldFilter->avgL * ptr_oldFilter->avgL);
		ptr_oldFilter->stdA = sqrt(stdA/labNumber - ptr_oldFilter->avgA * ptr_oldFilter->avgA);
		ptr_oldFilter->stdB = sqrt(stdB/labNumber - ptr_oldFilter->avgB * ptr_oldFilter->avgB);
}

