#ifndef INC_UTILITY_H
#define INC_UTILITY_H

#define XLCD	240
#define YLCD	320

enum colorID {R_Color, G_Color, B_Color};

//Can be controlled by GEL
extern float	RH_Threshold;// = 26;	//The minimum RED pixel value to be classified as RED
extern float    RS_Threshold;// = 33;	//The maximum GREEN pixel value to be classified as RED
extern float    RV_Threshold;// = 17;	//The maximum BLUE pixel value to be classified as RED
extern float 	RH_Bias;// = 10;	//The minimum RED pixel value to be classified as RED
extern float    RS_Bias;// = 10;	//The maximum GREEN pixel value to be classified as RED
extern float    RV_Bias;// = 10;	//The maximum BLUE pixel value to be classified as RED
extern unsigned short       R_LowerBound;// = 100;
extern unsigned short 		R_UpperBound;// = 1000;
extern unsigned short 		R_Level;// = 10;
extern unsigned short       R_Padding;// = 20;		//Search range beyond RED Box's border
extern unsigned short 		R_Border;// = 2;		//RED Box's border width
/*
extern float 	GH_Threshold;// = 17;	//The minimum RED pixel value to be classified as GREEN
extern float    GS_Threshold;// = 58;	//The maximum GREEN pixel value to be classified as GREEN
extern float    GV_Threshold;// = 17;	//The maximum BLUE pixel value to be classified as GREEN
extern float 	GH_Bias;// = 10;	//The minimum RED pixel value to be classified as RED
extern float    GS_Bias;// = 10;	//The maximum GREEN pixel value to be classified as RED
extern float    GV_Bias;// = 10;	//The maximum BLUE pixel value to be classified as RED
extern unsigned short       G_LowerBound;// = 100;
extern unsigned short 		G_UpperBound;// = 1000;
extern unsigned short 		G_Level;// = 10;
extern unsigned short       G_Padding;// = 20;		//Search range beyond RED Box's border
extern unsigned short 		G_Border;// = 2;		//RED Box's border width
*/
extern float 	BH_Threshold;// = 15;	//The minimum RED pixel value to be classified as BLUE
extern float    BS_Threshold;// = 32;	//The maximum GREEN pixel value to be classified as BLUE
extern float    BV_Threshold;// = 22;	//The maximum BLUE pixel value to be classified as BLUE
extern float 	BH_Bias;// = 10;	//The minimum RED pixel value to be classified as RED
extern float    BS_Bias;// = 10;	//The maximum GREEN pixel value to be classified as RED
extern float    BV_Bias;// = 10;	//The maximum BLUE pixel value to be classified as RED
extern unsigned short      	B_LowerBound;// = 100;
extern unsigned short 	   	B_UpperBound;// = 1000;
extern unsigned short 	   	B_Level;// = 10;
extern unsigned short      	B_Padding;// = 20;		//Search range beyond RED Box's border
extern unsigned short 	   	B_Border;// = 2;		//RED Box's border width

typedef struct{
	int Found;
	int Color;
	int Size;
	float Xcenter;
	float Ycenter;
	float H_Threshold;
	float S_Threshold;
	float V_Threshold;
	float H_Bias;
	float S_Bias;
	float V_Bias;
	short LowerBound;
	short UpperBound;
	short Level;
	short Padding;
	short Border;
} Filter;

void DrawBox(short tmp[][YLCD], int XFrom, int XTo, int YFrom, int YTo, int Border, int Padding);
void track(short tmp[][YLCD], Filter *Former, Filter *Latter);
void RGB2HSV(float rTemp,float gTemp,float bTemp, float *h, float *s, float *v);
void HSV2RGB(float *h, float *s, float *v, float *rTemp, float *gTemp, float *bTemp);
float rgbMax(float R_value, float G_value, float B_value);
float rgbMin(float rTemp, float gTemp, float bTemp);
int int_min(int A, int B);
int int_max(int A, int B);
void filterInitialize(int color, Filter *F);
void PLL6713();
unsigned short	ybr_565(short y,short u,short v);
//void overlayImage(short tmp[][YLCD], Filter *R_Latter, Filter *B_Latter);
#endif /* INC_UTILITY_H */
