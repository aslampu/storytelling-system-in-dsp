/* ======================================================================== */
/*            Copyright (C) 2008 - 2009 ND-Tech. Co., Ltd.                  */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
/* Project  : VideoAudioLoopback                Created  : 03-MAR-2009      */
/* File     : main.c                            Contact  : info@nd-tech.com */
/* ======================================================================== */
/* You are free to use or modify this code to the following restrictions:   */
/*          Acknowledge ND Tech. Co. Ltd.                                   */
/*          Or, put "Parts of code by ND Tech. Co., Ltd."                   */
/*          Or, leave this header as it is.                                 */
/* in somewhere in your code.                                               */
/* ======================================================================== */

#include	"c6000.h"
#include	"dsxio.h"
#include	"vm3224k.h"
#include	"dsk6713.h"
#include	"dsk6713_aic23.h"
#include	"stdio.h"

#include    "SubImage.h"
//#include    "Acrylic Paint.h"
#include "Paint.h"
//=================================================================== Additional Lib 
#include 	"math.h"
//===================================================================

// Definitions for async access(change as you wish)
#define		WSU			(2<<28)	 // Write Setup : 0-15
#define		WST			(8<<22)  // Write Strobe: 0-63
#define		WHD			(2<<20)	 // Write Hold  : 0-3
#define		RSU			(2<<16)	 // Read Setup  : 0-15
#define		TA			(3<<14)	 // Turn Around : 0-3
#define		RST			(8<<8)	 // Read Strobe : 0-63
#define		RHD			(2<<0)	 // Read Hold   : 0-3
#define		MTYPEA		(2<<4)

//#define	X_SHIFT	160 - (subImageWidth/2)
//#define	Y_SHIFT	120 - (subImageHeight/2)

#pragma 	DATA_SECTION ( lcd,".sdram" )
#pragma 	DATA_SECTION ( cam,".sdram" )
#pragma 	DATA_SECTION ( image,".sdram" )
#pragma 	DATA_SECTION ( sub_image,".sdram" )

short		lcd[240][320];
short		cam[240][320];
short		rgb[64][32][32];
int			flag=0;
DSK6713_AIC23_CodecHandle hCodec;

extern cregister volatile unsigned int IER;
extern cregister volatile unsigned int CSR;

//===================================================================
//---For GEL
short imageScaleFactor = 100;

//---For RGB_HSV transform

short rgbOri;
short rgbAdj;
float rTemp, gTemp, bTemp;
float hsv[3];

//---For scaled image
int subImageHeight = HEIGHT;
int subImageWidth = WIDTH;
//int	X_SHIFT	= 160 - (subImageWidth/2);
//int	Y_SHIFT	= 120 - (subImageHeight/2);

//===================================================================


DSK6713_AIC23_Config config = {
    0x0017, // 0 DSK6713_AIC23_LEFTINVOL  Left line input channel volume
    0x0017, // 1 DSK6713_AIC23_RIGHTINVOL Right line input channel volume
    0x0079, // 2 DSK6713_AIC23_LEFTHPVOL  Left channel headphone volume
    0x0079, // 3 DSK6713_AIC23_RIGHTHPVOL Right channel headphone volume
    0x0015, // 4 DSK6713_AIC23_ANAPATH    Analog audio path control
    0x0000, // 5 DSK6713_AIC23_DIGPATH    Digital audio path control
    0x0000, // 6 DSK6713_AIC23_POWERDOWN  Power down control
    0x0043, // 7 DSK6713_AIC23_DIGIF      Digital audio interface format
    0x0001, // 8 DSK6713_AIC23_SAMPLERATE Sample rate control
    0x0001  // 9 DSK6713_AIC23_DIGACT     Digital interface activation
};

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

