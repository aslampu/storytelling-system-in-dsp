#include "Utility.h"

void DrawBox(int xFrom, int xTo, int yFrom, int yTo, int boxBorder, int boxPadding, unsigned short ary2_imgFrame[XLCD][YLCD]){
	//xFrom, xTo, yFrom, yTo are integers and includes boxBorder and boxPadding
	int i,j;
	int tmpXStart1, tmpYStart1, tmpXEnd1, tmpYEnd1, tmpXStart2, tmpYStart2, tmpXEnd2, tmpYEnd2;

	//without boxPadding, but with boxBorder
	tmpXStart1 = Min(XLCD, Max(0, xFrom + boxPadding));
	tmpYStart1 = Min(YLCD, Max(0, yFrom + boxPadding));
	tmpXEnd1 = Min(XLCD, Max(0, xTo - boxPadding));
	tmpYEnd1 = Min(YLCD, Max(0, yTo - boxPadding));
	//without boxPadding and without boxBorder
	tmpXStart2 = Min(XLCD, Max(0, xFrom + boxPadding + boxBorder));
	tmpYStart2 = Min(YLCD, Max(0, yFrom + boxPadding + boxBorder));
	tmpXEnd2 = Min(XLCD, Max(0, xTo - boxPadding - boxBorder));
	tmpYEnd2 = Min(YLCD, Max(0, yTo - boxPadding - boxBorder));
		
	//show box in ary2_imgFrame
	//add black:0 UP
	for(j=tmpXStart1;j<tmpXStart2;j++)
		for(i=tmpYStart1;i<tmpYEnd1;i++)
			ary2_imgFrame[j][i] = 0;
	//Down
	for(j=tmpXEnd2;j<tmpXEnd1;j++)
		for(i=tmpYStart1;i<tmpYEnd1;i++)
			ary2_imgFrame[j][i] = 0;
	//Left
	for(i=tmpYStart1;i<tmpYStart2;i++)
		for(j=tmpXStart1;j<tmpXEnd1;j++)
			ary2_imgFrame[j][i] = 0;
	//Right
	for(i=tmpYEnd2;i<tmpYEnd1;i++)
		for(j=tmpXStart1;j<tmpXEnd1;j++)
			ary2_imgFrame[j][i] = 0;
}

