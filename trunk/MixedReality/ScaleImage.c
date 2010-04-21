#ifndef SCALE_IMAGE_HEADER
#define SCALE_IMAGE_HEADER

#include "Utility.h"

int getPixelValueBilinear(float pPrime, float qPrime, unsigned short ary2_imgSample[HEIGHT][WIDTH]) {
	// This function returns the pixel value at a non-integeral
	// coordinates of the image using bilinear interpolation.
	// If the values are outside of the bounds of the image,
	// it returns white (255).
	int lowerP = pPrime;
	int lowerQ = qPrime;
	float a = pPrime - lowerP;
	float b = qPrime - lowerQ;

	float rPrime, gPrime, bPrime;

	//int randomNoise100 = rand() % 100; // Update random noise for each interpolated pixel
	
	// R
	rPrime = (1-a) * (1-b) * ((ary2_imgSample[lowerP][lowerQ]&0xF800)>>11) +	    // r00
			 (1-a) *   b   * ((ary2_imgSample[lowerP + 1][lowerQ]&0xF800)>>11) +    // r01
			   a   * (1-b) * ((ary2_imgSample[lowerP][lowerQ + 1]&0xF800)>>11) +    // r10
			   a   *   b   * ((ary2_imgSample[lowerP + 1][lowerQ + 1]&0xF800)>>11); // r11

	// G
	gPrime = (1-a) * (1-b) * ((ary2_imgSample[lowerP][lowerQ]&0x7E0)>>5) +		  // g00
			 (1-a) *   b   * ((ary2_imgSample[lowerP + 1][lowerQ]&0x7E0)>>5) +	  // g01
			   a   * (1-b) * ((ary2_imgSample[lowerP][lowerQ + 1]&0x7E0)>>5) +	  // g10
			   a   *   b   * ((ary2_imgSample[lowerP + 1][lowerQ + 1]&0x7E0)>>5); // g11

	// B
	bPrime = (1-a) * (1-b) * (ary2_imgSample[lowerP][lowerQ]&0x1F) +		// b00
			 (1-a) *   b   * (ary2_imgSample[lowerP + 1][lowerQ]&0x1F) +	// b01
			   a   * (1-b) * (ary2_imgSample[lowerP][lowerQ + 1]&0x1F) +	// b10
			   a   *   b   * (ary2_imgSample[lowerP + 1][lowerQ + 1]&0x1F);	// b11

	if (rPrime != 0 && gPrime != 0 && bPrime != 0) {
		rPrime += noiseVariance * (rand() % 100);
		gPrime += noiseVariance * (rand() % 100);
		bPrime += noiseVariance * (rand() % 100);
	}

	rPrime = Min(rPrime, 31);
	gPrime = Min(gPrime, 63);
	bPrime = Min(bPrime, 31);

	return (((short)rPrime)<<11)|(((short)gPrime)<<5)|(((short)bPrime));
}

float getPixelValueBilinearOriginal(float pPrime, float qPrime, unsigned short ary2_imgSample[HEIGHT][WIDTH]) {
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
	r00 = (float)((ary2_imgSample[lowerP][lowerQ]&0xF800)>>11) / 31;
	r01 = (float)((ary2_imgSample[lowerP + 1][lowerQ]&0xF800)>>11) / 31;
	r10 = (float)((ary2_imgSample[lowerP][lowerQ + 1]&0xF800)>>11) / 31;
	r11 = (float)((ary2_imgSample[lowerP + 1][lowerQ + 1]&0xF800)>>11) / 31;
	rPrime = (1-a) * (1-b) * r00 +
			 (1-a) *   b   * r01 +
			   a   * (1-b) * r10 +
			   a   *   b   * r11;

	// G
	g00 = (float)((ary2_imgSample[lowerP][lowerQ]&0x7E0)>>5) / 63;
	g01 = (float)((ary2_imgSample[lowerP + 1][lowerQ]&0x7E0)>>5) / 63;
	g10 = (float)((ary2_imgSample[lowerP][lowerQ + 1]&0x7E0)>>5) / 63;
	g11 = (float)((ary2_imgSample[lowerP + 1][lowerQ + 1]&0x7E0)>>5) / 63;
	gPrime = (1-a) * (1-b) * g00 +
			 (1-a) *   b   * g01 +
			   a   * (1-b) * g10 +
			   a   *   b   * g11;

	// B
	b00 = (float)(ary2_imgSample[lowerP][lowerQ]&0x1F) / 31;
	b01 = (float)(ary2_imgSample[lowerP + 1][lowerQ]&0x1F) / 31;
	b10 = (float)(ary2_imgSample[lowerP][lowerQ + 1]&0x1F) / 31;
	b11 = (float)(ary2_imgSample[lowerP + 1][lowerQ + 1]&0x1F) / 31;
	bPrime = (1-a) * (1-b) * b00 +
			 (1-a) *   b   * b01 +
			   a   * (1-b) * b10 +
			   a   *   b   * b11;

	if (rPrime != 0 && gPrime != 0 && bPrime != 0) {
		rPrime += noiseVariance * (rand() % 100);
		gPrime += noiseVariance * (rand() % 100);
		bPrime += noiseVariance * (rand() % 100);
	}

	rPrime = Min(rPrime *= 31, 31);
	gPrime = Min(gPrime *= 63, 63);
	bPrime = Min(bPrime *= 31, 31);

	return (((short)rPrime)<<11)|(((short)gPrime)<<5)|(((short)bPrime));
}

void scaleImage(short scaleFactor100, unsigned short ary2_imgSample[HEIGHT][WIDTH], unsigned short ary2_imgInput[HEIGHT][WIDTH]) {
	int j,k;
	double pPrime, qPrime;
	int scaledHeight, scaledWidth;
	int offsetHeight, offsetWidth;
	
	scaledHeight = HEIGHT * scaleFactor100 / 100.0;
	scaledWidth = WIDTH * scaleFactor100 / 100.0;
	
	offsetHeight = (int)((HEIGHT-scaledHeight) / 2);
	offsetWidth = (int)((WIDTH-scaledWidth) / 2);

	for (j=0; j < scaledHeight; j++) {
		for (k=0; k < scaledWidth; k++) {
			pPrime = (double)j / (scaleFactor100 / 100.0);
			qPrime = (double)k / (scaleFactor100 / 100.0);
			ary2_imgInput[j+offsetHeight][k+offsetWidth] = getPixelValueBilinear(pPrime, qPrime, ary2_imgSample);
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