unsigned short	ybr_565(short y,short u,short v)
{
	int		r,g,b;

 	b = y + 1.772*(u-128);
	if (b<0) b=0;
	if (b>255) b=255;
	g = y - 0.344*(u-128) - 0.714*(v-128);
	if (g<0) g=0;
	if (g>255) g=255;
	r = y + 1.402*(v-128);
	if (r<0) r=0;
	if (r>255) r=255;
	return ((r&0x0f8)<<8)|((g&0x0fc)<<3)|((b&0x0f8)>>3);
}

void intr_disable(int num)
{
	IER = IER & ~(1<<num);
}

void intr_select(int vector, int select)
{
	if(vector>INT9) {
		vector -= 32;
		MUXH = (MUXH & ~(0x1f<<vector)) | (select<<vector);
	}
	else
		MUXL = (MUXL & ~(0x1f<<vector)) | (select<<vector);
}

void intr_enable(int num)
{
	IER = IER | (1<<num) | 3;
	CSR = CSR | 0x1;
}

interrupt void	c_int06(void)
{
	flag = 1;
}

interrupt void	c_int15(void)
{
	DXR1 = DRR1;
}


//===================================================================
float getPixelValueBilinear(float pPrime, float qPrime) {
	// This function returns the pixel value at a non-integeral
	// coordinates of the image using bilinear interpolation.
	// If the values are outside of the bounds of the image,
	// it returns white (255).
	int lowerP = pPrime;
	int lowerQ = qPrime;
	float a = pPrime - lowerP;
	float b = qPrime - lowerQ;

	float r00, r01, r10, r11, rPrime;
	float g00, g01, g10, g11, gPrime;
	float b00, b01, b10, b11, bPrime;
	
	//if (pPrime < 0 || pPrime >= HEIGHT - 1 || qPrime < 0 || qPrime >= WIDTH - 1)
	//	return BG_COLOR;
	
	// R
	r00 = (float)((image[lowerP][lowerQ]&0xF800)>>11) / 31;
	r01 = (float)((image[lowerP + 1][lowerQ]&0xF800)>>11) / 31;
	r10 = (float)((image[lowerP][lowerQ + 1]&0xF800)>>11) / 31;
	r11 = (float)((image[lowerP + 1][lowerQ + 1]&0xF800)>>11) / 31;
	rPrime = (1-a) * (1-b) * r00 +
			 (1-a) *   b   * r01 +
			   a   * (1-b) * r10 +
			   a   *   b   * r11;

	// G
	g00 = (float)((image[lowerP][lowerQ]&0x7E0)>>5) / 63;
	g01 = (float)((image[lowerP + 1][lowerQ]&0x7E0)>>5) / 63;
	g10 = (float)((image[lowerP][lowerQ + 1]&0x7E0)>>5) / 63;
	g11 = (float)((image[lowerP + 1][lowerQ + 1]&0x7E0)>>5) / 63;
	gPrime = (1-a) * (1-b) * g00 +
			 (1-a) *   b   * g01 +
			   a   * (1-b) * g10 +
			   a   *   b   * g11;
    
	// B
	b00 = (float)(image[lowerP][lowerQ]&0x1F) / 31;
	b01 = (float)(image[lowerP + 1][lowerQ]&0x1F) / 31;
	b10 = (float)(image[lowerP][lowerQ + 1]&0x1F) / 31;
	b11 = (float)(image[lowerP + 1][lowerQ + 1]&0x1F) / 31;
	bPrime = (1-a) * (1-b) * b00 +
			 (1-a) *   b   * b01 +
			   a   * (1-b) * b10 +
			   a   *   b   * b11;
    

	rPrime = rPrime * 31;
	gPrime = gPrime * 63;
	bPrime = bPrime * 31;

	return (((short)rPrime)<<11)|(((short)gPrime)<<5)|(((short)bPrime));
}


