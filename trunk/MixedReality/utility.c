#include "utility.h"

unsigned short RH_Threshold = 26;	//The minimum RED pixel value to be classified as RED
unsigned short RS_Threshold = 33;	//The maximum GREEN pixel value to be classified as RED
unsigned short RV_Threshold = 17;	//The maximum BLUE pixel value to be classified as RED
unsigned short RH_Bias = 10;	//The minimum RED pixel value to be classified as RED
unsigned short RS_Bias = 10;	//The maximum GREEN pixel value to be classified as RED
unsigned short RV_Bias = 10;	//The maximum BLUE pixel value to be classified as RED
unsigned short R_LowerBound = 100;
unsigned short R_UpperBound = 1000;
unsigned short R_Level = 10;
unsigned short R_Padding = 20;		//Search range beyond RED Box's border
unsigned short R_Border = 2;		//RED Box's border width
/*
unsigned short GH_Threshold = 17;	//The minimum RED pixel value to be classified as GREEN
unsigned short GS_Threshold = 58;	//The maximum GREEN pixel value to be classified as GREEN
unsigned short GV_Threshold = 17;	//The maximum BLUE pixel value to be classified as GREEN
unsigned short GH_Bias = 10;	//The minimum RED pixel value to be classified as RED
unsigned short GS_Bias = 10;	//The maximum GREEN pixel value to be classified as RED
unsigned short GV_Bias = 10;	//The maximum BLUE pixel value to be classified as RED
unsigned short G_LowerBound = 100;
unsigned short G_UpperBound = 1000;
unsigned short G_Level = 10;
unsigned short G_Padding = 20;		//Search range beyond RED Box's border
unsigned short G_Border = 2;		//RED Box's border width
*/
unsigned short BH_Threshold = 15;	//The minimum RED pixel value to be classified as BLUE
unsigned short BS_Threshold = 32;	//The maximum GREEN pixel value to be classified as BLUE
unsigned short BV_Threshold = 22;	//The maximum BLUE pixel value to be classified as BLUE
unsigned short BH_Bias = 10;	//The minimum RED pixel value to be classified as RED
unsigned short BS_Bias = 10;	//The maximum GREEN pixel value to be classified as RED
unsigned short BV_Bias = 10;	//The maximum BLUE pixel value to be classified as RED
unsigned short B_LowerBound = 100;
unsigned short B_UpperBound = 1000;
unsigned short B_Level = 10;
unsigned short B_Padding = 20;		//Search range beyond RED Box's border
unsigned short B_Border = 2;		//RED Box's border width

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

int int_min(int A, int B){
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
}

void filterInitialize(int color, Filter *F){
	F->Found = 0;
	F->Color = color;
	F->Size = 0;
	F->Xcenter = 0.0;
	F->Ycenter = 0.0;
	switch(color){
		case R_Color:
			F->H_Threshold = RH_Threshold;
			F->S_Threshold = RS_Threshold;
			F->V_Threshold = RV_Threshold;
			F->H_Bias = RH_Bias;
			F->S_Bias = RS_Bias;
			F->V_Bias = RV_Bias;
			F->LowerBound = R_LowerBound;
			F->UpperBound = R_UpperBound;
			F->Level = R_Level;
			F->Padding = R_Padding;
			F->Border = R_Border;
			break;
		case G_Color:
			/*F->H_Threshold = GH_Threshold;
			F->S_Threshold = GS_Threshold;
			F->V_Threshold = GV_Threshold;
			F->H_Bias = GH_Bias;
			F->S_Bias = GS_Bias;
			F->V_Bias = GV_Bias;
			F->LowerBound = G_LowerBound;
			F->UpperBound = G_UpperBound;
			F->Level = G_Level;
			F->Padding = G_Padding;
			F->Border = G_Border;*/
			break;
		case B_Color:
			F->H_Threshold = BH_Threshold;
			F->S_Threshold = BS_Threshold;
			F->V_Threshold = BV_Threshold;
			F->H_Bias = BH_Bias;
			F->S_Bias = BS_Bias;
			F->V_Bias = BV_Bias;
			F->LowerBound = B_LowerBound;
			F->UpperBound = B_UpperBound;
			F->Level = B_Level;
			F->Padding = B_Padding;
			F->Border = B_Border;
			break;
		default:
			;//error message
	}
}

