#include <math.h>
#include "utility.h"
//#include "scaleImage.h"
//#include "subImage.h"
//#include "Image.h"

void track(short tmp[][YLCD], Filter *Former, Filter *Latter){
	int i = 0, j = 0;
	int XFrom = -1, YFrom = -1, XTo = -1, YTo = -1;
	float rTemp = -1, gTemp = -1, bTemp = -1;
	float hTemp = -1, sTemp = -1, vTemp = -1;
	//Search Range Prediction
	int search_Xcenter = 2 * (Latter->Xcenter - Former->Xcenter);
	int search_Ycenter = 2 * (Latter->Ycenter - Former->Ycenter);
	int search_XLength = sqrt((Latter->Size * Latter->Size) / Former->Size);//always square
	//int search_YLength;//reserved
	//float imageScaleFactor;//reserved
	Filter NewComer;
	filterInitialize(Latter->Color, &NewComer);
	
	XFrom = int_min(XLCD, int_max(0, floor(search_Xcenter - (search_XLength / 2) - Latter->Padding - Latter->Border)));
	YFrom = int_min(YLCD, int_max(0, floor(search_Ycenter - (search_XLength / 2) - Latter->Padding - Latter->Border)));
	XTo = int_min(XLCD, int_max(0, floor(search_Xcenter + (search_XLength / 2) + Latter->Padding + Latter->Border)));
	YTo = int_min(YLCD, int_max(0, floor(search_Ycenter + (search_XLength / 2) + Latter->Padding + Latter->Border)));
	
	//Sweep given image matrix for the color ball
	for(j=XFrom;j<XTo;j++){
		for(i=YFrom;i<YTo;i++){
			rTemp = (tmp[j][i] & 0xf800) >> 11;
			gTemp = (tmp[j][i] & 0x07e0) >> 5;
			bTemp = (tmp[j][i] & 0x001f);
			RGB2HSV(rTemp,gTemp,bTemp,&hTemp,&sTemp,&vTemp);
	
			//Identify the color ball -- version1
			if(hTemp>(Latter->H_Threshold-Latter->H_Bias) && 
			   hTemp<(Latter->H_Threshold+Latter->H_Bias) &&
			   sTemp>(Latter->S_Threshold-Latter->S_Bias) && 
			   sTemp<(Latter->S_Threshold+Latter->S_Bias) && 
			   vTemp>(Latter->V_Threshold-Latter->V_Bias) && 
			   vTemp<(Latter->V_Threshold+Latter->V_Bias)){

				NewComer.Size++;
				NewComer.Xcenter += j;
				NewComer.Ycenter += i;
			}
			/*
			//Identify the color ball -- version2
			if(rTemp>(Latter->H_Threshold) && 
			   gTemp<(Latter->S_Threshold) &&
			   bTemp>(Latter->V_Threshold)){

				NewComer.Size++;
				NewComer.Xcenter += j;
				NewComer.Ycenter += i;
			}
			*/			
		}
	}

	if(NewComer.Size > NewComer.LowerBound){// Detect an object
		NewComer.Found = 1;
		NewComer.Xcenter = NewComer.Xcenter/NewComer.Size;
		NewComer.Ycenter = NewComer.Ycenter/NewComer.Size;
		Former = Latter;
		Latter = &NewComer;		

		//Draw if necessary
		/*if(NewComer.Size < NewComer.UpperBound){
			imageScaleFactor = floor((NewComer.Size * NewComer.Level) / NewComer.UpperBound)/ NewComer.Level;
			search_XLength = HEIGHT * imageScaleFactor;
			search_YLength = WIDTH * imageScaleFactor;
		}else{
			search_XLength = HEIGHT;
			search_YLength = WIDTH;
		}
		XFrom = int_min(XLCD, int_max(0, floor(NewComer.Xcenter - (search_XLength / 2) - NewComer.Padding - NewComer.Border)));
		YFrom = int_min(YLCD, int_max(0, floor(NewComer.Ycenter - (search_YLength / 2) - NewComer.Padding - NewComer.Border)));
		XTo = int_min(XLCD, int_max(0, floor(NewComer.Xcenter + (search_XLength / 2) + NewComer.Padding + NewComer.Border)));
		YTo = int_min(YLCD, int_max(0, floor(NewComer.Ycenter + (search_YLength / 2) + NewComer.Padding + NewComer.Border)));
		DrawBox(tmp, XFrom, XTo, YFrom, YTo, NewComer.Border, NewComer.Padding){
		*/

	}else{		
		filterInitialize(NewComer.Color, &Former);
		filterInitialize(NewComer.Color, &Latter);
	}
}

