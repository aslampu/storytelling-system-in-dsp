#include "Utility.h"

void PLL6713(){
	int i;
	// CPU Clock Input : 50MHz
	
	*(volatile int *)(0x01b7c100) = *(volatile int *)(0x01b7c100) & 0xfffffffe;
	for(i=0;i<4;i++);
	*(volatile int *)(0x01b7c100) = *(volatile int *)(0x01b7c100) | 0x08;
	*(volatile int *)(0x01b7c114) = 0x08001;  // 50MHz/2 = 25MHz
 	*(volatile int *)(0x01b7c110) = 0x0c;     // 25MHz * 12 = 300MHz
	*(volatile int *)(0x01b7c118) = 0x08000;  // SYSCLK1 = 300MHz/1 = 300MHz
	*(volatile int *)(0x01b7c11c) = 0x08001;  // SYSCLK2 = 300MHz/2 = 150MHz // Peripheral Clock
	*(volatile int *)(0x01b7c120) = 0x08003;  // SYSCLK3 = 300MHz/4 =  75MHz // SDRAM Clock
	for(i=0;i<4;i++);
	*(volatile int *)(0x01b7c100) = *(volatile int *)(0x01b7c100) & 0xfffffff7;
	for(i=0;i<4;i++);
	*(volatile int *)(0x01b7c100) = *(volatile int *)(0x01b7c100) | 0x01;
}

unsigned short	ybr_565(short y,short u,short v){
	int		r,g,b;

 	b = y + 1772*(u-128)/1000;
	if (b<0) b=0;
	if (b>255) b=255;
	g = y - (344*(u-128) + 714*(v-128))/1000;
	if (g<0) g=0;
	if (g>255) g=255;
	r = y + 1402*(v-128)/1000;
	if (r<0) r=0;
	if (r>255) r=255;
	return ((r&0x0f8)<<8)|((g&0x0fc)<<3)|((b&0x0f8)>>3);
}

void InitializeFilter(int ballColor, Filter *ptr_newFilter){
	ptr_newFilter->ballFound = 0;
	ptr_newFilter->ballColor = ballColor;
	ptr_newFilter->ballSize = 0;
	ptr_newFilter->xCenter = 0;
	ptr_newFilter->yCenter = 0;
	ptr_newFilter->xFrom = 0;
	ptr_newFilter->xTo = XLCD;
	ptr_newFilter->yFrom = 0;
	ptr_newFilter->yTo = YLCD;
	ptr_newFilter->avgL = 0;
	ptr_newFilter->avgA = 0;
	ptr_newFilter->avgB = 0;
	ptr_newFilter->stdL = 0;
	ptr_newFilter->stdA = 0;
	ptr_newFilter->stdB = 0;
	ptr_newFilter->scaleFactor = 100;

	switch(ballColor){
		case rColor:
			ptr_newFilter->hThreshold = rhThreshold;
			ptr_newFilter->hBias = rhBias;
			ptr_newFilter->lowerBound = rLowerBound;
			ptr_newFilter->upperBound = rUpperBound;
			ptr_newFilter->quantifiedLevel = rQuantifiedLevel;
			ptr_newFilter->boxPadding = rBoxPadding;
			ptr_newFilter->boxBorder = rBoxBorder;
			break;
		case gColor:
			ptr_newFilter->hThreshold = ghThreshold;
			ptr_newFilter->hBias = ghBias;
			ptr_newFilter->lowerBound = gLowerBound;
			ptr_newFilter->upperBound = gUpperBound;
			ptr_newFilter->quantifiedLevel = gQuantifiedLevel;
			ptr_newFilter->boxPadding = gBoxPadding;
			ptr_newFilter->boxBorder = gBoxBorder;
			break;
		case bColor:
			ptr_newFilter->hThreshold = bhThreshold;
			ptr_newFilter->hBias = bhBias;
			ptr_newFilter->lowerBound = bLowerBound;
			ptr_newFilter->upperBound = bUpperBound;
			ptr_newFilter->quantifiedLevel = bQuantifiedLevel;
			ptr_newFilter->boxPadding = bBoxPadding;
			ptr_newFilter->boxBorder = bBoxBorder;
			break;
		default:
			;//error message
	}
}

