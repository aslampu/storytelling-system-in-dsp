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
short doHSV = 1; //if doHSV is '0', the frame will be original
short negativeH = 0;
short negativeS = 0;
short negativeV = 0;
short h_adjust = 0;
short s_adjust = 0;
short v_adjust = 0;



short rgbOri[2] = {0,0};
short rgbAdj[2] = {0,0};
float rTemp0, gTemp0, bTemp0, rTemp1, gTemp1, bTemp1;

float hsv0[3], hsv1[3];

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
			VM3224ADDH = 0x08000;	// Select Cam screen buffer and Set address (1<<15|0x00000)
			for (j=0;j<240;j++)
			for (i=0;i<320;i++) cam[j][i]=VM3224DATA;
			for (j=0;j<240;j++)
			for (i=0;i<320;i+=2) {  // Conversion yuv to RGB565
				y0 = (cam[j][i]>>8) & 0x0ff;
				u0 = cam[j][i] & 0x0ff;
				y1 = (cam[j][i+1]>>8) & 0x0ff;
				v0 = cam[j][i+1] & 0x0ff;
				y0 = y0>>2;
				y1 = y1>>2;
				u0 = u0>>3;
				v0 = v0>>3;
				

				//------------------------------------------------------
				//Step 0: To Be Or Not To Be
				if(doHSV == 0)
				{
					lcd[j][i] = rgb[y0][u0][v0];
					lcd[j][i+1] = rgb[y1][u0][v0];
				}
				else
				{
				
				//Step 1: Get RGB
				rgbOri[0] = rgb[y0][u0][v0];
				rgbOri[1] = rgb[y1][u0][v0];

				//Step 2: Transform RGB value to 0~1
				rTemp0 = ((float)((rgbOri[0]&0xF800)>>11))/31;
				gTemp0 = ((float)((rgbOri[0]&0x7E0)>>5))/31;
				bTemp0 = ((float)(rgbOri[0]&0x1F))/31;
	
				rTemp1 = ((float)((rgbOri[1]&0xF800)>>11))/31;
				gTemp1 = ((float)((rgbOri[1]&0x7E0)>>5))/31;
				bTemp1 = ((float)(rgbOri[1]&0x1F))/31;

				//Step 3: RGB to HSV
				RGB2HSV(rTemp0,gTemp0,bTemp0, hsv0);
				RGB2HSV(rTemp1,gTemp1,bTemp1, hsv1);

				//Step 4: Adjust HSV
				if(negativeH == 0)
				{
					hsv0[0] = hsv0[0] + (h_adjust/100)*hsv0[0];
					hsv1[0] = hsv1[0] + (h_adjust/100)*hsv1[0];
					//lcd[j][i] =0;
					//lcd[j][i+1] = 0;
				}
				else if(negativeH == 1)
				{
					hsv0[0] = hsv0[0] - (h_adjust/100)*hsv0[0];
					hsv1[0] = hsv1[0] - (h_adjust/100)*hsv1[0];
					//lcd[j][i] = 255;
					//lcd[j][i+1] = 255;
				}
				
				if(negativeS == 0)
				{
					hsv0[1] = hsv0[1] + (s_adjust/100)*hsv0[1];
					hsv1[1] = hsv1[1] + (s_adjust/100)*hsv1[1];
				}
				else
				{
					hsv0[1] = hsv0[1] - (s_adjust/100)*hsv0[1];
					hsv1[1] = hsv1[1] - (s_adjust/100)*hsv1[1];

				}

				if(negativeV == 0)
				{
					hsv0[2] = hsv0[2] + (v_adjust/100)*hsv0[2];
					hsv1[2] = hsv1[2] + (v_adjust/100)*hsv1[2];
				}
				else
				{
					hsv0[2] = hsv0[2] - (v_adjust/100)*hsv0[2];
					hsv1[2] = hsv1[2] - (v_adjust/100)*hsv1[2];
				}
				
				//Step 5: HSV to RGB
				HSV2RGB(hsv0, &rTemp0, &gTemp0, &bTemp0);
				HSV2RGB(hsv1, &rTemp1, &gTemp1, &bTemp1);

				//Step 6: Transform RGB value (0~1) to RGB565 value
				rTemp0 = rTemp0*31;
				gTemp0 = gTemp0*31;
				bTemp0 = bTemp0*31;
			
				rTemp1 = rTemp1*31;
				gTemp1 = gTemp1*31;
				bTemp1 = bTemp1*31;

				rgbAdj[0] = (((short)rTemp0&0x0f8)<<8)|(((short)gTemp0&0x0fc)<<3)|(((short)bTemp0&0x0f8)>>3);
				rgbAdj[1] = (((short)rTemp1&0x0f8)<<8)|(((short)gTemp1&0x0fc)<<3)|(((short)bTemp1&0x0f8)>>3);

				//Step 7: Input to LCD
				lcd[j][i] = rgbAdj[0];
				lcd[j][i+1] = rgbAdj[1];	

				}

				
				//---------------------------------------------------------

			}
			VM3224ADDH = 0;		    // Select LCD screen buffer and Set address (0<<15|0x00000)
			for (j=0;j<240;j++)
			for (i=0;i<320;i++) VM3224DATA=lcd[j][i];
			flag = 0;
		}
	}
}
