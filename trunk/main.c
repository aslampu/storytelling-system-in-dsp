/* ======================================================================== */
/*            Copyright (C) 2008 - 2009 ND-Tech. Co., Ltd.                  */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
/* File     : main.c                            Created  : 13-Mar-2008      */
/* Version  : 2.0.0.0                           Contach  : info@nd-tech.com */
/* ======================================================================== */
/* You are free to use or modify this code to the following restrictions:   */
/*          Acknowledge ND Tech. Co. Ltd.                                   */
/*          Or, put "Parts of code by ND Tech. Co., Ltd."                   */
/*          Or, leave this header as it is.                                 */
/* in somewhere in your code.                                               */
/* ======================================================================== */

#include	"vm3224k.h"
#include    <math.h>

#define		CE2CTL		*(volatile int *)(0x01800010)
// Definitions for async access(change as you wish)
#define		WSU			(2<<28)	 // Write Setup : 0-15
#define		WST			(8<<22)  // Write Strobe: 0-63
#define		WHD			(2<<20)	 // Write Hold  : 0-3
#define		RSU			(2<<16)	 // Read Setup  : 0-15
#define		TA			(3<<14)	 // Turn Around : 0-3
#define		RST			(8<<8)	 // Read Strobe : 0-63
#define		RHD			(2<<0)	 // Read Hold   : 0-3
#define		MTYPE		(2<<4)
#define     XLCD		240
#define     YLCD		320

/* QDMA Registers */
#define		QDMA_OPT	*(volatile int *)(0x02000000)
#define		QDMA_SRC	*(volatile int *)(0x02000004)
#define		QDMA_CNT	*(volatile int *)(0x02000008)
#define		QDMA_DST	*(volatile int *)(0x0200000c)
#define		QDMA_IDX	*(volatile int *)(0x02000010)
#define		QDMA_S_OPT	*(volatile int *)(0x02000020)
#define		QDMA_S_SRC	*(volatile int *)(0x02000024)
#define		QDMA_S_CNT	*(volatile int *)(0x02000028)
#define		QDMA_S_DST	*(volatile int *)(0x0200002c)
#define		QDMA_S_IDX	*(volatile int *)(0x02000030)

/* EDMA Registers */
#define		PaRAM_OPT	0	// Options
#define		PaRAM_SRC	1	// Source Address
#define		PaRAM_CNT	2	// Frame count, Element count
#define		PaRAM_DST	3	// Destination Address
#define		PaRAM_IDX	4	// Frame index, Element index
#define		PaRAM_RDL	5	// Element count reload, Link address
#define		EDMA_CIPR	*(volatile int *)0x01A0FFE4	// EDMA Channel interrupt pending low register
#define		EDMA_CIER	*(volatile int *)0x01A0FFE8	// EDMA Channel interrupt enable low register
#define		EDMA_ER		*(volatile int *)0x01A0FFF0	// EDMA Event low register
#define		EDMA_EER	*(volatile int *)0x01A0FFF4	// EDMA Event enable low register
#define		EDMA_ECR	*(volatile int *)0x01A0FFF8	// EDMA Event clear low register
#define		EDMA_ESR	*(volatile int *)0x01A0FFFC	// EDMA Event set low register

#define 	PRI			(2<<29)	// 1:High priority, 2:Low priority
#define 	ESIZE		(1<<27)	// 0:32bit, 1:16bit, 2:8bit, 3:reserved
#define 	DS2			(0<<26)	// 0:1-Dimensional
#define 	SUM			(0<<24)	// 0:no update, 1:increment, 2:decrement, 3:by index
#define 	DD2			(1<<23)	// 0:1-Dimensional
#define 	DUM			(1<<21)	// 0:no update, 1:increment, 2:decrement, 3:by index
#define 	TCINT		(1<<20)	// 1:enable
#define 	TCC			(9<<16)	// 4 bit code
#define 	LINK		(0<<1)	// 0:disable, 1:enable
#define 	FS			(1<<0)	// 0:disable, 1:enable

#define 	DS2_1		(1<<26)	// 0:1-Dimensional
#define 	SUM_1		(1<<24)	// 0:no update, 1:increment, 2:decrement, 3:by index
#define 	DD2_1		(0<<23)	// 0:1-Dimensional
#define 	DUM_1		(0<<21)	// 0:no update, 1:increment, 2:decrement, 3:by index

#define 	OptionField_0 (PRI|ESIZE|DS2|SUM|DD2|DUM|TCINT|TCC|LINK|FS)
#define 	OptionField_1 (PRI|ESIZE|DS2_1|SUM_1|DD2_1|DUM_1|TCINT|TCC|LINK|FS)

