/* ======================================================================== */
/*            Copyright (C) 2008 - 2009 ND-Tech. Co., Ltd.                  */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
/* Project  : Cam2yuv                           Created  : 03-MAR-2009      */
/* File     : main.c                            Contact  : info@nd-tech.com */
/* ======================================================================== */
/* You are free to use or modify this code to the following restrictions:   */
/*          Acknowledge ND Tech. Co. Ltd.                                   */
/*          Or, put "Parts of code by ND Tech. Co., Ltd."                   */
/*          Or, leave this header as it is.                                 */
/* in somewhere in your code.                                               */
/* ======================================================================== */

#include	"vm3224k.h"
#include	"math.h"
#include  	"image.h"

#define		CE2CTL		*(volatile int *)(0x01800010)
// Definitions for async access(change as you wish)
#define		WSU			(2<<28)	 // Write Setup : 0-15
#define		WST			(8<<22)  // Write Strobe: 0-63
#define		WHD			(2<<20)	 // Write Hold  : 0-3
#define		RSU			(2<<16)	 // Read Setup  : 0-15
#define		TA			(3<<14)	 // Turn Around : 0-3
#define		RST			(8<<8)	 // Read Strobe : 0-63
#define		RHD			(2<<0)	 // Read Hold   : 0-3
#define		MTYPEA		(2<<4)

#pragma		DATA_SECTION (in_img, ".sdram")
#pragma 	DATA_ALIGN(in_img,8)
#pragma		DATA_SECTION (rev_img, ".sdram")
#pragma		DATA_ALIGN(rev_img,8)
#pragma		DATA_SECTION (dct_img, ".sdram")
//rgb table used for outputing to LCD
short		rgb[64];
//Variable for Noise Gain levels
/*float GAIN0=0;
float GAIN1=0;
float GAIN2=0;
float GAIN3=0;
float GAIN4=0;
float GAIN5=0;
float GAIN6=0;
float GAIN7=0;*/
float GAIN[8] = {0,0,0,0,0,0,0,0};

//-----------------
//#define RAND_MAX 256;
double DCT_Table[8][8][8][8];
short N = 8;
//double noise = 0;
double check = 0;
double pi = 3.14159265424;
unsigned char rev_img[X_SIZE][Y_SIZE];
//short rev_img[X_SIZE][Y_SIZE];
double dct_img[X_SIZE][Y_SIZE];

//-----------------

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

void main()
{
	int 	i,j,p,q,n,k;
	int 	rand256;
	double  sum;
	PLL6713();			// Initialize C6713 PLL
	CE2CTL = (WSU|WST|WHD|RSU|RST|RHD|MTYPEA);
	vm3224init();       // Initialize vm3224k2
	vm3224rate(3);		// Set frame rate
	vm3224bl(15);       // Set backlight
	
	// Create RGB565 lookup table
	for (k=0;k<64;k++)  
		rgb[k] = ybr_565(k<<2,128,128);

	//Generate a table of DCT Coefficients
	for(p = 0; p < N; p++){
		for(q = 0; q < N; q++){
			for (n = 0; n < N; n++){
				for (k=0;k<N;k++){
					DCT_Table[p][q][n][k] =  cos((pi/N) * (n+0.5) * (p+0.5)) * cos((pi/N) * (k+0.5) * (q+0.5));
				}
			}
		}
				
	}
		
	while (1) {
		//Transform input image (in_img) to DCT coefficients
	for(i=0; i < X_SIZE; i+=8) {
		for(j=0; j < Y_SIZE; j+=8) {
			for(p=0; p<N; p++){
				for(q=0; q<N; q++){
					sum = 0;
					for (n = 0; n < N; n++) {
						for (k = 0; k < N; k++) {
							sum += ((int)in_img[i+n][j+k] * DCT_Table[p][q][n][k]);
						}
					}
					dct_img[i+p][j+q] = (2.0 / N) * sum;
				}
			}
		}
	}

		//apply noise to DCT coeffs. at subset of frequencies
		for(i=0; i < X_SIZE; i+=8) {
			for(j=0; j < Y_SIZE; j+=8) {
				rand256 = rand() % 256;
				for(p=0; p<N; p++){
					dct_img[i+p][j+p] += GAIN[p] * rand256;					
				}
			}
		}	
		//take inverse DCT transform
		for(i=0; i < X_SIZE; i+=8) {
			for(j=0; j < Y_SIZE; j+=8) {
				for(p=0; p<N; p++){
					for(q=0; q<N; q++){
						sum = 0;
						for (n = 0; n < N; n++) {
							for (k = 0; k < N; k++) {
								sum += (dct_img[i+n][j+k] * DCT_Table[p][q][n][k]);
							}
						}
						check = (2.0 / N) * sum;
						if(check > 255.0)
			 				check = 255.0;
						if(check < 0.0)
							check = 0.0;
				
						rev_img[i+p][j+q] = floor(check);
						//rev_img[i+p][j+q] = (short)floor(check);
					}
				}
			}
		}
		//used to output final image to LCD
		VM3224ADDH = 0;		     // Select LCD screen buffer and Set address (0<<15|0x00000)
		for (j=0;j<240;j++){
			for (i=0;i<320;i++){
				//since image is only 240x240
				if( i < 240 ){
					//if(DSK6713_DIP_get(0)){
						//note that rgb only has 64 levels of greyscale
						//so divide an 8-bit value by 4 (or shift by 2)	 
					//	VM3224DATA = (short)(rgb[in_img[j][i]>>2]); 
					//	DSK6713_LED_off(0);
					//}else{
						//VM3224DATA = (short)(rgb[(in_img[j][i]>>2)]);
						VM3224DATA = (short)(rgb[rev_img[j][i]>>2]);
					//	DSK6713_LED_on(0);
					//}
				}
				else{
					VM3224DATA = rgb[0];		
					//DSK6713_LED_off(0);	
				}
			}
    	}
	}
}
