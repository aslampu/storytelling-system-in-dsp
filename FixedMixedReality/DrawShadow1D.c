#include "Utility.h"
#include <math.h>

#define DECIMAL 10000

// This function is used to draw a false shadow on the final image after the
// insert object has been superimposed onto the background image.  The 1D
// indicates that either a red *OR* blue target has been found.
void DrawShadow1D(Filter *ptr_theFilter, unsigned short ary2_imgFrame[XLCD][YLCD]){
	int i,j;

	//----change
	//float outcome,distanceFromCenter,distance,edge,adjust,focusYRight,focusYLeft;
	short outcome;
	short distanceFromCenter;
	int distance;
	int edge;
	int adjust;

	short focusYRight;
	short focusYLeft;
	//----change
	
	Shadow tmpShadow;
	
	unsigned short tmpColor;

	unsigned short tmpH,tmpS,tmpV;

	tmpShadow.xCenter = Min(XLCD, ptr_theFilter->xCenter + 75 * ptr_theFilter->scaleFactor / 100 + shadowAOffset - 50);
	tmpShadow.yCenter = Min(YLCD, ptr_theFilter->yCenter + shadowBOffset - 50);
	tmpShadow.a		  = shadowA * ptr_theFilter->scaleFactor / 100;	// Statically define elipse parameters until sizing is complete
	tmpShadow.b		  = shadowB * ptr_theFilter->scaleFactor / 100;	// Statically define elipse parameters until sizing is complete
	
	//------change
	//tmpShadow.eccentricity = pow(1 - pow(((double)tmpShadow.b / (double)tmpShadow.a),2.0), 1.0 / 2.0);
	tmpShadow.eccentricity = (pow(1 - pow(((double)tmpShadow.b / (double)tmpShadow.a),2.0), 1.0 / 2.0) * DECIMAL);
	//------change


	//DrawBox(ptr_theFilter->xFrom, ptr_theFilter->xTo, ptr_theFilter->yFrom, ptr_theFilter->yTo, ptr_theFilter->boxBorder, ptr_theFilter->boxPadding, ary2_imgFrame);
	for(j=tmpShadow.xCenter - tmpShadow.b;j<tmpShadow.xCenter + tmpShadow.b;j++){
		if(j >= XLCD || j < 0)
			continue;
		for(i=tmpShadow.yCenter - tmpShadow.a;i<tmpShadow.yCenter + tmpShadow.a;i++){
			if(i >= YLCD || i < 0)
				continue;
			
			// Checks that the current pixel is within the shadow boundary
			//-----change
			//outcome = (pow((i - tmpShadow.yCenter),2))/(pow(tmpShadow.a,2)) + (pow((j - tmpShadow.xCenter),2))/(pow(tmpShadow.b,2));
			outcome = ((pow((double)(i - tmpShadow.yCenter),2))/(pow((double)tmpShadow.a,2)) + (pow((double)(j - tmpShadow.xCenter),2))/(pow((double)tmpShadow.b,2)))*DECIMAL;
			//-----change

			// If the current pixel is within the ROI, adjust the shadow (V channel values)
			//if (outcome <= 1) {
			if(outcome <= DECIMAL){
				//------change
				distanceFromCenter = (tmpShadow.a * tmpShadow.eccentricity / DECIMAL);
				focusYRight = tmpShadow.yCenter + distanceFromCenter;
				focusYLeft = tmpShadow.yCenter - distanceFromCenter;


				distance = DECIMAL * (pow(pow((i - focusYRight),2.0) + pow((j - tmpShadow.xCenter),2.0), 1.0 / 2.0) + pow(pow((i - focusYLeft),2.0) + pow((j - tmpShadow.xCenter),2.0),1.0 / 2.0));
				edge = ((tmpShadow.a + distanceFromCenter)*DECIMAL);
				adjust = (pow((double)distance / (double)edge, 2.0) * DECIMAL);

				// Ensures that adjustment value never inflates
				if (adjust >= DECIMAL)
					continue;

				tmpColor = ary2_imgFrame[j][i];
				RGB2HSV(tmpColor, &tmpH, &tmpS, &tmpV);
				tmpV = tmpV * ((double)adjust/(double)DECIMAL);
				tmpColor = HSV2RGB(tmpH, tmpS, tmpV);
				//ary2_imgFrame[j][i] = 0;
				ary2_imgFrame[j][i] = tmpColor;
				
			}
		}
	}
	
}

