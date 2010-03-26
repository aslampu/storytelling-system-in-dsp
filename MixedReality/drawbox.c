#include "utility.h"

void DrawBox(short tmp[][YLCD], int XFrom, int XTo, int YFrom, int YTo, int Border, int Padding){
	//XFrom, XTo, YFrom, YTo are integers and includes Border and Padding
	int i,j;
	int tmpXstart1, tmpYstart1, tmpXend1, tmpYend1, tmpXstart2, tmpYstart2, tmpXend2, tmpYend2;

	//without Padding, but with Border
	tmpXstart1 = int_min(XLCD, int_max(0, XFrom + Padding));
	tmpYstart1 = int_min(YLCD, int_max(0, YFrom + Padding));
	tmpXend1 = int_min(XLCD, int_max(0, XTo - Padding));
	tmpYend1 = int_min(YLCD, int_max(0, YTo - Padding));
	//without Padding and without Border
	tmpXstart2 = int_min(XLCD, int_max(0, XFrom + Padding + Border));
	tmpYstart2 = int_min(YLCD, int_max(0, YFrom + Padding + Border));
	tmpXend2 = int_min(XLCD, int_max(0, XTo - Padding - Border));
	tmpYend2 = int_min(YLCD, int_max(0, YTo - Padding - Border));
		
	//show Box in tmp
	//add black:0 UP
	for(j=tmpXstart1;j<tmpXstart2;j++)
		for(i=tmpYstart1;i<tmpYend1;i++)
			tmp[j][i] = 0;
	//Down
	for(j=tmpXend2;j<tmpXend1;j++)
		for(i=tmpYstart1;i<tmpYend1;i++)
			tmp[j][i] = 0;
	//Left
	for(i=tmpYstart1;i<tmpYstart2;i++)
		for(j=tmpXstart1;j<tmpXend1;j++)
			tmp[j][i] = 0;
	//Right
	for(i=tmpYend2;i<tmpYend1;i++)
		for(j=tmpXstart1;j<tmpXend1;j++)
			tmp[j][i] = 0;
}

