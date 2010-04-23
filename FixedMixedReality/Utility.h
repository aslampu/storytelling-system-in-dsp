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
#define Guard(G,A,B)(Min(Max(G,A),B))//A:min, B:max
//Min(100,floor((x-min/max)*lvl)*lvl)
enum colorID {rColor, gColor, bColor};

extern unsigned short 	rhThreshold;
extern unsigned short 	rhBias;				
extern unsigned short 	rLowerBound;
extern unsigned short 	rUpperBound;
extern unsigned short 	rQuantifiedLevel;
extern unsigned short 	rBoxPadding;		
extern unsigned short 	rBoxBorder;			

extern unsigned short 	ghThreshold;			
extern unsigned short 	ghBias;			
extern unsigned short 	gLowerBound;
extern unsigned short   gUpperBound;
extern unsigned short   gQuantifiedLevel;
extern unsigned short   gBoxPadding;			
extern unsigned short   gBoxBorder;				

extern unsigned short 	bhThreshold;		
extern unsigned short	bhBias;			
extern unsigned short 	bLowerBound;
extern unsigned short 	bUpperBound;
extern unsigned short 	bQuantifiedLevel;
extern unsigned short 	bBoxPadding;			
extern unsigned short 	bBoxBorder;

extern float			backgroundAvgL;
extern float 			backgroundAvgA;
extern float			backgroundAvgB;
extern float			backgroundStdL;
extern float 			backgroundStdA;
extern float			backgroundStdB;

extern int				imgSizeScale;
extern float			noiseVariance;
extern float			weightingS;
extern float			weightingD;
extern unsigned short 	displacementThreshold;	
//extern int				randomNoise100;

extern int				shadowA;
extern int				shadowB;
extern int				shadowAOffset;
extern int				shadowBOffset;


/* Adjust */
extern int decideLAB_apply;
/* Adjus */

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
	/*short   xFrom;
	short   xTo;
	short   yFrom;
	short   yTo;
	float   avgL;
	float   avgA;
	float   avgB;
	float   stdL;
	float   stdA;
	float   stdB;*/
	int     scaleFactor;
} Filter;

typedef struct {
	int		xCenter;
	int		yCenter;
	int		a;
	int		b;
	double	eccentricity;
} Shadow;

void TrackBall2D(int xTrackCenter, int yTrackCenter, int trackRange, Filter *ptr_oldFilterGreen, Filter *ptr_oldFilterBlue, unsigned short ary2_imgFrame[XLCD][YLCD], float ary2_rgb2hsvTable[NUM_RGB][3], short ary2_rgb2labTable[NUM_RGB][3]);
//void TrackBall(Filter *ptr_oldFilter, unsigned short ary2_imgFrame[XLCD][YLCD], float ary2_rgb2hsvTable[NUM_RGB][3], short ary2_rgb2labTable[NUM_RGB][3]);
void DrawBox(int xFrom, int xTo, int yFrom, int yTo, int boxBorder, int boxPadding,unsigned short ary2_imgFrame[XLCD][YLCD]);
void DebugBall(Filter *ptr_oldFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary2_rgb2hsvTable[NUM_RGB][3]);
void RGB2HSV(unsigned short rgbColor, float *h, float *s, float *v);
unsigned short HSV2RGB(float hValue, float sValue, float vValue);
//void RGB2HSV(unsigned short rgbColor, unsigned short *h, unsigned short *s, unsigned short *v);
//unsigned short HSV2RGB(unsigned short hValue, unsigned short sValue, unsigned short vValue);
void RGB2Lab(unsigned short rgbColor, short *ptr_LValue, short *ptr_aValue, short *ptr_bValue);
unsigned short Lab2RGB(short LValue, short aValue, short bValue);
void InitializeFilter(int ballColor, Filter *ptr_newFilter);
void PLL6713();
unsigned short	ybr_565(short y,short u,short v);
void scaleImage(short scaleFactor100, unsigned short ary2_imgSample[HEIGHT][WIDTH], unsigned short ary2_imgInput[HEIGHT][WIDTH]);
int getPixelValueBilinear(float pPrime, float qPrime, unsigned short ary2_imgSample[HEIGHT][WIDTH]); 
float getPixelValueBilinearOriginal(float pPrime, float qPrime, unsigned short ary2_imgSample[HEIGHT][WIDTH]);


//void OverlayImage1D(float imgAvgL, float imgAvgA, float imgAvgB, float imgStdL, float imgStdA, float imgStdB, Filter *ptr_theFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary2_imgInput[HEIGHT][WIDTH], unsigned short ary_lab2rgbTable[NUM_RGB], unsigned short ary_lab2rgbTable[NUM_RGB]);
//void OverlayImage1D(float imgAvgL, float imgAvgA, float imgAvgB, float imgStdL, float imgStdA, float imgStdB, Filter *ptr_theFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary2_imgInput[HEIGHT][WIDTH],short	ary2_rgb2labTable[NUM_RGB][3]);
void OverlayImage1D(Filter *ptr_theFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary2_imgInput[HEIGHT][WIDTH]);



//void OverlayImage2D(float imgAvgL, float imgAvgA, float imgAvgB, float imgStdL, float imgStdA, float imgStdB, Filter *ptr_leftFilter, Filter *ptr_rightFilter, unsigned short ary3_imgFrame[XLCD][YLCD], unsigned short ary2_imgInput[HEIGHT][WIDTH], unsigned short ary_lab2rgbTable[NUM_RGB], unsigned short ary_lab2rgbTable[NUM_RGB]);
void OverlayImage2D(Filter *ptr_leftFilter, Filter *ptr_rightFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary2_imgInput[HEIGHT][WIDTH]);
void DrawShadow1D(Filter *ptr_theFilter, unsigned short ary2_imgFrame[XLCD][YLCD]);


/*Adjust*/
void applyLAB(float imgAvgL, float imgAvgA, float imgAvgB, float imgStdL, float imgStdA, float imgStdB,unsigned short ary2_source[HEIGHT][WIDTH],unsigned short ary2_imgInputModified[HEIGHT][WIDTH], short	ary2_rgb2labTable[NUM_RGB][3]);
/**/

#endif /* INC_UTILITY_H */

