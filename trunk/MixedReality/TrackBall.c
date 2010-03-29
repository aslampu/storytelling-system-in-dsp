#include <math.h>
#include "Utility.h"
#include "Image.h"

void TrackBall(Filter *ptr_formerFilter, Filter *ptr_latterFilter, short ary2_imgFrame[XLCD][YLCD]){
	int i = 0, j = 0, hueFlag = 0;
	int xFrom = -1, yFrom = -1, xTo = -1, yTo = -1;
	float rTemp = -1, gTemp = -1, bTemp = -1;
	float hTemp = -1, sTemp = -1, vTemp = -1;
	float hL,hU,sL,sU,vL,vU;
	//Search Range Prediction
	int searchXCenter = 2 * (ptr_latterFilter->xCenter - ptr_formerFilter->xCenter);
	int searchYCenter = 2 * (ptr_latterFilter->yCenter - ptr_formerFilter->yCenter);
	int searchXLength = 2 * XLCD;
	int searchYLength = 2 * YLCD;
	float imageScaleFactor;//reserved
	Filter newComer;
	InitializeFilter(ptr_latterFilter->ballColor, &newComer);
	
	if(ptr_latterFilter->ballFound){
		searchXLength = sqrt((ptr_latterFilter->ballSize * ptr_latterFilter->ballSize) / ptr_formerFilter->ballSize);//always square
		searchYLength = searchXLength;
	}

	xFrom = Min(XLCD, Max(0, floor(searchXCenter - (searchXLength / 2) - ptr_latterFilter->boxPadding - ptr_latterFilter->boxBorder)));
	yFrom = Min(YLCD, Max(0, floor(searchYCenter - (searchXLength / 2) - ptr_latterFilter->boxPadding - ptr_latterFilter->boxBorder)));
	xTo = Min(XLCD, Max(0, floor(searchXCenter + (searchXLength / 2) + ptr_latterFilter->boxPadding + ptr_latterFilter->boxBorder)));
	yTo = Min(YLCD, Max(0, floor(searchYCenter + (searchYLength / 2) + ptr_latterFilter->boxPadding + ptr_latterFilter->boxBorder)));
	
	//Sweep given image matrix for the color ball
	for(j=xFrom;j<xTo;j++){
		for(i=yFrom;i<yTo;i++){
			rTemp = (ary2_imgFrame[j][i] & 0xf800) >> 11;
			gTemp = (ary2_imgFrame[j][i] & 0x07e0) >> 5;
			bTemp = (ary2_imgFrame[j][i] & 0x001f);
			RGB2HSV(rTemp,gTemp,bTemp,&hTemp,&sTemp,&vTemp);
			
			hL = ptr_latterFilter->hThreshold-ptr_latterFilter->hBias;
			hU = ptr_latterFilter->hThreshold+ptr_latterFilter->hBias;
			if(hL < 0 || hU >= 360){
				if(hL < 0){
					hL += 360;
					hueFlag = hTemp>hL || hTemp<hU;
				}else{
					hU -= 360;
					hueFlag = hTemp>hL || hTemp<hU;
				}
			}else
				hueFlag = hTemp>hL && hTemp<hU;
			

			sL = ptr_latterFilter->sThreshold-ptr_latterFilter->sBias;
			sU = ptr_latterFilter->sThreshold+ptr_latterFilter->sBias;
			vL = ptr_latterFilter->vThreshold-ptr_latterFilter->vBias;
			vU = ptr_latterFilter->vThreshold+ptr_latterFilter->vBias;
							
			//Identify the color ball -- version1
			if( hueFlag && sTemp>sL && sTemp<sU && vTemp>vL && vTemp<vU){
				newComer.ballSize++;
				newComer.xCenter += j;
				newComer.yCenter += i;
			}
			
			//Identify the color ball -- version2
			/*if(rTemp>(ptr_latterFilter->hThreshold) && 
			     gTemp<(ptr_latterFilter->sThreshold) &&
			     bTemp<(ptr_latterFilter->vThreshold)){

				newComer.ballSize++;
				newComer.xCenter += j;
				newComer.yCenter += i;
			}*/
						
		}
	}

	if(newComer.ballSize > newComer.lowerBound){// Detect an object
		newComer.ballFound = 1;
		newComer.xCenter = newComer.xCenter/newComer.ballSize;
		newComer.yCenter = newComer.yCenter/newComer.ballSize;
		ptr_formerFilter = ptr_latterFilter;
		ptr_latterFilter = &newComer;		

		//Draw if necessary
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

	}else{		
		InitializeFilter(newComer.ballColor, ptr_formerFilter);
		InitializeFilter(newComer.ballColor, ptr_latterFilter);
	}
}

