#include <math.h>
#include "Utility.h"
																
void TrackBall2D(int xTrackCenter, int yTrackCenter, int trackRange, Filter *ptr_oldFilterGreen, Filter *ptr_oldFilterBlue, unsigned short ary2_imgFrame[XLCD][YLCD], float ary2_rgb2hsvTable[NUM_RGB][3], short ary2_rgb2labTable[NUM_RGB][3]){
	int i, j, xFrom, yFrom, xTo, yTo, hueFlagGreen = 0, hueFlagBlue=0;
	float hTemp, hLGreen, hUGreen, sTemp, vTemp, hLBlue, hUBlue;
	int L,a,b;
	int labNumber = 0;
	float avgL = 0, avgA = 0, avgB = 0, stdL = 0, stdA = 0, stdB = 0;
	//int rTemp, gTemp, bTemp;

	Filter newComerGreen, newComerBlue;
	InitializeFilter(ptr_oldFilterGreen->ballColor, &newComerGreen);
	InitializeFilter(ptr_oldFilterBlue->ballColor, &newComerBlue);

	//newComer.avgL = ptr_oldFilter->avgL;
	//newComer.avgA = ptr_oldFilter->avgA;
	//newComer.avgB = ptr_oldFilter->avgB;
	//newComer.stdL = ptr_oldFilter->stdL;
	//newComer.stdA = ptr_oldFilter->stdA;
	//newComer.stdB = ptr_oldFilter->stdB;
	
	//xFrom = ptr_oldFilter->xFrom;
	//xTo = ptr_oldFilter->xTo;
	//yFrom = ptr_oldFilter->yFrom;
	//yTo = ptr_oldFilter->yTo;
	xFrom = Min(XLCD, Max(0, xTrackCenter-trackRange));
	xTo = Min(XLCD, Max(0, xTrackCenter+trackRange));
	yFrom = Min(YLCD, Max(0, yTrackCenter-trackRange));
	yTo = Min(YLCD, Max(0, yTrackCenter+trackRange));

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

			hLGreen = ptr_oldFilterGreen->hThreshold-ptr_oldFilterGreen->hBias;
			hUGreen = ptr_oldFilterGreen->hThreshold+ptr_oldFilterGreen->hBias;
			hueFlagGreen = hTemp > hLGreen && hTemp < hUGreen && sTemp > 144 && vTemp > 102;

			hLBlue = ptr_oldFilterBlue->hThreshold-ptr_oldFilterBlue->hBias;
			hUBlue = ptr_oldFilterBlue->hThreshold+ptr_oldFilterBlue->hBias;
			hueFlagBlue = hTemp > hLBlue && hTemp < hUBlue && sTemp > 144 && vTemp > 102;

			if(hueFlagGreen){
				hueFlagGreen = 0;
				newComerGreen.ballSize++;
				newComerGreen.xCenter += j;
				newComerGreen.yCenter += i;
			}else if(hueFlagBlue){
				hueFlagBlue = 0;
				newComerBlue.ballSize++;
				newComerBlue.xCenter += j;
				newComerBlue.yCenter += i;
			}else{
				hueFlagGreen = 0;
				hueFlagBlue = 0;

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

			/*if(hL < 0){
				hSwap = hU;
				hU = hL + 360;
				hL = hSwap;
			}
			if(hU >= 360){
				hSwap = hL;
				hL = hU - 360;
				hU = hSwap;
			}*/

			/*if(ptr_oldFilter->ballColor)	//Blue
				//hueFlag = hTemp>hL && hTemp<hU;
				//hueFlag = hTemp>hL && hTemp<hU && rTemp > 50 && rTemp < 90 && gTemp > 95 && gTemp < 130 && bTemp > 130 && bTemp < 180;
				hueFlag = hTemp > hL && hTemp < hU && sTemp > 144 && vTemp > 102;
			else							//Red
				//hueFlag = hTemp<hL || hTemp>hU && rTemp > 190 && rTemp < 225 && gTemp > 100 && gTemp < 150 && bTemp > 60 && bTemp < 120;
				hueFlag = (hTemp < hL || hTemp > hU) && sTemp > 144 && vTemp > 102;*/
			
			/*if(hueFlag){
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
			}*/
		}
	}
	switch((newComerGreen.ballSize > newComerGreen.lowerBound) * 2 + (newComerBlue.ballSize > newComerBlue.lowerBound)){
		case 0:
			InitializeFilter(newComerGreen.ballColor, ptr_oldFilterGreen);
			InitializeFilter(newComerBlue.ballColor, ptr_oldFilterBlue);
			backgroundAvgL = avgL/labNumber;
			backgroundAvgA = avgA/labNumber;
			backgroundAvgB = avgB/labNumber;
			backgroundStdL = sqrt(stdL/labNumber - backgroundAvgL * backgroundAvgL);
			backgroundStdA = sqrt(stdA/labNumber - backgroundAvgA * backgroundAvgA);
			backgroundStdB = sqrt(stdB/labNumber - backgroundAvgB * backgroundAvgB);
			break;
		case 1:
			newComerBlue.ballFound = 1;
			newComerBlue.xCenter = newComerBlue.xCenter/newComerBlue.ballSize;
			newComerBlue.yCenter = newComerBlue.yCenter/newComerBlue.ballSize;
			if(pow(newComerBlue.xCenter - ptr_oldFilterBlue->xCenter,2) + pow(newComerBlue.yCenter - ptr_oldFilterBlue->yCenter,2) < displacementThreshold){
				newComerBlue.xCenter = ptr_oldFilterBlue->xCenter;
				newComerBlue.yCenter = ptr_oldFilterBlue->yCenter;	
			}
			*ptr_oldFilterBlue = newComerBlue;
			break;
		case 2:					
			newComerGreen.ballFound = 1;
			newComerGreen.xCenter = newComerGreen.xCenter/newComerGreen.ballSize;
			newComerGreen.yCenter = newComerGreen.yCenter/newComerGreen.ballSize;
			if(pow(newComerGreen.xCenter - ptr_oldFilterGreen->xCenter,2) + pow(newComerGreen.yCenter - ptr_oldFilterGreen->yCenter,2) < displacementThreshold){
				newComerGreen.xCenter = ptr_oldFilterGreen->xCenter;
				newComerGreen.yCenter = ptr_oldFilterGreen->yCenter;	
			}
			*ptr_oldFilterGreen = newComerGreen;
			break;
		case 3:
			newComerBlue.ballFound = 1;
			newComerBlue.xCenter = newComerBlue.xCenter/newComerBlue.ballSize;
			newComerBlue.yCenter = newComerBlue.yCenter/newComerBlue.ballSize;
			if(pow(newComerBlue.xCenter - ptr_oldFilterBlue->xCenter,2) + pow(newComerBlue.yCenter - ptr_oldFilterBlue->yCenter,2) < displacementThreshold){
				newComerBlue.xCenter = ptr_oldFilterBlue->xCenter;
				newComerBlue.yCenter = ptr_oldFilterBlue->yCenter;	
			}
			*ptr_oldFilterBlue = newComerBlue;
			newComerGreen.ballFound = 1;
			newComerGreen.xCenter = newComerGreen.xCenter/newComerGreen.ballSize;
			newComerGreen.yCenter = newComerGreen.yCenter/newComerGreen.ballSize;
			if(pow(newComerGreen.xCenter - ptr_oldFilterGreen->xCenter,2) + pow(newComerGreen.yCenter - ptr_oldFilterGreen->yCenter,2) < displacementThreshold){
				newComerGreen.xCenter = ptr_oldFilterGreen->xCenter;
				newComerGreen.yCenter = ptr_oldFilterGreen->yCenter;	
			}
			*ptr_oldFilterGreen = newComerGreen;
			break;
		}
		//newComer.xFrom = Min(XLCD, Max(0, floor(ptr_oldFilter->xFrom + newComer.xCenter - ptr_oldFilter->xCenter)));
		//newComer.xTo = Min(XLCD, Max(0, floor(ptr_oldFilter->xTo + newComer.xCenter - ptr_oldFilter->xCenter)));
		//newComer.yFrom = Min(YLCD, Max(0, floor(ptr_oldFilter->yFrom + newComer.yCenter - ptr_oldFilter->yCenter)));
		//newComer.yTo = Min(YLCD, Max(0, floor(ptr_oldFilter->yTo + newComer.yCenter - ptr_oldFilter->yCenter)));
		
		//newComer.xFrom = Min(XLCD, Max(0, floor(newComer.xCenter- sqrt(newComer.ballSize) / 2 - newComer.boxPadding)));
		//newComer.xTo = Min(XLCD, Max(0, floor(newComer.xFrom + sqrt(newComer.ballSize) + 2 * newComer.boxPadding)));
		//newComer.yFrom = Min(XLCD, Max(0, floor(newComer.yCenter- sqrt(newComer.ballSize) /2 - newComer.boxPadding)));
		//newComer.yTo = Min(XLCD, Max(0, floor(newComer.yFrom + sqrt(newComer.ballSize) + 2 * newComer.boxPadding)));
		
		//newComer.xFrom = Min(XLCD, Max(0, floor(newComer.xCenter-100)));
		//newComer.xTo = Min(XLCD, Max(0, floor(newComer.xFrom + 200)));
		//newComer.yFrom = Min(XLCD, Max(0, floor(newComer.yCenter-100)));
		//newComer.yTo = Min(XLCD, Max(0, floor(newComer.yFrom + 200)));
		
		//newComer.xFrom = Min(XLCD, Max(0, floor(ptr_oldFilter->xFrom)));
		//newComer.xTo = Min(XLCD, Max(0, floor(ptr_oldFilter->xTo)));
		//newComer.yFrom = Min(YLCD, Max(0, floor(ptr_oldFilter->yFrom)));
		//newComer.yTo = Min(YLCD, Max(0, floor(ptr_oldFilter->yTo)));
	
	/*}else{		
		InitializeFilter(newComer.ballColor, ptr_oldFilter);
		ptr_oldFilter->avgL = avgL/labNumber;
		ptr_oldFilter->avgA = avgA/labNumber;
		ptr_oldFilter->avgB = avgB/labNumber;
		ptr_oldFilter->stdL = sqrt(stdL/labNumber - ptr_oldFilter->avgL * ptr_oldFilter->avgL);
		ptr_oldFilter->stdA = sqrt(stdA/labNumber - ptr_oldFilter->avgA * ptr_oldFilter->avgA);
		ptr_oldFilter->stdB = sqrt(stdB/labNumber - ptr_oldFilter->avgB * ptr_oldFilter->avgB);
	}*/
}

