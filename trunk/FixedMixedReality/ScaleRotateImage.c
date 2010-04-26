#ifndef SCALE_IMAGE_HEADER
#define SCALE_IMAGE_HEADER

#include "Utility.h"

static int floatToIntScale = 20;

int getPixelValueBilinear(int pPrime, int qPrime, unsigned short ary2_imgSample[HEIGHT][WIDTH], int angle) {
	// This function returns the pixel value at a non-integeral
	// coordinates of the image using bilinear interpolation.
	// If the values are outside of the bounds of the image,
	// it returns white (255).
	int lowerP = pPrime / floatToIntScale;
	int lowerQ = qPrime / floatToIntScale;
	int a = pPrime - lowerP * floatToIntScale;
	int b = qPrime - lowerQ * floatToIntScale;

	int rPrime, gPrime, bPrime;

	//int randomNoise100 = rand() % 100; // Update random noise for each interpolated pixel

	if (ary2_imgSample[lowerP][lowerQ] == 0x0000 ||
		ary2_imgSample[lowerP][lowerQ+1] == 0x0000 ||
		ary2_imgSample[lowerP+1][lowerQ] == 0x0000 ||
		ary2_imgSample[lowerP+1][lowerQ+1] == 0x0000)
		return Max(Max(Max(ary2_imgSample[lowerP][lowerQ], ary2_imgSample[lowerP][lowerQ+1]),ary2_imgSample[lowerP+1][lowerQ]),ary2_imgSample[lowerP+1][lowerQ+1]);
	
	// R
	rPrime = (floatToIntScale-a) * (floatToIntScale-b) * ((ary2_imgSample[lowerP][lowerQ]&0xF800)>>11) +	    // r00
			 (floatToIntScale-a) *   b   * ((ary2_imgSample[lowerP + 1][lowerQ]&0xF800)>>11) +    // r01
			   a   * (floatToIntScale-b) * ((ary2_imgSample[lowerP][lowerQ + 1]&0xF800)>>11) +    // r10
			   a   *   b   * ((ary2_imgSample[lowerP + 1][lowerQ + 1]&0xF800)>>11); // r11

	// G
	gPrime = (floatToIntScale-a) * (floatToIntScale-b) * ((ary2_imgSample[lowerP][lowerQ]&0x7E0)>>5) +		  // g00
			 (floatToIntScale-a) *   b   * ((ary2_imgSample[lowerP + 1][lowerQ]&0x7E0)>>5) +	  // g01
			   a   * (floatToIntScale-b) * ((ary2_imgSample[lowerP][lowerQ + 1]&0x7E0)>>5) +	  // g10
			   a   *   b   * ((ary2_imgSample[lowerP + 1][lowerQ + 1]&0x7E0)>>5); // g11

	// B
	bPrime = (floatToIntScale-a) * (floatToIntScale-b) * (ary2_imgSample[lowerP][lowerQ]&0x1F) +		// b00
			 (floatToIntScale-a) *   b   * (ary2_imgSample[lowerP + 1][lowerQ]&0x1F) +	// b01
			   a   * (floatToIntScale-b) * (ary2_imgSample[lowerP][lowerQ + 1]&0x1F) +	// b10
			   a   *   b   * (ary2_imgSample[lowerP + 1][lowerQ + 1]&0x1F);	// b11

	if (rPrime != 0 && gPrime != 0 && bPrime != 0) {
		rPrime += noiseVariance * (rand() % 100);
		gPrime += noiseVariance * (rand() % 100);
		bPrime += noiseVariance * (rand() % 100);
	}

	rPrime = Min(rPrime / 400, 31);
	gPrime = Min(gPrime / 400, 63);
	bPrime = Min(bPrime / 400, 31);
	
	if(rPrime < rValue && gPrime < gValue && bPrime < bValue)
		return 0x0000;

	return (((short)rPrime)<<11)|(((short)gPrime)<<5)|(((short)bPrime));
}

void scaleImage(short scaleFactor100, unsigned short ary2_imgSample[HEIGHT][WIDTH], unsigned short ary2_imgInput[HEIGHT][WIDTH]) {
	int j,k;
	int pPrime, qPrime;
	int scaledHeight, scaledWidth;
	int offsetHeight, offsetWidth;
	
	scaledHeight = HEIGHT * scaleFactor100 / 100.0;
	scaledWidth = WIDTH * scaleFactor100 / 100.0;
	
	offsetHeight = (int)((HEIGHT-scaledHeight) / 2);
	offsetWidth = (int)((WIDTH-scaledWidth) / 2);

	for (j=0; j < scaledHeight; j++) {
		for (k=0; k < scaledWidth; k++) {
			pPrime = j * floatToIntScale / (scaleFactor100 / 100.0);
			qPrime = k * floatToIntScale / (scaleFactor100 / 100.0);
			ary2_imgInput[j+offsetHeight][k+offsetWidth] = getPixelValueBilinear(pPrime, qPrime, ary2_imgSample, 0);
			//ary2_imgInput[j][k] = 0x0000;
		}
	}
	//up
	for (j=0; j < offsetHeight; j++) {
		for (k=0; k < WIDTH; k++) {
			ary2_imgInput[j][k] = 0x0000;
			//ary2_imgInput[j][k] = 0xffff;
		}
	}
	//down
	for (j=scaledHeight + offsetHeight; j < HEIGHT; j++) {
		for (k=0; k < WIDTH; k++) {
			ary2_imgInput[j][k] = 0x0000;
			//ary2_imgInput[j][k] = 0xffff;
		}
	}
	//left
	for (j=0; j < HEIGHT; j++) {
		for (k=0; k < offsetWidth; k++) {
			ary2_imgInput[j][k] = 0x0000;
			//ary2_imgInput[j][k] = 0xffff;
		}
	}
	//right
	for (j=0; j < HEIGHT; j++) {
		for (k=scaledHeight + offsetWidth; k < WIDTH; k++) {
			ary2_imgInput[j][k] = 0x0000;
			//ary2_imgInput[j][k] = 0xffff;
		}
	}
}

#endif // SCALE_IMAGE_HEADER
