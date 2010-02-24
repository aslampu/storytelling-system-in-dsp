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



//=================================================================== Additional Lib 
#include 	"math.h"
#include	"ball.dat"
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

#pragma 	DATA_SECTION ( lcd,".sdram" )
#pragma 	DATA_SECTION ( cam,".sdram" )

short		lcd[240][320];
short		cam[240][320];
short		rgb[64][32][32];
int			flag=0;
DSK6713_AIC23_CodecHandle hCodec;

extern cregister volatile unsigned int IER;
extern cregister volatile unsigned int CSR;

//=================================================================== HSV_GEL Variable
//---For GEL
short doHSV = 0; //if doHSV is '0', the frame will be original
short negativeH = 0;
short negativeS = 0;
short negativeV = 0;
short h_adjust = 0;
short s_adjust = 0;
short v_adjust = 0;
short time = 100;

//---For RGB_HSV transform
short ball_img[BALL_SIZE][BALL_SIZE];
short rgbOri;
short rgbAdj;
float rTemp, gTemp, bTemp;
float hsv[3];

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


//=================================================================== HSV_GEL Function
float rgbMax(float R_value, float G_value, float B_value)
{
	float temp = 0;
	if(R_value >= temp)
	{
		temp = R_value;
	}
	if(G_value >= temp)
	{
		temp = G_value;
	}
	if(B_value >= temp)
	{
		temp = B_value;
	}

	return temp;
}

float rgbMin(float rTemp, float gTemp, float bTemp)
{
	float temp = 255;
	if(rTemp <= temp)
	{
		temp = rTemp;
	}
	if( gTemp <= temp)
	{
		temp =  gTemp;
	}
	if(bTemp <= temp)
	{
		temp = bTemp;
	}

	return temp;
}

void RGB2HSV(float rTemp,float gTemp,float bTemp, float *hsv)
{
	float RGB_Min; //var_Min
  	float RGB_Max; //var_Max
  	float Difference; //del_Max

	RGB_Min = rgbMin(rTemp, gTemp, bTemp);
	RGB_Max = rgbMax(rTemp, gTemp, bTemp);
	Difference = RGB_Max - RGB_Min;

	// V_value
	hsv[2] = RGB_Max;

	// S_value
	if(RGB_Max != 0)
	{
		hsv[1] = Difference/RGB_Max;
	}
	else
	{
		hsv[1] = 0;
		hsv[0] = 0;
		return;
	}

	//H_value
	if(rTemp == RGB_Max)
		hsv[0] = (gTemp - bTemp)/Difference;
	else if(gTemp == RGB_Max)
		hsv[0] = 2 + ( bTemp - rTemp ) / Difference;
	else
		hsv[0] = 4 + ( rTemp - gTemp ) / Difference;
		
	
	hsv[0] *= 60;
	if( hsv[0] < 0 )
    	hsv[0] += 360; 

}

void HSV2RGB(float* hsv, float* rTemp, float* gTemp, float* bTemp)
{
	 int i;
     float f, p, q, t;

	//--- Threshold for H(0~360), S(0~1), V(0~1)
	if(hsv[0] > 360)
	{
		hsv[0] = 360;
	}

	if(hsv[0] < 0)
	{
		hsv[0] = 0;
	}

	if(hsv[1] > 1)
	{
		hsv[1] = 1;
	}

	if(hsv[1] < 0)
	{
		hsv[1] = 0;
	}

	if(hsv[2] > 1)
	{
		hsv[2] = 1;
	}

	if(hsv[2] < 0)
	{
		hsv[2] = 0;
	}
	//--- 


    if( hsv[1] == 0 ) 
    {
    	// achromatic (grey)
        *rTemp = hsv[2];
        *gTemp = hsv[2];
        *bTemp = hsv[2];
        return;
    }

	hsv[0] /= 60;                        // sector 0 to 5
    i = floor( hsv[0] ); 
    f = hsv[0] - i;                        // factorial part of h
    p = hsv[2] * ( 1 - hsv[1] );
    q = hsv[2] * ( 1 - hsv[1] * f );
    t = hsv[2] * ( 1 - hsv[1] * ( 1 - f ) );

	switch( i ) {
		case 0:
		        *rTemp = hsv[2];
		        *gTemp = t;
		        *bTemp = p;
		        break;
		case 1:
		        *rTemp = q;
		        *gTemp = hsv[2];
		        *bTemp = p;
		        break;
		case 2:
		        *rTemp = p;
		        *gTemp = hsv[2];
		        *bTemp = t;
		        break;
		case 3:
		        *rTemp = p;
		        *gTemp = q;
		        *bTemp = hsv[2];
		        break;
		case 4:
		        *rTemp = t;
		        *gTemp = p;
		        *bTemp = hsv[2];
		        break;
		default:                // case 5:
		        *rTemp = hsv[2];
		        *gTemp = p;
		        *bTemp = q;
		        break;
		}
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
			for(i = 0; i < BALL_SIZE; i++)
			{
				for(j = 0; j < BALL_SIZE; j++)
				{
					//lcd[i][j] = ball[20][20];

					rTemp = ((float)((ball[i][j]&0xF800)>>11))/31;
					gTemp = ((float)((ball[i][j]&0x7E0)>>5))/63;
					bTemp = ((float)(ball[i][j]&0x1F))/31;

					RGB2HSV(rTemp,gTemp,bTemp,hsv);

					if(negativeH == 0)
					{
						hsv[0] = hsv[0] + ((float)h_adjust/100)*hsv[0]*time;
					}
					else if(negativeH == 1)
					{
						hsv[0] = hsv[0] - ((float)h_adjust/100)*hsv[0]*time;
					}
					if(negativeS == 0)
					{
						hsv[1] = hsv[1] + ((float)s_adjust/100)*hsv[1]*time;
					}
					else
					{
						hsv[1] = hsv[1] - ((float)s_adjust/100)*hsv[1]*time;
					}

					if(negativeV == 0)
					{
						hsv[2] = hsv[2] + ((float)v_adjust/100)*hsv[2]*time;
					}
					else
					{
						hsv[2] = hsv[2] - ((float)v_adjust/100)*hsv[2]*time;
					}

					HSV2RGB(hsv, &rTemp, &gTemp, &bTemp);

					rTemp = rTemp*31;
					gTemp = gTemp*63;
					bTemp = bTemp*31;

					rgbAdj = (((short)rTemp)<<11)|(((short)gTemp)<<5)|(((short)bTemp));
					lcd[i][j] = rgbAdj;
				}
			}
			
			VM3224ADDH = 0x0000;
			for (j=0;j<240;j++)
			for (i=0;i<320;i++) VM3224DATA=lcd[j][i];
			flag = 0;
		}
	}
}
