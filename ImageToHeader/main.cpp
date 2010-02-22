#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include "IOHelper.h"

int R_BITS = 5;
int G_BITS = 6;
int B_BITS = 5;

int main (int argc, char * const argv[]) {
    if (argc != 5) {
		cout << "usage: ./ImageToHeader /path/to/input/file.raw width height /path/to/output/file.h" << endl;
		return 1;
	}
	char* inputImgPath = argv[1];
	int width = atoi(argv[2]);
	int height = atoi(argv[3]);
	char* outputDatPath = argv[4];
	
	// Initialize input and output files
	ByteFile inputImg = ByteFile(inputImgPath);
	ofstream outputDat(outputDatPath, ofstream::binary);
	
	// Dimension Validation
	if (width * height * 3 != inputImg.getNumBytes())
		cout << "WARNING: Mismatch in image dimensions" << endl; 
	
	// Parameter Definition Output
	string* strBuffer = NULL;
	char buffer[200];
	sprintf(buffer, "#define		HEIGHT	%d\n", height);
	strBuffer = new string(buffer);
	outputDat.write(strBuffer->c_str(), strBuffer->length());
	delete strBuffer;
	sprintf(buffer, "#define		WIDTH	%d\n", width);
	strBuffer = new string(buffer);
	outputDat.write(strBuffer->c_str(), strBuffer->length());
	delete strBuffer;
	
	// Begin reading pixel data
	unsigned char* rVal = inputImg.getFileStream()->getNextByte();
	unsigned char* gVal = inputImg.getFileStream()->getNextByte();
	unsigned char* bVal = inputImg.getFileStream()->getNextByte();

	// Convert 24-bit pixel to 16-bit pixel (RGB: 5,6,5)
	int rScaled = (int)*rVal / 255 * (pow(2.0, (double)R_BITS) - 1);
	int gScaled = (int)*gVal / 255 * (pow(2.0, (double)G_BITS) - 1);
	int bScaled = (int)*bVal / 255 * (pow(2.0, (double)B_BITS) - 1);
	int outputVal  = rScaled<<(G_BITS+B_BITS);
	outputVal += gScaled<<(B_BITS);
	outputVal += bScaled;
	
	sprintf(buffer, "#define		BG_COLOR	0x%04x\n", outputVal);
	strBuffer = new string(buffer);
	outputDat.write(strBuffer->c_str(), strBuffer->length());
	delete strBuffer;
	
	outputDat.write("short		image[HEIGHT][WIDTH] = {\n", 32);
	
	int countWidth = 0;
	int countHeight = 0;
	while (rVal != NULL && gVal != NULL && bVal != NULL) {
		// Convert 24-bit pixel to 16-bit pixel (RGB: 5,6,5)
		rScaled = (int)*rVal / 255 * (pow(2.0, (double)R_BITS) - 1);
		gScaled = (int)*gVal / 255 * (pow(2.0, (double)G_BITS) - 1);
		bScaled = (int)*bVal / 255 * (pow(2.0, (double)B_BITS) - 1);
		
		// Generate single 16-bit int representation
		outputVal  = rScaled<<(G_BITS+B_BITS);
		outputVal += gScaled<<(B_BITS);
		outputVal += bScaled;
		
		// Output to file
		if (countWidth == 0) {
			sprintf(buffer, "{0x%04x", outputVal);
		} else {
			sprintf(buffer, ",0x%04x", outputVal);
		}
	
		strBuffer = new string(buffer);
		outputDat.write(strBuffer->c_str(), strBuffer->length());
		delete strBuffer;
		
		countWidth++;
		
		if (countWidth == width) {
			countWidth = 0;
			countHeight++;
			if (countHeight == height)
				outputDat.write("}\n", 2);
			else
				outputDat.write("},\n", 3);
		}
		
		// Continue
		rVal = inputImg.getFileStream()->getNextByte();
		gVal = inputImg.getFileStream()->getNextByte();
		bVal = inputImg.getFileStream()->getNextByte();
		outputVal = 0;
	}
	
	outputDat.write("};\n", 3);
	
	// Clean-up
	delete rVal; delete gVal; delete bVal;
	
    return 0;
}
