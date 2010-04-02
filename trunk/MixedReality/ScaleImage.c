#ifndef SCALE_IMAGE_HEADER
#define SCALE_IMAGE_HEADER

#include "Utility.h"
//#include "Image.h"
//#include "subImage.h"

//int subImageHeight = HEIGHT;
//int subImageWidth = WIDTH;

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

void scaleImage(short scaleFactor100) {
	int j,k;
	double pPrime, qPrime;
	
	subImageHeight = HEIGHT * scaleFactor100 / 100.0;
	subImageWidth = WIDTH * scaleFactor100 / 100.0;
	
	for (j=0; j < subImageHeight; j++) {
		for (k=0; k < subImageWidth; k++) {
			pPrime = (double)j / (scaleFactor100 / 100.0);
			qPrime = (double)k / (scaleFactor100 / 100.0);
			ary2_subImage[j][k] = getPixelValueBilinear(pPrime, qPrime);
		}
	}
}

#endif // SCALE_IMAGE_HEADER
