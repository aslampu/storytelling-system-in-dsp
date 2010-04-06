#include <math.h>
#include "Utility.h"

void OverlayImage1D(Filter *ptr_theFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary2_imgInput[HEIGHT][WIDTH]){
	int i,j;
	unsigned short tmpColor;
	//int comLength = 10 * floor((70 + (210/75000) * (ptr_theFilter->ballSize-5000)) / 10);
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

	int xStart = Min(XLCD, floor(ptr_theFilter->xCenter-100));
	int yStart = Min(YLCD, floor(ptr_theFilter->yCenter-100));
	//DrawBox(ptr_theFilter->xFrom, ptr_theFilter->xTo, ptr_theFilter->yFrom, ptr_theFilter->yTo, ptr_theFilter->boxBorder, ptr_theFilter->boxPadding, ary2_imgFrame);
	for(j=0;j<HEIGHT;j++){
		if(xStart+j >= XLCD || xStart+j < 0)
			continue;
		for(i=0;i<WIDTH;i++){
			if(yStart+i >= YLCD || yStart+i < 0)
				continue;
			tmpColor = ary2_imgInput[j][i];
			if(((tmpColor & 0xf800) >> 11) < 28 && ((tmpColor & 0x07e0) >> 5) < 58 && (tmpColor & 0x001f) < 28)
				ary2_imgFrame[xStart+j][yStart+i] = tmpColor;
		}
	}
}

