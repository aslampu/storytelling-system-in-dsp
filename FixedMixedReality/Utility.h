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

//extern int				backgroundAvgL;
//extern int	 			backgroundAvgA;
//extern int				backgroundAvgB;
//extern int				backgroundStdL;
//extern int	 			backgroundStdA;
//extern int				backgroundStdB;
extern float				backgroundAvgL;
extern float	 			backgroundAvgA;
extern float				backgroundAvgB;
extern float				backgroundStdL;
extern float	 			backgroundStdA;
extern float				backgroundStdB;

extern int				imgSizeScale;
extern float			noiseVariance;
//extern int				weightingS;
//extern int				weightingD;
extern float				weightingS;
extern float				weightingD;
extern unsigned short 	displacementThreshold;	

extern int				shadowA;
extern int				shadowB;
extern int				shadowAOffset;
extern int				shadowBOffset;

extern int				rValue;
extern int				gValue;
extern int				bValue;

extern int				rotationPrecisionScale;

/* Adjust */
extern int 				decideLAB_apply;
/* Adjus */

typedef struct{
	unsigned short	ballFound;
	unsigned short 	ballColor;
	int			 	ballSize;
	int 			xCenter;
	int 			yCenter;
	unsigned short 	hThreshold;
	unsigned short 	hBias;
	unsigned short  boxPadding;
	unsigned short  scaleFactor;
	unsigned short  rotatedAngle;
	unsigned short 	lowerBound;
	unsigned short 	quantifiedLevel;
} Filter;

typedef struct {
	short	xCenter;
	short	yCenter;
	short	a;
	short	b;
	int	eccentricity;
} Shadow;


void RGB2HSV(unsigned short rgbColor, unsigned short *h, unsigned short *s, unsigned short *v);
unsigned short HSV2RGB(unsigned short hValue, unsigned short sValue, unsigned short vValue);
void RGB2Lab(unsigned short rgbColor, short *ptr_LValue, short *ptr_aValue, short *ptr_bValue);
unsigned short Lab2RGB(short LValue, short aValue, short bValue);
//void PLL6713();
unsigned short	ybr_565(short y,short u,short v);
void InitializeFilter(int ballColor, Filter *ptr_newFilter);

void TrackBall2D(int xTrackCenter, int yTrackCenter, int trackRange, Filter *ptr_oldFilterGreen, Filter *ptr_oldFilterBlue, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary2_rgb2hsvTable[NUM_RGB][3], short ary2_rgb2labTable[NUM_RGB][3]);
void scaleImage(short scaleFactor100, unsigned short ary2_imgSample[HEIGHT][WIDTH], unsigned short ary2_imgInput[HEIGHT][WIDTH], int rotMatrix[4]);
int getPixelValueBilinear(int pPrime100, int qPrime100, unsigned short ary2_imgSample[HEIGHT][WIDTH]); 
void OverlayImage1D(Filter *ptr_theFilter, unsigned short ary2_imgFrame[XLCD][YLCD], unsigned short ary2_imgInput[HEIGHT][WIDTH]);
void DrawShadow1D(Filter *ptr_theFilter, unsigned short ary2_imgFrame[XLCD][YLCD]);

/*Adjust*/
//void applyLAB(int imgAvgL, int imgAvgA, int imgAvgB, int imgStdL, int imgStdA, int imgStdB, unsigned short ary2_source[HEIGHT][WIDTH],unsigned short ary2_imgInputModified[HEIGHT][WIDTH], short ary2_rgb2labTable[NUM_RGB][3]);
void applyLAB(float imgAvgL, float imgAvgA, float imgAvgB, float imgStdL, float imgStdA, float imgStdB, unsigned short ary2_source[HEIGHT][WIDTH],unsigned short ary2_imgInputModified[HEIGHT][WIDTH], short ary2_rgb2labTable[NUM_RGB][3]);
/**/

#endif /* INC_UTILITY_H */

