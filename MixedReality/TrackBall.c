#include <math.h>
#include "Utility.h"
																
void TrackBall(Filter *ptr_oldFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary3_rgb2hsvTable[NUM_RGB][3]){
	int i, j, xFrom, yFrom, xTo, yTo, hueFlag = 0;
	int hTemp, hL, hU, hSwap, sTemp, vTemp;

	Filter newComer;
	InitializeFilter(ptr_oldFilter->ballColor, &newComer);
	
	xFrom = ptr_oldFilter->xFrom;
	xTo = ptr_oldFilter->xTo;
	yFrom = ptr_oldFilter->yFrom;
	yTo = ptr_oldFilter->yTo;

	//Sweep given image matrix for the color ball
	for(j=xFrom;j<xTo;j++){
		for(i=yFrom;i<yTo;i++){
			//hTemp = RGB2HSV(ary2_imgFrame[j][i]);
			hTemp = ary3_rgb2hsvTable[ary2_imgFrame[j][i]][0];
			sTemp = ary3_rgb2hsvTable[ary2_imgFrame[j][i]][1];
			vTemp = ary3_rgb2hsvTable[ary2_imgFrame[j][i]][2];
			//hTemp = ary2_rgb2hsvTable[ary2_imgFrame[j][i]][0];
			//sTemp = ary2_rgb2hsvTable[ary2_imgFrame[j][i]][1];
			//vTemp = ary2_rgb2hsvTable[ary2_imgFrame[j][i]][2];

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
			if(ptr_oldFilter->ballColor)	//not Red
				hueFlag = hTemp>hL && hTemp<hU && sTemp > 216 && vTemp > 64;
			else
				hueFlag = hTemp<hL || hTemp>hU && sTemp > 216 && vTemp > 64;
			
			if(hueFlag){
				hueFlag = 0;
				newComer.ballSize++;
				newComer.xCenter += j;
				newComer.yCenter += i;
			}					
		}
	}
	if(newComer.ballSize > newComer.lowerBound){
		newComer.ballFound = 1;
		newComer.xCenter = newComer.xCenter/newComer.ballSize;
		newComer.yCenter = newComer.yCenter/newComer.ballSize;
		//newComer.xFrom = Min(XLCD, Max(0, floor(ptr_oldFilter->xFrom + newComer.xCenter - ptr_oldFilter->xCenter)));
		//newComer.xTo = Min(XLCD, Max(0, floor(ptr_oldFilter->xTo + newComer.xCenter - ptr_oldFilter->xCenter)));
		//newComer.yFrom = Min(YLCD, Max(0, floor(ptr_oldFilter->yFrom + newComer.yCenter - ptr_oldFilter->yCenter)));
		//newComer.yTo = Min(YLCD, Max(0, floor(ptr_oldFilter->yTo + newComer.yCenter - ptr_oldFilter->yCenter)));
		newComer.xFrom = Min(XLCD, Max(0, floor(ptr_oldFilter->xFrom)));
		newComer.xTo = Min(XLCD, Max(0, floor(ptr_oldFilter->xTo)));
		newComer.yFrom = Min(YLCD, Max(0, floor(ptr_oldFilter->yFrom)));
		newComer.yTo = Min(YLCD, Max(0, floor(ptr_oldFilter->yTo)));
		*ptr_oldFilter = newComer;

		/*Draw if necessary
		if(newComer.ballSize < newComer.upperBound){
			imageScaleFactor = floor((newComer.ballSize * newComer.quantifiedLevel) / newComer.upperBound) / newComer.quantifiedLevel;		
			searchXLength = HEIGHT * imageScaleFactor;
			searchYLength = WIDTH * imageScaleFactor;
		}else{
			searchXLength = HEIGHT;
			searchYLength = WIDTH;
		}
		xFrom = Min(XLCD, Max(0, floor(newComer.xCenter - (searchXLength / 2) - newComer.boxPadding - newComer.boxBorder)));
		yFrom = Min(YLCD, Max(0, floor(newComer.yCenter - (searchYLength / 2) - newComer.boxPadding - newComer.boxBorder)));
		xTo = Min(XLCD, Max(0, floor(newComer.xCenter + (searchXLength / 2) + newComer.boxPadding + newComer.boxBorder)));
		yTo = Min(YLCD, Max(0, floor(newComer.yCenter + (searchYLength / 2) + newComer.boxPadding + newComer.boxBorder)));
		DrawBox(xFrom, xTo, yFrom, yTo, newComer.boxBorder, newComer.boxPadding, ary2_imgFrame);
		*/
	}else		
		InitializeFilter(newComer.ballColor, ptr_oldFilter);
}