#pragma 	DATA_SECTION ( cam,".sdram" )
#pragma 	DATA_SECTION ( tmp,".sdram" )

short		rgb[64][32][32];
short		cam[XLCD][YLCD];
short 		tmp[XLCD][YLCD];

//Can be controlled by GEL
unsigned short 		RR_Threshold = 26;	//The minimum RED pixel value to be classified as RED
unsigned short      RG_Threshold = 33;	//The maximum GREEN pixel value to be classified as RED
unsigned short      RB_Threshold = 17;	//The maximum BLUE pixel value to be classified as RED
unsigned short		R_Tolerance = 50; 	//The minimum necessary RED pixels to be judged as an object
unsigned short      R_Padding = 20;		//Search range beyond RED Box's border
unsigned short 		R_Border = 2;		//RED Box's border width
/*
unsigned short 		GR_Threshold = 17;	//The minimum RED pixel value to be classified as GREEN
unsigned short      GG_Threshold = 58;	//The maximum GREEN pixel value to be classified as GREEN
unsigned short      GB_Threshold = 17;	//The maximum BLUE pixel value to be classified as GREEN
unsigned short    	G_Tolerance = 50;	//The minimum necessary GREEN pixels to be judged as an object
unsigned short    	G_Padding = 20;		//Search range beyond GREEN Box's border
unsigned short 		G_Border = 2;		//GREEN Box's border width
*/
unsigned short 		BR_Threshold = 15;	//The minimum RED pixel value to be classified as BLUE
unsigned short      BG_Threshold = 32;	//The maximum GREEN pixel value to be classified as BLUE
unsigned short      BB_Threshold = 22;	//The maximum BLUE pixel value to be classified as BLUE
unsigned short   	B_Tolerance = 50;	//The minimum necessary BLUE pixels to be judged as an object
unsigned short    	B_Padding = 20;		//Search range beyond BLUE Box's border
unsigned short 		B_Border = 2;		//BLUE Box's border width

typedef struct Box{
	int Size;
	float Xcenter;
	float Ycenter;
	float Xlength;
	float Ylength;
	int Xstart;		//Include border width
	int Ystart;
	int Xend;
	int Yend;
} Box;

typedef struct LAB{
	float avgL;
	float avgA;
	float avgB;
	float varL;
	float varA;
	float varB;
} LAB;

