#include "Utility.h"

float 			rhThreshold = 0;//= 26;		//The minimum RED pixel value to be classified as RED
float 			rsThreshold = 0.5;//= 33;	//The maximum GREEN pixel value to be classified as RED
float 			rvThreshold = 0.5;//= 17;	//The maximum BLUE pixel value to be classified as RED
float 			rhBias = 10;				//The minimum RED pixel value to be classified as RED
float 			rsBias = 0.1;				//The maximum GREEN pixel value to be classified as RED
float 			rvBias = 0.1;				//The maximum BLUE pixel value to be classified as RED
unsigned short 	rLowerBound = 100;
unsigned short 	rUpperBound = 1000;
unsigned short 	rQuantifiedLevel = 10;
unsigned short 	rBoxPadding = 20;			//Search range beyond RED Box's border
unsigned short 	rBoxBorder = 2;				//RED Box's border width
/*
float 			ghThreshold = 17;			//The minimum RED pixel value to be classified as GREEN
float 			gsThreshold = 58;			//The maximum GREEN pixel value to be classified as GREEN
float 			gvThreshold = 17;			//The maximum BLUE pixel value to be classified as GREEN
float 			ghBias = 10;				//The minimum RED pixel value to be classified as RED
float 			gsBias = 10;				//The maximum GREEN pixel value to be classified as RED
float 			gvBias = 10;				//The maximum BLUE pixel value to be classified as RED
unsigned short 	gLowerBound = 100;
unsigned short  gUpperBound = 1000;
unsigned short  gQuantifiedLevel = 10;
unsigned short  gBoxPadding = 20;			//Search range beyond RED Box's border
unsigned short  gBoxBorder = 2;				//RED Box's border width
*/
float 			bhThreshold = 15;			//The minimum RED pixel value to be classified as BLUE
float 			bsThreshold = 32;			//The maximum GREEN pixel value to be classified as BLUE
float 			bvThreshold = 22;			//The maximum BLUE pixel value to be classified as BLUE
float 			bhBias = 10;				//The minimum RED pixel value to be classified as RED	
float 			bsBias = 10;				//The maximum GREEN pixel value to be classified as RED
float 			bvBias = 10;				//The maximum BLUE pixel value to be classified as RED
unsigned short 	bLowerBound = 100;
unsigned short 	bUpperBound = 1000;
unsigned short 	bQuantifiedLevel = 10;
unsigned short 	bBoxPadding = 20;			//Search range beyond RED Box's border
unsigned short 	bBoxBorder = 2;				//RED Box's border width

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

/*int int_min(int A, int B){
	if(A < B)
		return A;
	else
		return B;
}

int int_max(int A, int B){
	if(A > B)
		return A;
	else
		return B;
}*/

void InitializeFilter(int ballColor, Filter *ptr_newFilter){
	ptr_newFilter->ballFound = 0;
	ptr_newFilter->ballColor = ballColor;
	ptr_newFilter->ballSize = 0;
	ptr_newFilter->xCenter = 0.0;
	ptr_newFilter->yCenter = 0.0;
	switch(ballColor){
		case rColor:
			ptr_newFilter->hThreshold = rhThreshold;
			ptr_newFilter->sThreshold = rsThreshold;
			ptr_newFilter->vThreshold = rvThreshold;
			ptr_newFilter->hBias = rhBias;
			ptr_newFilter->sBias = rsBias;
			ptr_newFilter->vBias = rvBias;
			ptr_newFilter->lowerBound = rLowerBound;
			ptr_newFilter->upperBound = rUpperBound;
			ptr_newFilter->quantifiedLevel = rQuantifiedLevel;
			ptr_newFilter->boxPadding = rBoxPadding;
			ptr_newFilter->boxBorder = rBoxBorder;
			break;
		case gColor:
			/*ptr_newFilter->hThreshold = ghThreshold;
			ptr_newFilter->sThreshold = gsThreshold;
			ptr_newFilter->vThreshold = gvThreshold;
			ptr_newFilter->hBias = ghBias;
			ptr_newFilter->sBias = gsBias;
			ptr_newFilter->vBias = gvBias;
			ptr_newFilter->lowerBound = gLowerBound;
			ptr_newFilter->upperBound = gUpperBound;
			ptr_newFilter->quantifiedLevel = gQuantifiedLevel;
			ptr_newFilter->boxPadding = gBoxPadding;
			ptr_newFilter->boxBorder = gBoxBorder;*/
			break;
		case bColor:
			ptr_newFilter->hThreshold = bhThreshold;
			ptr_newFilter->sThreshold = bsThreshold;
			ptr_newFilter->vThreshold = bvThreshold;
			ptr_newFilter->hBias = bhBias;
			ptr_newFilter->sBias = bsBias;
			ptr_newFilter->vBias = bvBias;
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

