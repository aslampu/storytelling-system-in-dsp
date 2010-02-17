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
#include 	"math.h"

// Definitions for async access(change as you wish)
#define		WSU			(2<<28)	 // Write Setup : 0-15
#define		WST			(8<<22)  // Write Strobe: 0-63
#define		WHD			(2<<20)	 // Write Hold  : 0-3
#define		RSU			(2<<16)	 // Read Setup  : 0-15
#define		TA			(3<<14)	 // Turn Around : 0-3
#define		RST			(8<<8)	 // Read Strobe : 0-63
#define		RHD			(2<<0)	 // Read Hold   : 0-3
#define		MTYPEA		(2<<4)
#define     XLCD		240
#define     YLCD		320

#pragma 	DATA_SECTION ( lcd,".sdram" )
#pragma 	DATA_SECTION ( cam,".sdram" )
#pragma 	DATA_SECTION ( now,".sdram" )
#pragma 	DATA_SECTION ( tmp,".sdram" )

short       now[XLCD][YLCD];
short 		tmp[XLCD][YLCD];
short		lcd[XLCD][YLCD];
short		cam[XLCD][YLCD];
short		rgb[64][32][32];
int			flag=0;

//Can be controlled by GEL
unsigned short 		R_Threshold = 0;
unsigned short		R_Tolerance = 0; 
unsigned short      R_Padding = 2;
unsigned short 		R_Border = 2;

typedef struct {
	int Size;
	float Xcenter;
	float Ycenter;
	float Xlength;
	float Ylength;
	int Xstart;
	int Ystart;
	int Xend;
	int Yend;
	//float brightness;
	//float hue;
	//float saturation;
} Box;

typedef struct {
	int stepIn;
	int move;
	int drawBox;
	int resetFlag;
} Display;

DSK6713_AIC23_CodecHandle hCodec;

extern cregister volatile unsigned int IER;
extern cregister volatile unsigned int CSR;

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
	A->Xend = 240;
	A->Yend = 320;
}

void DrawBox(short tmp[][YLCD], Box *NewComer, int XFrom, int XTo, int YFrom, int YTo, int Threshold, int Tolerance, int Border){
	int i,j;
	for(j=XFrom;j<XTo;j++){
		for(i=YFrom;i<YTo;i++){
			if((tmp[j][i]>>11) > Threshold){
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
		
		//show Box in tmp
		//add black:0 UP
		for(j=XFrom;j<int_min(XLCD,XFrom+Border);j++)
		for(i=YFrom;i<YTo;i++)
			tmp[j][i] = 0;
		//Down
		for(j=XTo;j<int_max(0,XTo-Border);j--)
		for(i=YFrom;i<YTo;i++)
			tmp[j][i] = 0;
		//Left
		for(i=YFrom;i<int_min(YLCD,YFrom+Border);i++)
		for(j=XFrom;j<XTo;j++)
			tmp[j][i] = 0;
		//Right
		for(i=YTo;j<int_max(0,XTo-Border);i--)
		for(j=XFrom;j<XTo;j++)
			tmp[j][i] = 0;	
	}// else tmp is unchanged		
}

void main()
{
	int 				i,j,k,y0,y1,v0,u0;
	int 				R_XFrom,R_XTo,R_YFrom,R_YTo;
	unsigned short 		mask=0xffff;
	
	Box R_Former;
	Box R_Latter;
	Box R_NewComer;
	Display state = {0,0,0,1};
	
	enum colorID {ALL_Color, R_Color, G_Color, B_Color};

	boxInitialize(&R_Former);
	boxInitialize(&R_Latter);
	boxInitialize(&R_NewComer);
	
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

	DSK6713_LED_init();
	DSK6713_DIP_init();

	intr_enable(6);
	intr_enable(15);

	while(1) {
		state.stepIn = !DSK6713_DIP_get(3);
		if(DSK6713_DIP_get(0)){
			DSK6713_LED_off(0);
			if(state.stepIn){
				state.drawBox = 0;
				DSK6713_LED_on(3);
			}else{
				state.drawBox = 1;
				DSK6713_LED_off(3);
			}
			state.stepIn = 1;
			state.move = 1;
		}else{//pressed:0 => stepIn
			DSK6713_LED_on(0);
			if(state.stepIn){
				DSK6713_LED_on(3);
			}else{
				state.move = 1;
				DSK6713_LED_off(3);
			}
		}
				
		if(flag && state.stepIn && state.move) {
			VM3224ADDH = 0x08000;	// Select Cam screen buffer and Set address (1<<15|0x00000)
			for (j=0;j<XLCD;j++)
			for (i=0;i<YLCD;i++) cam[j][i]=VM3224DATA;

			for (j=0;j<XLCD;j++){
				for (i=0;i<YLCD;i+=2) {  // Conversion yuv to RGB565
					y0 = (cam[j][i]>>8) & 0x0ff;
					u0 = cam[j][i] & 0x0ff;
					y1 = (cam[j][i+1]>>8) & 0x0ff;
					v0 = cam[j][i+1] & 0x0ff;
					y0 = y0>>2;
					y1 = y1>>2;
					u0 = u0>>3;
					v0 = v0>>3;
								
					now[j][i] = rgb[y0][u0][v0];
					now[j][i+1] = rgb[y1][u0][v0];				
					tmp[j][i] = rgb[y0][u0][v0];
					tmp[j][i+1] = rgb[y1][u0][v0];
				}
			}
			flag = 0;
			state.move = 0;
			// We get rgb in now and tmp matrix now
			// Predicted image search range 
			R_XFrom = int_max(0,floor(2 * R_Latter.Xcenter - R_Former.Xcenter - R_Padding - R_Border));
			R_XTo = int_min(XLCD,floor(2 * R_Latter.Xcenter - R_Former.Xcenter + R_Latter.Xlength + R_Padding + R_Border));
			R_YFrom = int_max(0,floor(2 * R_Latter.Ycenter - R_Former.Ycenter - R_Padding - R_Border));
			R_YTo = int_min(YLCD,floor(2 * R_Latter.Ycenter - R_Former.Ycenter + R_Latter.Ylength + R_Padding + R_Border));
			// Modify tmp matrix if necessary
			DrawBox(tmp, &R_NewComer, R_XFrom, R_XTo, R_YFrom, R_YTo, R_Threshold, R_Tolerance, R_Border);
		}
		
		switch(!DSK6713_DIP_get(1)*2+!DSK6713_DIP_get(2)){
			case ALL_Color:
				mask=0xffff;
				DSK6713_LED_off(1);
				DSK6713_LED_off(2);
				break;
			case R_Color:
				mask = 0xf800;
				DSK6713_LED_off(1);
				DSK6713_LED_on(2);
				break;
			case G_Color:
				mask = 0x07e0;
				DSK6713_LED_on(1);
				DSK6713_LED_off(2);
				break;
			case B_Color:
				mask = 0x001f;
				DSK6713_LED_on(1);
				DSK6713_LED_on(2);
				break;
			default://Something must be wrong
				DSK6713_LED_on(0);
				DSK6713_LED_on(1);
				DSK6713_LED_on(2);
				DSK6713_LED_on(3);
		}	
		VM3224ADDH = 0;		    // Select LCD screen buffer and Set address (0<<15|0x00000)
		if(state.drawBox){
			for (j=0;j<XLCD;j++)
				for (i=0;i<YLCD;i++)
					VM3224DATA= tmp[j][i] & mask;
		}else{
			for (j=0;j<XLCD;j++)
				for (i=0;i<YLCD;i++)
					VM3224DATA= now[j][i] & mask;
		}
	}
}
