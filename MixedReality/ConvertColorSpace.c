#include <math.h>
#include "Utility.h"

//void RGB2HSV(unsigned short rgbColor, float *ptr_hValue, float *ptr_sValue, float *ptr_vValue){
float RGB2HSV(unsigned short rgbColor){
	float rTemp = (((rgbColor & 0xf800) >> 11) * 255 / 31);
	float gTemp = (((rgbColor & 0x07e0) >> 5) * 255 / 63);
	float bTemp = ((rgbColor & 0x001f) * 255 / 31);
	float RGB_Min = Min(rTemp,Min(gTemp,bTemp));
	float RGB_Max = Max(rTemp,Max(gTemp,bTemp));
	float Difference = RGB_Max - RGB_Min;
	float hValue = 0;

	// V_value
	//*ptr_vValue = RGB_Max;

	// S_value
	/*if(RGB_Max != 0)
		*ptr_sValue = Difference/RGB_Max;
	else{
		*ptr_sValue = 0;
		*ptr_hValue = 0;
		return;
	}*/
	if(Difference == 0)
		return hValue;

	//H_value
	if(rTemp == RGB_Max)
		hValue = (gTemp - bTemp) / Difference;
		//*ptr_hValue = (gTemp - bTemp)/Difference;
	else if(gTemp == RGB_Max)
		hValue = 2 + ( bTemp - rTemp ) / Difference;
		//*ptr_hValue = 2 + ( bTemp - rTemp ) / Difference;
	else
		hValue = 4 + ( rTemp - gTemp ) / Difference;
		//*ptr_hValue = 4 + ( rTemp - gTemp ) / Difference;
		
	/*(*ptr_hValue) *= 60;
	if( *ptr_hValue < 0 )
    	*ptr_hValue += 360; */

	hValue *= 60;
	if(hValue < 0)
    	hValue += 360;

	return hValue;
}

/*unsigned short HSV2RGB(float hValue, float sValue, float vValue){
	 int rTemp,gTemp,bTemp;
     float f, p, q, t, i;

	//--- Threshold for H(0~360), S(0~1), V(0~1)
	if(hValue > 360)
		hValue = 360;
	if(hValue < 0)
		hValue = 0;
	if(sValue > 1)
		sValue = 1;
	if(sValue < 0)
		sValue = 0;
	if(vValue > 1)
		vValue = 1;
	if(vValue < 0)
		vValue = 0;
    if( sValue == 0 ) 
    {    	
    // achromatic (grey)
        rTemp = floor(vValue);
        gTemp = floor(vValue);
        bTemp = floor(vValue);
        return ((rTemp&0x0f8)<<8)|((gTemp&0x0fc)<<3)|((bTemp&0x0f8)>>3);
    }

	hValue /= 60;                        // sector 0 to 5
    i = floor(hValue); 
    f = hValue - i;                      // factorial part of h
    p = floor(vValue * ( 1 - sValue ));
    q = floor(vValue * ( 1 - sValue * f ));
    t = floor(vValue * ( 1 - sValue * ( 1 - f )));

	switch( i ) {
		case 0:
		        rTemp = vValue;
		        gTemp = t;
		        bTemp = p;
		        break;
		case 1:
		        rTemp = q;
		        gTemp = vValue;
		        bTemp = p;
		        break;
		case 2:
		        rTemp = p;
		        gTemp = vValue;
		        bTemp = t;
		        break;
		case 3:
		        rTemp = p;
		        gTemp = q;
		        bTemp = vValue;
		        break;
		case 4:
		        rTemp = t;
		        gTemp = p;
		        bTemp = vValue;
		        break;
		default:                // case 5:
		        rTemp = vValue;
		        gTemp = p;
		        bTemp = q;
		        break;
		}

	return ((rTemp&0x0f8)<<8)|((gTemp&0x0fc)<<3)|((bTemp&0x0f8)>>3);
}*/

//Reference: Mr.Mark Ruzon
/* Color.c */
// Convert between RGB and CIE-Lab color spaces
// Uses ITU-R recommendation BT.709 with D65 as reference white.
// Yossi Rubner
// Last modified 2/24/98
//=================================================================== LAB_GEL Function
void RGB2Lab(float R, float G, float B,
	     int *L, int *a, int *b)
{
  float X, Y, Z, fX, fY, fZ;

  X = 0.412453*R + 0.357580*G + 0.180423*B;
  Y = 0.212671*R + 0.715160*G + 0.072169*B;
  Z = 0.019334*R + 0.119193*G + 0.950227*B;

  X /= (255 * 0.950456);
  Y /=  255;
  Z /= (255 * 1.088754);

  if (Y > 0.008856)
    {
      fY = pow(Y, 1.0/3.0);
      *L = (int)(116.0*fY - 16.0 + 0.5);
    }
  else
    {
      fY = 7.787*Y + 16.0/116.0;
      *L = (int)(903.3*Y + 0.5);
    }

  if (X > 0.008856)
      fX = pow(X, 1.0/3.0);
  else
      fX = 7.787*X + 16.0/116.0;

  if (Z > 0.008856)
      fZ = pow(Z, 1.0/3.0);
  else
      fZ = 7.787*Z + 16.0/116.0;

  *a = (int)(500.0*(fX - fY) + 0.5);
  *b = (int)(200.0*(fY - fZ) + 0.5);

//printf("RGB=(%d,%d,%d) ==> Lab(%d,%d,%d)\n",R,G,B,*L,*a,*b);
}

void Lab2RGB(int L, int a, int b, 
	     float *R, float *G, float *B)
{
  float X, Y, Z, fX, fY, fZ;
  int RR, GG, BB;

  fY = pow((L + 16.0) / 116.0, 3.0);
  if (fY < 0.008856)
      fY = L / 903.3;
  Y = fY;

  if (fY > 0.008856)
    fY = pow(fY, 1.0/3.0);
  else
    fY = 7.787 * fY + 16.0/116.0;

  fX = a / 500.0 + fY;      
  if (fX > 0.206893)
      X = pow(fX, 3.0);
  else
      X = (fX - 16.0/116.0) / 7.787;
 
  fZ = fY - b /200.0;      
  if (fZ > 0.206893)
      Z = pow(fZ, 3.0);
  else
      Z = (fZ - 16.0/116.0) / 7.787;

  X *= (0.950456 * 255);
  Y *=             255;
  Z *= (1.088754 * 255);

  RR =  (int)(3.240479*X - 1.537150*Y - 0.498535*Z + 0.5);
  GG = (int)(-0.969256*X + 1.875992*Y + 0.041556*Z + 0.5);
  BB =  (int)(0.055648*X - 0.204043*Y + 1.057311*Z + 0.5);

  *R = (float)(RR < 0 ? 0 : RR > 255 ? 255 : RR);
  *G = (float)(GG < 0 ? 0 : GG > 255 ? 255 : GG);
  *B = (float)(BB < 0 ? 0 : BB > 255 ? 255 : BB);

//printf("Lab=(%f,%f,%f) ==> RGB(%f,%f,%f)\n",L,a,b,*R,*G,*B);
}