void scaleImage() {
	int j,k;
	double pPrime, qPrime;
	
	subImageHeight = HEIGHT * imageScaleFactor / 100.0;
	subImageWidth = WIDTH * imageScaleFactor / 100.0;
	
	for (j=0; j < subImageHeight; j++) {
		for (k=0; k < subImageWidth; k++) {
			pPrime = (double)j / (imageScaleFactor / 100.0);
			qPrime = (double)k / (imageScaleFactor / 100.0);
			sub_image[j][k] = getPixelValueBilinear(pPrime, qPrime);
		}
	}
}



//===================================================================


void main()
{
	int i,j,k,y0,y1,v0,u0;
	FILE * outputDat;
	PLL6713();			// Initialize C6713 PLL	
	CE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPEA);

	intr_disable(6);
	intr_disable(15);
	intr_select(INT6,6);
	intr_select(INT15,RINT1);

	hCodec = DSK6713_AIC23_openCodec(0, &config);   // Set codec control registers and get codec handle

	vm3224init();       // Initialize vm3224k2
	vm3224rate(3);		// Set frame rate
	vm3224bl(15);       // Set backlight
	for (k=0;k<64;k++)  // Create RGB565 lookup table
    for (i=0;i<32;i++)
    for (j=0;j<32;j++) rgb[k][i][j] = ybr_565(k<<2,i<<3,j<<3);
	VM3224CNTL = VM3224CNTL&0xffff | 0x2;

	intr_enable(6);
	intr_enable(15);

	while(1) {
		if(flag) {
			VM3224ADDH = 0x08000;
			for (j=0;j<240;j++)
			for (i=0;i<320;i++) cam[j][i]=VM3224DATA;

			for (j=0;j<240;j++)
			for (i=0;i<320;i+=2) {
				y0 = (cam[j][i]>>8) & 0x0ff;
				u0 = cam[j][i] & 0x0ff;
				y1 = (cam[j][i+1]>>8) & 0x0ff;
				v0 = cam[j][i+1] & 0x0ff;
				y0 = y0>>2;
				y1 = y1>>2;
				u0 = u0>>3;
				v0 = v0>>3;
				lcd[j][i]=rgb[y0][u0][v0];
				lcd[j][i+1]=rgb[y1][u0][v0];
			}

			//============================Capture Image Intruction
			//1. Set the breakpoint in scaleImage(), runs until back to the breakpoint. 
			//2. Run this routine several times
			//3. Then you can see the written imag.
            scaleImage();
			//outputDat = fopen("Z:\\david On My Mac\\Desktop\\output.dat", "wb");
			outputDat = fopen("C:\\Shadow_Anaylsis\\output.dat", "wb");
			/*
			for(i = 0; i < subImageWidth; i++)
			{
				for(j = 0; j < subImageHeight; j++)
				{
					
					if((sub_image[j][i] - ((short)0xFFFF)) >= (1))
					{ 
						//lcd[j][i] = 256;
						
						rTemp = ((float)((sub_image[j][i]&0xF800)>>11));
						gTemp = ((float)((sub_image[j][i]&0x7E0)>>5));
						bTemp = ((float)(sub_image[j][i]&0x1F));

						rgbAdj = (((short)rTemp)<<11)|(((short)gTemp)<<5)|(((short)bTemp));
						lcd[j+ 120 - (subImageHeight / 2)][i+ 160 - (subImageWidth / 2)] = rgbAdj;
						
					}
				}
			}
			*/
			
			
			
			
			VM3224ADDH = 0x0000;
			for (j=0;j<240;j++)
			for (i=0;i<320;i++) VM3224DATA=lcd[j][i];
			flag = 0;
			for (j=0;j<240;j++)
			for (i=0;i<320;i++) {
			fputc( (int)(((lcd[j][i]&0xF800)>>11) / 31.0 * 255.0), outputDat);
			fputc( (int)(((lcd[j][i]&0x7E0)>>5) / 63.0 * 255.0), outputDat);
			fputc( (int)(((lcd[j][i]&0x1F)) / 31.0 * 255.0), outputDat);
			}
			fclose(outputDat);
		}
	}
}
