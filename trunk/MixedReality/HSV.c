float rgbMax(float R_value, float G_value, float B_value)
{
	float temp = 0;
	if(R_value >= temp)
	{
		temp = R_value;
	}
	if(G_value >= temp)
	{
		temp = G_value;
	}
	if(B_value >= temp)
	{
		temp = B_value;
	}

	return temp;
}

float rgbMin(float rTemp, float gTemp, float bTemp)
{
	float temp = 255;
	if(rTemp <= temp)
	{
		temp = rTemp;
	}
	if( gTemp <= temp)
	{
		temp =  gTemp;
	}
	if(bTemp <= temp)
	{
		temp = bTemp;
	}

	return temp;
}

void RGB2HSV(float rTemp,float gTemp,float bTemp, float *h, float *s, float *v)
{
	float RGB_Min; //var_Min
  	float RGB_Max; //var_Max
  	float Difference; //del_Max

	RGB_Min = rgbMin(rTemp, gTemp, bTemp);
	RGB_Max = rgbMax(rTemp, gTemp, bTemp);
	Difference = RGB_Max - RGB_Min;

	// V_value
	*v = RGB_Max;

	// S_value
	if(RGB_Max != 0)
	{
		*s = Difference/RGB_Max;
	}
	else
	{
		*s = 0;
		*h = 0;
		return;
	}

	//H_value
	if(rTemp == RGB_Max)
		*h = (gTemp - bTemp)/Difference;
	else if(gTemp == RGB_Max)
		*h = 2 + ( bTemp - rTemp ) / Difference;
	else
		*h = 4 + ( rTemp - gTemp ) / Difference;
		
	
	(*h) *= 60;
	if( *h < 0 )
    	*h += 360; 

}

void HSV2RGB(float *h, float *s, float *v, float *rTemp, float *gTemp, float *bTemp)
{
	 int i;
     float f, p, q, t;

	//--- Threshold for H(0~360), S(0~1), V(0~1)
	if(*h > 360)
	{
		*h = 360;
	}

	if(*h < 0)
	{
		*h = 0;
	}

	if(*s > 1)
	{
		*s = 1;
	}

	if(*s < 0)
	{
		*s = 0;
	}

	if(*v > 1)
	{
		*v = 1;
	}

	if(*v < 0)
	{
		*v = 0;
	}
	//--- 


    if( *s == 0 ) 
    {
    	// achromatic (grey)
        *rTemp = *v;
        *gTemp = *v;
        *bTemp = *v;
        return;
    }

	*h /= 60;                        // sector 0 to 5
    i = floor(*h); 
    f = *h - i;                        // factorial part of h
    p = *v * ( 1 - *s );
    q = *v * ( 1 - *s * f );
    t = *v * ( 1 - *s * ( 1 - f ) );

	switch( i ) {
		case 0:
		        *rTemp = *v;
		        *gTemp = t;
		        *bTemp = p;
		        break;
		case 1:
		        *rTemp = q;
		        *gTemp = *v;
		        *bTemp = p;
		        break;
		case 2:
		        *rTemp = p;
		        *gTemp = *v;
		        *bTemp = t;
		        break;
		case 3:
		        *rTemp = p;
		        *gTemp = q;
		        *bTemp = *v;
		        break;
		case 4:
		        *rTemp = t;
		        *gTemp = p;
		        *bTemp = *v;
		        break;
		default:                // case 5:
		        *rTemp = *v;
		        *gTemp = p;
		        *bTemp = q;
		        break;
		}
}

