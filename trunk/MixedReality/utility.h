#ifndef INC_UTILITY_H
#define INC_UTILITY_H

#define XLCD	240
#define YLCD	320

#define Min(A,B)(A<B ? A:B)
#define Max(A,B)(A>B ? A:B)

enum colorID {rColor, gColor, bColor};

//Can be controlled by GEL
extern float			rhThreshold;// = 26;	//The minimum RED pixel value to be classified as RED
extern float    		rsThreshold;// = 33;	//The maximum GREEN pixel value to be classified as RED
extern float    		rvThreshold;// = 17;	//The maximum BLUE pixel value to be classified as RED
extern float 			rhBias;		// = 10;	//The minimum RED pixel value to be classified as RED
extern float    		rsBias;		// = 10;	//The maximum GREEN pixel value to be classified as RED
extern float    		rvBias;		// = 10;	//The maximum BLUE pixel value to be classified as RED
extern unsigned short   rLowerBound;// = 100;
extern unsigned short 	rUpperBound;// = 1000;
extern unsigned short 	rQuantifiedLevel;// = 10;
extern unsigned short   rBoxPadding;// = 20;	//Search range beyond RED Box's border
extern unsigned short 	rBoxBorder;	// = 2;		//RED Box's border width
/*
extern float 			ghThreshold;// = 17;	//The minimum RED pixel value to be classified as GREEN
extern float    		gsThreshold;// = 58;	//The maximum GREEN pixel value to be classified as GREEN
extern float    		gvThreshold;// = 17;	//The maximum BLUE pixel value to be classified as GREEN
extern float 			ghBias;// = 10;			//The minimum RED pixel value to be classified as RED
extern float    		gsBias;// = 10;			//The maximum GREEN pixel value to be classified as RED
extern float    		gvBias;// = 10;			//The maximum BLUE pixel value to be classified as RED
extern unsigned short   gLowerBound;// = 100;
extern unsigned short 	gUpperBound;// = 1000;
extern unsigned short 	gQuantifiedLevel;// = 10;
extern unsigned short   gBoxPadding;// = 20;	//Search range beyond RED Box's border
extern unsigned short 	gBoxBorder;// = 2;		//RED Box's border width
*/
extern float 			bhThreshold;// = 15;	//The minimum RED pixel value to be classified as BLUE
extern float    		bsThreshold;// = 32;	//The maximum GREEN pixel value to be classified as BLUE
extern float    		bvThreshold;// = 22;	//The maximum BLUE pixel value to be classified as BLUE
extern float 			bhBias;// = 10;			//The minimum RED pixel value to be classified as RED
extern float    		bsBias;// = 10;			//The maximum GREEN pixel value to be classified as RED
extern float    		bvBias;// = 10;			//The maximum BLUE pixel value to be classified as RED
extern unsigned short   bLowerBound;// = 100;
extern unsigned short 	bUpperBound;// = 1000;
extern unsigned short 	bQuantifiedLevel;// = 10;
extern unsigned short   bBoxPadding;// = 20;	//Search range beyond RED Box's border
extern unsigned short 	bBoxBorder;// = 2;		//RED Box's border width

typedef struct{
	int		ballFound;
	int 	ballColor;
	int 	ballSize;
	float 	xCenter;
	float 	yCenter;
	float 	hThreshold;
	float 	sThreshold;
	float 	vThreshold;
	float 	hBias;
	float 	sBias;
	float 	vBias;
	short 	lowerBound;
	short 	upperBound;
	short 	quantifiedLevel;
	short 	boxPadding;
	short 	boxBorder;
} Filter;

void DrawBox(int xFrom, int xTo, int yFrom, int yTo, int boxBorder, int boxPadding, short ary2_imgFrame[XLCD][YLCD]);
void TrackBall(Filter *ptr_formerFilter, Filter *ptr_latterFilter, short ary2_imgFrame[XLCD][YLCD]);
void RGB2HSV(float rTemp,float gTemp,float bTemp, float *ptr_hValue, float *ptr_sValue, float *ptr_vValue);
void HSV2RGB(float *ptr_hValue, float *ptr_sValue, float *ptr_vValue, float *ptr_rTemp, float *ptr_gTemp, float *ptr_bTemp);
float rgbMax(float rTemp, float gTemp, float bTemp);
float rgbMin(float rTemp, float gTemp, float bTemp);
//int int_min(int A, int B);
//int int_max(int A, int B);
void InitializeFilter(int ballColor, Filter *ptr_newFilter);
void PLL6713();
unsigned short	ybr_565(short y,short u,short v);
void OverlayImage(Filter *ptr_leftFilter, Filter *ptr_rightFilter, int numRow, int numCol, short ary2_imgFrame[XLCD][YLCD]);
#endif /* INC_UTILITY_H */
