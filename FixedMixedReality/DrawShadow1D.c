#include "Utility.h"
#include <math.h>

// This function is used to draw a false shadow on the final image after the
// insert object has been superimposed onto the background image.  The 1D
// indicates that either a red *OR* blue target has been found.
void DrawShadow1D(Filter *ptr_theFilter, unsigned short ary2_imgFrame[XLCD][YLCD]){
	int i,j;
	float outcome,distanceFromCenter,distance,edge,adjust,focusYRight,focusYLeft;
	//int k,m;
	Shadow tmpShadow;
	//unsigned short tmpColor,tmpH,tmpS,tmpV;
	unsigned short tmpColor;
	//float tmpH,tmpS,tmpV;
	unsigned short tmpH,tmpS,tmpV;

	tmpShadow.xCenter = Min(XLCD, ptr_theFilter->xCenter + 75 * ptr_theFilter->scaleFactor / 100 + shadowAOffset - 50);
	tmpShadow.yCenter = Min(YLCD, ptr_theFilter->yCenter + shadowBOffset - 50);
	tmpShadow.a		  = shadowA * ptr_theFilter->scaleFactor / 100;	// Statically define elipse parameters until sizing is complete
	tmpShadow.b		  = shadowB * ptr_theFilter->scaleFactor / 100;	// Statically define elipse parameters until sizing is complete
	tmpShadow.eccentricity = pow(1 - pow(((double)tmpShadow.b / (double)tmpShadow.a),2.0), 1.0 / 2.0);
	//DrawBox(ptr_theFilter->xFrom, ptr_theFilter->xTo, ptr_theFilter->yFrom, ptr_theFilter->yTo, ptr_theFilter->boxBorder, ptr_theFilter->boxPadding, ary2_imgFrame);
	for(j=tmpShadow.xCenter - tmpShadow.b;j<tmpShadow.xCenter + tmpShadow.b;j++){
		if(j >= XLCD || j < 0)
			continue;
		for(i=tmpShadow.yCenter - tmpShadow.a;i<tmpShadow.yCenter + tmpShadow.a;i++){
			if(i >= YLCD || i < 0)
				continue;
			
			// Checks that the current pixel is within the shadow boundary
			outcome = (pow((i - tmpShadow.yCenter),2))/(pow(tmpShadow.a,2)) + (pow((j - tmpShadow.xCenter),2))/(pow(tmpShadow.b,2));
			
			// If the current pixel is within the ROI, adjust the shadow (V channel values)
			if (outcome <= 1) {
				distanceFromCenter = tmpShadow.a * tmpShadow.eccentricity;
				focusYRight = tmpShadow.yCenter + distanceFromCenter;
				focusYLeft = tmpShadow.yCenter - distanceFromCenter;


				distance = pow(pow((i - focusYRight),2.0) + pow((j - tmpShadow.xCenter),2.0), 1.0 / 2.0) + pow(pow((i - focusYLeft),2.0) + pow((j - tmpShadow.xCenter),2.0),1.0 / 2.0);
				edge = tmpShadow.a + distanceFromCenter;
				adjust = pow(distance / edge, 2.0);

				// Ensures that adjustment value never inflates
				if (adjust >= 1)
					continue;

				tmpColor = ary2_imgFrame[j][i];
				RGB2HSV(tmpColor, &tmpH, &tmpS, &tmpV);
				tmpV = tmpV * adjust;
				tmpColor = HSV2RGB(tmpH, tmpS, tmpV);
				//ary2_imgFrame[j][i] = 0;
				ary2_imgFrame[j][i] = tmpColor;
			}
		}
	}
	
}

