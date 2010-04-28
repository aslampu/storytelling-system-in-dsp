#include "Utility.h"

void applyLAB(float imgAvgL, float imgAvgA, float imgAvgB, float imgStdL, float imgStdA, float imgStdB, unsigned short ary2_source[HEIGHT][WIDTH],unsigned short ary2_imgInputModified[HEIGHT][WIDTH], short ary2_rgb2labTable[NUM_RGB][3]){

	int i,j;
	int adjustedLValue, adjustedAValue, adjustedBValue;
	float sL,sA,sB,dL,dA,dB;
	
	for(j = 0; j<HEIGHT; j++){
		for(i = 0; i < WIDTH; i++){

		
			if(ary2_source[j][i] != 0){
				sL = 1 - weightingS + weightingS * (backgroundStdL / imgStdL); 
				sA = 1 - weightingS + weightingS * (backgroundStdA / imgStdA); 
				sB = 1 - weightingS + weightingS * (backgroundStdB / imgStdB); 
				dL = weightingD * (backgroundAvgL - imgAvgL);
				dA = weightingD * (backgroundAvgA - imgAvgA);
				dB = weightingD * (backgroundAvgB - imgAvgB);

				adjustedLValue = sL * (ary2_rgb2labTable[ary2_source[j][i]][0] - imgAvgL) + imgAvgL + dL;
				adjustedAValue = sA * (ary2_rgb2labTable[ary2_source[j][i]][1] - imgAvgA) + imgAvgA + dA;
				adjustedBValue = sB * (ary2_rgb2labTable[ary2_source[j][i]][2] - imgAvgB) + imgAvgB + dB;
			
					ary2_imgInputModified[j][i] = Lab2RGB(adjustedLValue,adjustedAValue,adjustedBValue);
			}
			else{
				ary2_imgInputModified[j][i] = 0;
			}
		
		
		}
	}

}