void PLL6713()
{
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

void RGB2Lab(unsigned char R, unsigned char G, unsigned char B,
	     int *L, int *a, int *b)
{
  float X, Y, Z, fX, fY, fZ;

  X = 0.412453*R + 0.357580*G + 0.180423*B;
  Y = 0.212671*R + 0.715160*G + 0.072169*B;
  Z = 0.019334*R + 0.119193*G + 0.950227*B;

  X /= (255 * 0.950456);
  Y /=  255;
  Z /= (255 * 1.088754);

  if (Y > 0.008856)
    {
      fY = pow(Y, 1.0/3.0);
      *L = (int)(116.0*fY - 16.0 + 0.5);
    }
  else
    {
      fY = 7.787*Y + 16.0/116.0;
      *L = (int)(903.3*Y + 0.5);
    }

  if (X > 0.008856)
      fX = pow(X, 1.0/3.0);
  else
      fX = 7.787*X + 16.0/116.0;

  if (Z > 0.008856)
      fZ = pow(Z, 1.0/3.0);
  else
      fZ = 7.787*Z + 16.0/116.0;

  *a = (int)(500.0*(fX - fY) + 0.5);
  *b = (int)(200.0*(fY - fZ) + 0.5);

//("RGB=(%d,%d,%d) ==> Lab(%d,%d,%d)\n",R,G,B,*L,*a,*b);
}

void Lab2RGB(int L, int a, int b, 
	     unsigned char *R, unsigned char *G, unsigned char *B)
{
  float X, Y, Z, fX, fY, fZ;
  int RR, GG, BB;

  fY = pow((L + 16.0) / 116.0, 3.0);
  if (fY < 0.008856)
      fY = L / 903.3;
  Y = fY;

  if (fY > 0.008856)
    fY = pow(fY, 1.0/3.0);
  else
    fY = 7.787 * fY + 16.0/116.0;

  fX = a / 500.0 + fY;      
  if (fX > 0.206893)
      X = pow(fX, 3.0);
  else
      X = (fX - 16.0/116.0) / 7.787;
 
  fZ = fY - b /200.0;      
  if (fZ > 0.206893)
      Z = pow(fZ, 3.0);
  else
      Z = (fZ - 16.0/116.0) / 7.787;

  X *= (0.950456 * 255);
  Y *=             255;
  Z *= (1.088754 * 255);

  RR =  (int)(3.240479*X - 1.537150*Y - 0.498535*Z + 0.5);
  GG = (int)(-0.969256*X + 1.875992*Y + 0.041556*Z + 0.5);
  BB =  (int)(0.055648*X - 0.204043*Y + 1.057311*Z + 0.5);

  *R = (unsigned char)(RR < 0 ? 0 : RR > 255 ? 255 : RR);
  *G = (unsigned char)(GG < 0 ? 0 : GG > 255 ? 255 : GG);
  *B = (unsigned char)(BB < 0 ? 0 : BB > 255 ? 255 : BB);

//("Lab=(%f,%f,%f) ==> RGB(%f,%f,%f)\n",L,a,b,*R,*G,*B);
}

unsigned short	ybr_565(short y,short u,short v)
{
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

void boxInitialize(Box *A){
	A->Size = 0;
	A->Xcenter = 0.0;
	A->Ycenter = 0.0;
	A->Xlength = 0.0;
	A->Ylength = 0.0;
	A->Xstart = 0;
	A->Ystart = 0;
	A->Xend = XLCD;
	A->Yend = YLCD;
}

int DrawBox(int color, short tmp[][YLCD], Box *NewComer, int XFrom, int XTo, int YFrom, int YTo, int R_Threshold, int G_Threshold, int B_Threshold, int Tolerance, int Border, int Padding){
	//NewComer should be cleared before going into this function
	//XFrom, XTo, YFrom, YTo including Border and Padding means Predicted search range
	int i,j;
	int tmpXstart1, tmpYstart1, tmpXend1, tmpYend1, tmpXstart2, tmpYstart2, tmpXend2, tmpYend2;
	int rTemp,gTemp,bTemp;
	//int L, A, B;
	//int sumL=0;
	//int sumA=0;
	//int sumB=0;
	//int varL=0;
	//int varA=0;
	//int varB=0;
	int first, first_threshold, second, second_threshold, third, third_threshold;
	for(j=XFrom;j<XTo;j++){
		for(i=YFrom;i<YTo;i++){
			rTemp = (tmp[j][i] & 0xf800) >> 11;
			gTemp = (tmp[j][i] & 0x07e0) >> 5;
			bTemp = (tmp[j][i] & 0x001f);
			switch(color){
				case 1://Red Box
					first = rTemp;
					first_threshold = R_Threshold;
					second = gTemp;
					second_threshold = G_Threshold;
					third = bTemp;
					third_threshold = B_Threshold;
					break;
				case 2://Green Box
					first = gTemp;
					first_threshold = G_Threshold;
					second = rTemp;
					second_threshold = R_Threshold;
					third = bTemp;
					third_threshold = B_Threshold;
					break;
				case 3://Blue Box
					first = bTemp;
					first_threshold = B_Threshold;
					second = gTemp;
					second_threshold = G_Threshold;
					third = rTemp;
					third_threshold = R_Threshold;
					break;
				default:
					break;	
			}
			if(first > first_threshold && second < second_threshold && third < third_threshold){
			//if(rTemp > R_Threshold && gTemp < G_Threshold && bTemp < B_Threshold){
				NewComer->Size++;
				NewComer->Xcenter += j;
				NewComer->Ycenter += i;
				NewComer->Xlength += j*j;
				NewComer->Ylength += i*i;
			}
		}
	}
	if(NewComer->Size > Tolerance){// Detect an object
		NewComer->Xcenter = NewComer->Xcenter/NewComer->Size;
		NewComer->Ycenter = NewComer->Ycenter/NewComer->Size;
		NewComer->Xlength = 4 * sqrt(NewComer->Xlength/ NewComer->Size - NewComer->Xcenter * NewComer->Xcenter);
		NewComer->Ylength = 4 * sqrt(NewComer->Ylength/ NewComer->Size - NewComer->Ycenter * NewComer->Ycenter);	
		//with Padding, and with Border
		NewComer->Xstart = floor(NewComer->Xcenter-(1/2) * NewComer->Xlength - Border - Padding);
		NewComer->Ystart = floor(NewComer->Ycenter-(1/2) * NewComer->Ylength - Border - Padding);
		NewComer->Xend = floor(NewComer->Xstart + NewComer->Xlength + 2 * Border + 2 * Padding);
		NewComer->Yend = floor(NewComer->Ystart + NewComer->Ylength + 2 * Border + 2 * Padding);
		//without Padding, but with Border
		tmpXstart1 = int_min(XLCD, int_max(0, NewComer->Xstart + Padding));
		tmpYstart1 = int_min(YLCD, int_max(0, NewComer->Ystart + Padding));
		tmpXend1 = int_min(XLCD, int_max(0, NewComer->Xend - Padding));
		tmpYend1 = int_min(YLCD, int_max(0, NewComer->Yend - Padding));
		//without Padding and without Border
		tmpXstart2 = int_min(XLCD, int_max(0, NewComer->Xstart + Padding + Border));
		tmpYstart2 = int_min(YLCD, int_max(0, NewComer->Ystart + Padding + Border));
		tmpXend2 = int_min(XLCD, int_max(0, NewComer->Xend - Padding - Border));
		tmpYend2 = int_min(YLCD, int_max(0, NewComer->Yend - Padding - Border));
		
		//show Box in tmp
		//add black:0 UP
		for(j=tmpXstart1;j<tmpXstart2;j++){
			for(i=tmpYstart1;i<tmpYend1;i++){
				tmp[j][i] = 0;
			}
		}
		//Down
		for(j=tmpXend2;j<tmpXend1;j++){
			for(i=tmpYstart1;i<tmpYend1;i++){
				tmp[j][i] = 0;
			}
		}
		//Left
		for(i=tmpYstart1;i<tmpYstart2;i++){
			for(j=tmpXstart1;j<tmpXend1;j++){
				tmp[j][i] = 0;
			}
		}
		//Right
		for(i=tmpYend2;i<tmpYend1;i++){
			for(j=tmpXstart1;j<tmpXend1;j++){
				tmp[j][i] = 0;
			}
		}
		/* compute Lab
		for(j=tmpXstart2; j<tmpXend2;j++){
			for(i=tmpYstart2; i<tmpYend2; i++){
				L=0;
				a=0;
				b=0;
				rTemp = (tmp[j][i] & 0xf800) >> 11;
				gTemp = (tmp[j][i] & 0x07e0) >> 5;
				bTemp = (tmp[j][i] & 0x001f);
				RGB2Lab(rTemp, gTemp, bTemp, &L, &A, &B);
				avgL = avgL + L;
				avgA = avgA + A;
				avgB = avgB + B;
				varL = varL + L*L;
				varA = varA + A*A;
				varB = varB + B*B;
			}
		}
		avgL = avgL/((tmpXend2-tmpXstart2)*(tmpYend2-tmpYstart2));
		avgA = avgA/((tmpXend2-tmpXstart2)*(tmpYend2-tmpYstart2));
		avgB = avgB/((tmpXend2-tmpXstart2)*(tmpYend2-tmpYstart2));
		varL = sqrt(varL/((tmpXend2-tmpXstart2)*(tmpYend2-tmpYstart2)) - avgL * avgL);
		varA = sqrt(varA/((tmpXend2-tmpXstart2)*(tmpYend2-tmpYstart2)) - avgA * avgA);
		varB = sqrt(varB/((tmpXend2-tmpXstart2)*(tmpYend2-tmpYstart2)) - avgB * avgB);

		//get image's avgL avgA avgB varL varA varB
		//normalize and use Lab2RGB
		//overlay normalized image to background
		*/
		return 1;
	}else{ 
		//Losing the object, so no box drawing anymore
		return 0;
	}
}

void main()
{
	int 	i,j,k,y0,y1,v0,u0;
	int 				R_XFrom=0;
	int                 R_XTo=0;
	int                 R_YFrom=0;
	int					R_YTo=0;
	/*
	int 				G_XFrom=0;
	int                 G_XTo=0;
	int                 G_YFrom=0;
	int					G_YTo=0;
	*/
	int 				B_XFrom=0;
	int                 B_XTo=0;
	int                 B_YFrom=0;
	int					B_YTo=0;
	
	Box R_Former, R_Latter, R_NewComer;
	//Box G_Former, G_Latter, G_NewComer;
	Box B_Former, B_Latter, B_NewComer;
	
	enum colorID {ALL_Color, R_Color, G_Color, B_Color};
	//precompute image LAB

	boxInitialize(&R_Former);
	boxInitialize(&R_Latter);
	boxInitialize(&R_NewComer);
	/*
	boxInitialize(&G_Former);
	boxInitialize(&G_Latter);
	boxInitialize(&G_NewComer);
	*/
	boxInitialize(&B_Former);
	boxInitialize(&B_Latter);
	boxInitialize(&B_NewComer);
	
	PLL6713();					// Initialize C6713 PLL	
	CE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPE);
	vm3224init();
	vm3224rate(3);
	vm3224bl(15);

    for (k=0;k<64;k++)
    for (i=0;i<32;i++)
    for (j=0;j<32;j++) rgb[k][i][j] = ybr_565(k<<2,i<<3,j<<3);

	QDMA_CNT 	= (239<<16)|320;
	QDMA_IDX 	= 0x0000<<16;
	
	while (1) {
		for(i=0;i<1000000;i++) if(EDMA_CIPR&0x200) break;		
		VM3224ADDH = 0x08000;		
		for(i=0;i<200;i++);		
		EDMA_CIPR = 0x200;	
		QDMA_SRC	= (int)&VM3224DATA;
		QDMA_DST 	= (int)cam;			
		QDMA_S_OPT 	= OptionField_0;

		for (j=0;j<XLCD;j++)
		for (i=0;i<YLCD;i+=2) {
			y0 = (cam[j][i]>>8) & 0x0ff;
			u0 = cam[j][i] & 0x0ff;
			y1 = (cam[j][i+1]>>8) & 0x0ff;
			v0 = cam[j][i+1] & 0x0ff;
			y0 = y0>>2;
			y1 = y1>>2;
			u0 = u0>>3;
			v0 = v0>>3;
			tmp[j][i]=rgb[y0][u0][v0];
			tmp[j][i+1]=rgb[y1][u0][v0];
		}
		
		R_XFrom = int_min(XLCD, int_max(0,2 * R_Latter.Xstart - R_Former.Xstart));
		R_YFrom = int_min(YLCD, int_max(0,2 * R_Latter.Ystart - R_Former.Ystart));
		R_XTo = int_min(XLCD, int_max(0,2 * R_Latter.Xend - R_Former.Xend));
		R_YTo = int_min(YLCD, int_max(0,2 * R_Latter.Yend - R_Former.Yend));
		/*
		G_XFrom = int_min(XLCD, int_max(0,2 * G_Latter.Xstart - G_Former.Xstart));
		G_YFrom = int_min(YLCD, int_max(0,2 * G_Latter.Ystart - G_Former.Ystart));
		G_XTo = int_min(XLCD, int_max(0,2 * G_Latter.Xend - G_Former.Xend));
		G_YTo = int_min(YLCD, int_max(0,2 * G_Latter.Yend - G_Former.Yend));
		*/
		B_XFrom = int_min(XLCD, int_max(0,2 * B_Latter.Xstart - B_Former.Xstart));
		B_YFrom = int_min(YLCD, int_max(0,2 * B_Latter.Ystart - B_Former.Ystart));
		B_XTo = int_min(XLCD, int_max(0,2 * B_Latter.Xend - B_Former.Xend));
		B_YTo = int_min(YLCD, int_max(0,2 * B_Latter.Yend - B_Former.Yend));
		
		boxInitialize(&R_NewComer);
		//boxInitialize(&G_NewComer);
		boxInitialize(&B_NewComer);

		if(DrawBox(R_Color, tmp, &R_NewComer, R_XFrom, R_XTo, R_YFrom, R_YTo, RR_Threshold, RG_Threshold, RB_Threshold, R_Tolerance, R_Border, R_Padding)){
			R_Former = R_Latter;
			R_Latter = R_NewComer;
		}else{
			boxInitialize(&R_NewComer);//if losing tracking of the object, then reset
			boxInitialize(&R_Former);
			boxInitialize(&R_Latter);
		}
			
		/*if(DrawBox(G_Color, tmp, &G_NewComer, G_XFrom, G_XTo, G_YFrom, G_YTo, GR_Threshold, GG_Threshold, GB_Threshold, G_Tolerance, G_Border, G_Padding)){
			G_Former = G_Latter;
			G_Latter = G_NewComer;
		}else{
			boxInitialize(&G_NewComer);//if losing tracking of the object, then reset
			boxInitialize(&G_Former);
			boxInitialize(&G_Latter);
		}*/
		if(DrawBox(B_Color, tmp, &B_NewComer, B_XFrom, B_XTo, B_YFrom, B_YTo, BR_Threshold, BG_Threshold, BB_Threshold, B_Tolerance, B_Border, B_Padding)){
			B_Former = B_Latter;
			B_Latter = B_NewComer;
		}else{
			boxInitialize(&B_NewComer);//if losing tracking of the object, then reset
			boxInitialize(&B_Former);
			boxInitialize(&B_Latter);
		}
		
		for(i=0;i<1000000;i++) if(EDMA_CIPR&0x200) break;
		VM3224ADDH = 0x0000;		
		EDMA_CIPR = 0x200;	
		QDMA_SRC	= (int)tmp;
		QDMA_DST 	= (int)&VM3224DATA;		
		QDMA_S_OPT 	= OptionField_1;				
	}
}
