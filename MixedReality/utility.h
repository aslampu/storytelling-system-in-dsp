#ifndef INC_UTILITY_H
#define INC_UTILITY_H

#define XLCD	240
#define YLCD	320

enum colorID {R_Color, G_Color, B_Color};

//Can be controlled by GEL
extern unsigned short		RH_Threshold;// = 26;	//The minimum RED pixel value to be classified as RED
extern unsigned short       RS_Threshold;// = 33;	//The maximum GREEN pixel value to be classified as RED
extern unsigned short       RV_Threshold;// = 17;	//The maximum BLUE pixel value to be classified as RED
extern unsigned short 		RH_Bias;// = 10;	//The minimum RED pixel value to be classified as RED
extern unsigned short       RS_Bias;// = 10;	//The maximum GREEN pixel value to be classified as RED
extern unsigned short       RV_Bias;// = 10;	//The maximum BLUE pixel value to be classified as RED
extern unsigned short       R_LowerBound;// = 100;
extern unsigned short 		R_UpperBound;// = 1000;
extern unsigned short 		R_Level;// = 10;
extern unsigned short       R_Padding;// = 20;		//Search range beyond RED Box's border
extern unsigned short 		R_Border;// = 2;		//RED Box's border width
/*
extern unsigned short 		GH_Threshold;// = 17;	//The minimum RED pixel value to be classified as GREEN
extern unsigned short       GS_Threshold;// = 58;	//The maximum GREEN pixel value to be classified as GREEN
extern unsigned short       GV_Threshold;// = 17;	//The maximum BLUE pixel value to be classified as GREEN
extern unsigned short 		GH_Bias;// = 10;	//The minimum RED pixel value to be classified as RED
extern unsigned short       GS_Bias;// = 10;	//The maximum GREEN pixel value to be classified as RED
extern unsigned short       GV_Bias;// = 10;	//The maximum BLUE pixel value to be classified as RED
extern unsigned short       G_LowerBound;// = 100;
extern unsigned short 		G_UpperBound;// = 1000;
extern unsigned short 		G_Level;// = 10;
extern unsigned short       G_Padding;// = 20;		//Search range beyond RED Box's border
extern unsigned short 		G_Border;// = 2;		//RED Box's border width
*/
extern unsigned short 	   	BH_Threshold;// = 15;	//The minimum RED pixel value to be classified as BLUE
extern unsigned short      	BS_Threshold;// = 32;	//The maximum GREEN pixel value to be classified as BLUE
extern unsigned short      	BV_Threshold;// = 22;	//The maximum BLUE pixel value to be classified as BLUE
extern unsigned short 	   	BH_Bias;// = 10;	//The minimum RED pixel value to be classified as RED
extern unsigned short      	BS_Bias;// = 10;	//The maximum GREEN pixel value to be classified as RED
extern unsigned short      	BV_Bias;// = 10;	//The maximum BLUE pixel value to be classified as RED
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
	short H_Threshold;
	short S_Threshold;
	short V_Threshold;
	short H_Bias;
	short S_Bias;
	short V_Bias;
	short LowerBound;
	short UpperBound;
	short Level;
	short Padding;
	short Border;
} Filter;

//void DrawBox(short tmp[][YLCD], int XFrom, int XTo, int YFrom, int YTo, int Border, int Padding);
//void track(short tmp[][YLCD], Filter *Former, Filter *Latter);
//void overlayImage(short tmp[][YLCD], Filter *R_Latter, Filter *B_Latter);
//void RGB2HS/V(float rTemp,float gTemp,float bTemp, float *h, float *s, float *v);
//void HSV2RGB(float *h, float *s, float *v, float *rTemp, float *gTemp, float *bTemp);

#endif /* INC_UTILITY_H */
