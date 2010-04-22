#include <math.h>
#include "Utility.h"

void LabCompute(Filter *theFilter, unsigned short ary2_imgInput[HEIGHT][WIDTH])
	int labNumber = 0;
	int L,a,b;
	float avgL = 0;
	float avgA = 0;
	float avgB = 0;
	float stdL = 0;
	float stdA = 0;
	float stdB = 0;

	for(j=0; j<HEIGHT; j++){
		for(i=0; i<WIDTH;i++){
			//compute input images' Lab
			if(ary2_imgInput[j][i] != 65535){
				labNumber++;
				L = ary2_rgb2labTable[ary2_imgInput[j][i]][0];
				a = ary2_rgb2labTable[ary2_imgInput[j][i]][1];
				b = ary2_rgb2labTable[ary2_imgInput[j][i]][2];
			}else{
				L = 0;
				a = 0;
				b = 0;
			}

			avgL += L;
			avgA += a;
			avgB += b;
			stdL += L * L;
			stdA += a * a;
			stdB += b * b;			
		}
	}

	avgL = avgL/labNumber;
	avgA = avgA/labNumber;
	avgB = avgB/labNumber;
	stdL = sqrt(stdL/labNumber - avgL * avgL);
	stdA = sqrt(stdA/labNumber - avgA * avgA);
	stdB = sqrt(stdB/labNumber - avgB * avgB);

}
