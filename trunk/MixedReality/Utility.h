#ifndef INC_UTILITY_H
#define INC_UTILITY_H

#define XLCD	240
#define YLCD	320
#define HEIGHT	200
#define WIDTH	200
#define NUM_RGB 65536
#define PERCENT 100

#define Min(A,B)(A<B ? A:B)
#define Max(A,B)(A>B ? A:B)

enum colorID {rColor, gColor, bColor};

typedef struct{
	int		ballFound;
	int 	ballColor;
	unsigned short 	ballSize;
	float 	xCenter;
	float 	yCenter;
	float 	hThreshold;
	float 	hBias;
	short 	lowerBound;
	unsigned short 	upperBound;
	short 	quantifiedLevel;
	short 	boxPadding;
	short 	boxBorder;
	// Predicted Search Range
	short   xFrom;
	short   xTo;
	short   yFrom;
	short   yTo;
} Filter;

void DrawBox(int xFrom, int xTo, int yFrom, int yTo, int boxBorder, int boxPadding,unsigned short ary2_imgFrame[XLCD][YLCD]);
//void TrackBall(Filter *ptr_oldFilter, unsigned short ary2_imgFrame[XLCD][YLCD], float ary3_rgb2hsvTable[NUM_RGB][3]);
void TrackBall(Filter *ptr_oldFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary3_rgb2hsvTable[NUM_RGB][3]);
//float RGB2Hue(unsigned short rgbColor);
void RGB2HSV(unsigned short rgbColor, unsigned short *h, unsigned short *s, unsigned short *v);
//void RGB2HSV(unsigned short rgbColor, float *ptr_hValue, float *ptr_sValue, float *ptr_vValue);
//unsigned short HSV2RGB(float hValue, float sValue, float vValue);
void RGB2Lab(unsigned short rgbColor, int *L, int *a, int *b);
void InitializeFilter(int ballColor, Filter *ptr_newFilter);
void PLL6713();
unsigned short	ybr_565(short y,short u,short v);
void OverlayImage1D(Filter *ptr_theFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary2_imgInput[HEIGHT][WIDTH]);
void OverlayImage2D(Filter *ptr_leftFilter, Filter *ptr_rightFilter, unsigned short ary3_imgFrame[XLCD][YLCD], unsigned short ary2_imgInput[HEIGHT][WIDTH]);
#endif /* INC_UTILITY_H */

