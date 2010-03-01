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

#include    "Acrylic Paint.h"

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

#define 	X_SHIFT		160 - (WIDTH/2)
#define		Y_SHIFT		120 - (HEIGHT/2)


#pragma 	DATA_SECTION ( lcd,".sdram" )
#pragma 	DATA_SECTION ( cam,".sdram" )
#pragma 	DATA_SECTION ( image,".sdram" )

short		lcd[240][320];
short		cam[240][320];
short		rgb[64][32][32];
int			flag=0;
DSK6713_AIC23_CodecHandle hCodec;

extern cregister volatile unsigned int IER;
extern cregister volatile unsigned int CSR;

//=================================================================== LAB_GEL Variable
//---For GEL
short doLAB = 0; //if doLAB is '0', the frame will be original
short negativeL = 0;
short negativeA = 0;
short negativeB = 0;
short l_adjust = 0;
short a_adjust = 0;
short b_adjust = 0;
short time = 100;

//---For RGB_LAB transform

short rgbOri;
short rgbAdj;
//unsigned char rTemp, gTemp, bTemp;
float rTemp, gTemp, bTemp;
unsigned char rChar, gChar, bChar;
int *l, *a, *b;

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

//Reference: Mr.Mark Ruzon
/* Color.c */
// Convert between RGB and CIE-Lab color spaces
// Uses ITU-R recommendation BT.709 with D65 as reference white.
// Yossi Rubner
// Last modified 2/24/98
//=================================================================== LAB_GEL Function
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

//printf("RGB=(%d,%d,%d) ==> Lab(%d,%d,%d)\n",R,G,B,*L,*a,*b);
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

//printf("Lab=(%f,%f,%f) ==> RGB(%f,%f,%f)\n",L,a,b,*R,*G,*B);
}


//===================================================================


void main()
{
	int i,j,k,y0,y1,v0,u0;
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

			//============================Change Here
			for(i = 0; i < WIDTH; i++)
			{
				for(j = 0; j < HEIGHT; j++)
				{
					
					if((image[j][i] -  ((short)0xFFFF)) > (1))
					{
						rTemp = ((float)((image[j][i]&0xF800)>>11))*255/31;
						gTemp = ((float)((image[j][i]&0x7E0)>>5))*255/63;
						bTemp = ((float)(image[j][i]&0x1F))*255/31;

						rChar = (unsigned char)rTemp;
						gChar = (unsigned char)gTemp;
						bChar = (unsigned char)bTemp;
						
						RGB2Lab(rChar,gChar,bChar,l,a,b);

						
						if(negativeL == 0)
						{
							*l = *l + ((float)l_adjust/100) * (*l) * time;
						}
						else
						{
							*l = *l - ((float)l_adjust/100) * (*l) * time;
						}
						if(negativeA == 0)
						{
							*a = *a + ((float)a_adjust/100)* (*a) *time;
						}
						else
						{
							*a = *a - ((float)a_adjust/100)* (*a) *time;
						}

						if(negativeB == 0)
						{
							*b = *b + ((float)b_adjust/100)* (*b) *time;
						}
						else
						{
							*b = *b - ((float)b_adjust/100)* (*b) *time;
						}
						

						Lab2RGB(*l, *a, *b, &rChar, &gChar, &bChar);
	
						rTemp = (float)rChar*31/255;
						gTemp = (float)gChar*63/255;
						bTemp = (float)bChar*31/255;

						rgbAdj = (((short)rTemp)<<11)|(((short)gTemp)<<5)|(((short)bTemp));
						lcd[j+Y_SHIFT][i+X_SHIFT] = rgbAdj;
					}
				}
			}
			
			VM3224ADDH = 0x0000;
			for (j=0;j<240;j++)
			for (i=0;i<320;i++) VM3224DATA=lcd[j][i];
			flag = 0;
		}
	}
}
