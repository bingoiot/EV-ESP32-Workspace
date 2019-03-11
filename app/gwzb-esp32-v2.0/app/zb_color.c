/*
 * zb_color.c
 *
 *  Created on: 2017��9��17��
 *      Author: zigbee
 */

/*********************************************************************
 * MACROS
 */
#include "zb_color.h"
#include "math.h"

#define PWM_FULL_DUTY_CYCLE 		0xFF

#define WHITE_POINT_X 				0x5000
#define WHITE_POINT_Y 				0x5555

#define LIGHTING_COLOR_HUE_MAX       0xfe
#define LIGHTING_COLOR_SAT_MAX       0xfe

#define LEVEL_MIN                 	0x01
#define LEVEL_MAX                 	0xFE

#define COLOR_XY_MIN                 0x0
#define COLOR_XY_MAX                 LIGHTING_COLOR_CURRENT_X_MAX
#define COLOR_SAT_MIN                0x0
#define COLOR_SAT_MAX                LIGHTING_COLOR_SAT_MAX
#define COLOR_HUE_MIN                0x0
#define COLOR_HUE_MAX                LIGHTING_COLOR_HUE_MAX
#define COLOR_ENH_HUE_MIN            0x0
#define COLOR_ENH_HUE_MAX            0xFFFF
#define GAMMA_VALUE					 2

#define MAX3(a,b,c) (((a) > (b)) ? ( ((a) > (c)) ? (a) : (c) ) : ( ((b) > (c)) ? (b) : (c) ))
#define MIN3(a,b,c) (((a) > (b)) ? ( ((b) > (c)) ? (c) : (b) ) : ( ((a) > (c)) ? (c) : (a) ))
#define DISTANCE(a,b)  (((a) > (b)) ? ((a) - (b)) : ((b)-(a)))

//Color table generated for R[0749,0249], G[0074,799], B[0129,0029].
const unsigned short hueToX[] = {
0xbfff,0xbdf6,0xbbee,0xb9e5,0xb7dd,0xb5d4,0xb3cc,0xb1c4,
0xafbb,0xadb3,0xabaa,0xa9a2,0xa799,0xa591,0xa389,0xa180,
0x9f78,0x9d6f,0x9b67,0x995e,0x9756,0x954e,0x9345,0x913d,
0x8f34,0x8d2c,0x8b24,0x891b,0x8713,0x850a,0x8302,0x80f9,
0x7ef1,0x7ce9,0x7ae0,0x78d8,0x76cf,0x74c7,0x72be,0x70b6,
0x6eae,0x6ca5,0x6a9d,0x6894,0x668c,0x6483,0x627b,0x6073,
0x5e6a,0x5c62,0x5a59,0x5851,0x5649,0x5440,0x5238,0x502f,
0x4e27,0x4c1e,0x4a16,0x480e,0x4605,0x43fd,0x41f4,0x3fec,
0x3de3,0x3bdb,0x39d3,0x37ca,0x35c2,0x33b9,0x31b1,0x2fa8,
0x2da0,0x2b98,0x298f,0x2787,0x257e,0x2376,0x216e,0x1f65,
0x1d5d,0x1b54,0x194c,0x1743,0x1333,0x135d,0x1387,0x13b2,
0x13dc,0x1406,0x1431,0x145b,0x1486,0x14b0,0x14da,0x1505,
0x152f,0x155a,0x1584,0x15ae,0x15d9,0x1603,0x162e,0x1658,
0x1682,0x16ad,0x16d7,0x1702,0x172c,0x1756,0x1781,0x17ab,
0x17d6,0x1800,0x182a,0x1855,0x187f,0x18aa,0x18d4,0x18fe,
0x1929,0x1953,0x197e,0x19a8,0x19d2,0x19fd,0x1a27,0x1a52,
0x1a7c,0x1aa6,0x1ad1,0x1afb,0x1b26,0x1b50,0x1b7a,0x1ba5,
0x1bcf,0x1bfa,0x1c24,0x1c4e,0x1c79,0x1ca3,0x1cce,0x1cf8,
0x1d22,0x1d4d,0x1d77,0x1da2,0x1dcc,0x1df6,0x1e21,0x1e4b,
0x1e76,0x1ea0,0x1eca,0x1ef5,0x1f1f,0x1f4a,0x1f74,0x1f9e,
0x1fc9,0x1ff3,0x201e,0x2048,0x2072,0x209d,0x20c7,0x20f2,
0x2147,0x2325,0x2503,0x26e1,0x28bf,0x2a9d,0x2c7b,0x2e59,
0x3037,0x3215,0x33f3,0x35d1,0x37af,0x398d,0x3b6b,0x3d49,
0x3f27,0x4105,0x42e3,0x44c1,0x469f,0x487d,0x4a5b,0x4c39,
0x4e17,0x4ff5,0x51d3,0x53b1,0x558f,0x576d,0x594b,0x5b29,
0x5d07,0x5ee5,0x60c3,0x62a1,0x647f,0x665d,0x683b,0x6a19,
0x6bf7,0x6dd5,0x6fb3,0x7192,0x7370,0x754e,0x772c,0x790a,
0x7ae8,0x7cc6,0x7ea4,0x8082,0x8260,0x843e,0x861c,0x87fa,
0x89d8,0x8bb6,0x8d94,0x8f72,0x9150,0x932e,0x950c,0x96ea,
0x98c8,0x9aa6,0x9c84,0x9e62,0xa040,0xa21e,0xa3fc,0xa5da,
0xa7b8,0xa996,0xab74,0xad52,0xaf30,0xb10e,0xb2ec,0xb4ca,
0xb6a8,0xb886,0xba64,0xbc42,0xbe20,0xbec9,0xbf72,0xbfff,};

const unsigned short hueToY[] = {
0x3fff,0x41a7,0x434f,0x44f7,0x469f,0x4847,0x49ef,0x4b97,
0x4d3f,0x4ee7,0x508f,0x5237,0x53df,0x5587,0x572f,0x58d7,
0x5a7f,0x5c27,0x5dd0,0x5f78,0x6120,0x62c8,0x6470,0x6618,
0x67c0,0x6968,0x6b10,0x6cb8,0x6e60,0x7008,0x71b0,0x7358,
0x7500,0x76a8,0x7850,0x79f9,0x7ba1,0x7d49,0x7ef1,0x8099,
0x8241,0x83e9,0x8591,0x8739,0x88e1,0x8a89,0x8c31,0x8dd9,
0x8f81,0x9129,0x92d1,0x9479,0x9622,0x97ca,0x9972,0x9b1a,
0x9cc2,0x9e6a,0xa012,0xa1ba,0xa362,0xa50a,0xa6b2,0xa85a,
0xaa02,0xabaa,0xad52,0xaefa,0xb0a2,0xb24b,0xb3f3,0xb59b,
0xb743,0xb8eb,0xba93,0xbc3b,0xbde3,0xbf8b,0xc133,0xc2db,
0xc483,0xc62b,0xc7d3,0xc97b,0xcccc,0xca7a,0xc828,0xc5d6,
0xc385,0xc133,0xbee1,0xbc90,0xba3e,0xb7ec,0xb59b,0xb349,
0xb0f7,0xaea6,0xac54,0xaa02,0xa7b1,0xa55f,0xa30d,0xa0bc,
0x9e6a,0x9c18,0x99c7,0x9775,0x9523,0x92d2,0x9080,0x8e2e,
0x8bdd,0x898b,0x8739,0x84e8,0x8296,0x8044,0x7df3,0x7ba1,
0x794f,0x76fe,0x74ac,0x725a,0x7009,0x6db7,0x6b65,0x6914,
0x66c2,0x6470,0x621f,0x5fcd,0x5d7b,0x5b2a,0x58d8,0x5686,
0x5435,0x51e3,0x4f91,0x4d40,0x4aee,0x489c,0x464b,0x43f9,
0x41a7,0x3f56,0x3d04,0x3ab2,0x3861,0x360f,0x33bd,0x316c,
0x2f1a,0x2cc8,0x2a77,0x2825,0x25d3,0x2382,0x2130,0x1ede,
0x1c8d,0x1a3b,0x17e9,0x1597,0x1346,0x10f4,0x0ea2,0x0c51,
0x07ae,0x0857,0x0901,0x09aa,0x0a54,0x0afe,0x0ba7,0x0c51,
0x0cfa,0x0da4,0x0e4e,0x0ef7,0x0fa1,0x104a,0x10f4,0x119e,
0x1247,0x12f1,0x139b,0x1444,0x14ee,0x1597,0x1641,0x16eb,
0x1794,0x183e,0x18e7,0x1991,0x1a3b,0x1ae4,0x1b8e,0x1c37,
0x1ce1,0x1d8b,0x1e34,0x1ede,0x1f88,0x2031,0x20db,0x2184,
0x222e,0x22d8,0x2381,0x242b,0x24d4,0x257e,0x2628,0x26d1,
0x277b,0x2824,0x28ce,0x2978,0x2a21,0x2acb,0x2b75,0x2c1e,
0x2cc8,0x2d71,0x2e1b,0x2ec5,0x2f6e,0x3018,0x30c1,0x316b,
0x3215,0x32be,0x3368,0x3411,0x34bb,0x3565,0x360e,0x36b8,
0x3762,0x380b,0x38b5,0x395e,0x3a08,0x3ab2,0x3b5b,0x3c05,
0x3cae,0x3d58,0x3e02,0x3eab,0x3f55,0x3ffe,0x3ffd,0x3fff,};

static float Hue_2_RGB( float v1, float v2, float vH );//Function Hue_2_RGB
static void saturation_to_xy(uint16 *x, uint16 *y, uint8 sat);
static uint8 XyToSat(uint16 x, uint16 y, uint8 hue);

void color_huesat_to_xy(color_xy_t *pXY,color_hue_t *pHue)
{
	uint16 colorX, colorY;
	colorX = hueToX[pHue->hue];
	colorY = hueToY[pHue->hue];
	saturation_to_xy(&colorX,&colorY,pHue->saturation);
	pXY->x = (float)colorX/0xFEFF;
	pXY->y = (float)colorY/0xFEFF;
}
void color_xy_to_huesat(color_hue_t *pHue, color_xy_t *pXY)
{
	//update the current hue/sat values from xy
    //Loop thrugh hueToX/Y tables and look for the value closest to the
    //zclColor_CurrentX and zclColor_CurrentY
	uint8 idx, chosenIdx=0;
	uint32 currDist=0, minDist = 0xFFFFF;
    for( idx = 0; idx != 0xFF; idx++ )
    {
		currDist = ((uint32)DISTANCE(hueToX[idx], pXY->x)+ DISTANCE(hueToY[idx], pXY->y));
		if ( currDist < minDist )
		{
			chosenIdx = idx;
			minDist = currDist;
		}
    }
	pHue->hue = chosenIdx;
	pHue->saturation = XyToSat(pXY->x,pXY->y,pHue->hue);
}
float color_rgb_to_xy(color_xy_t *pXY, color_rgb_t *pRGB)
{
	float X,Y,Z;
	float temp;
	color_standard_rgb_to_xy(&X,&Y,&Z,pRGB);
	pXY->x = X / ( X + Y + Z );
	pXY->y = Y / ( X + Y + Z );
	Y*=100;
	temp = pow((Y/254.0),0.5)*254.0;
	return temp;
}

/*************
LinearY range 0~100
*/
void color_xyl_to_rgb(color_rgb_t *pRGB,color_xy_t *pXY, float LinearY)
{
  float Y;
  float X;
  float Z;
  float f_R;
  float f_G;
  float f_B;
  //gamma correct the level
  Y = pow( ( LinearY / LEVEL_MAX ), (float)GAMMA_VALUE ) * (float)LEVEL_MAX;
  //Y = pow( ( LinearY / 0xFE ), (float)2 ) * (float)0xFE;
  // from xyY to XYZ
  if (pXY->y != 0)
  {
    do
    {
      X = pXY->x * ( Y / pXY->y );
      Z = ( 1 - pXY->x - pXY->y ) * ( Y / pXY->y );
    }
    while( ((X > 95.047) || (Z > 108.883)) && ((uint8)(Y--) > 0));
    // normalize variables:
    // X from 0 to 0.95047
    X = (X > 95.047 ? 95.047 : (X < 0 ? 0 : X));
    X = X / 100;
    // Z from 0 to 1.08883
    Z = (Z > 108.883 ? 108.883 : (Z < 0 ? 0 : Z));
    Z = Z / 100;
  }
  else
  {
    X = 0;
    Z = 0;
  }
  // Y from 0 to 1
  Y = (Y > 100 ? 100 : (Y < 0 ? 0 : Y));
  Y = Y / 100;

  // transformation according to standard illuminant D65.
  f_R = X *  3.2406 + Y * -1.5372 + Z * -0.4986;
  f_G = X * -0.9689 + Y *  1.8758 + Z *  0.0415;
  f_B = X *  0.0557 + Y * -0.2040 + Z *  1.0570;

  //color correction
  if ( f_R > 0.003 )
	  f_R = (1.22 * ( pow((double)f_R, ( 1/1.5 ) )) - 0.040);
  else
	  f_R = 0; //1.8023 * var_R;
  if ( f_G > 0.003)
	  f_G = (1.22 * ( pow((double)f_G, ( 1/1.5 ) )) - 0.040);
  else
	  f_G = 0; //1.8023 * var_G;
  if ( f_B > 0.003 )
	  f_B = (1.09 * ( pow((double)f_B, ( 1/1.5 ) )) - 0.050);
  else
	  f_B = 0; //1.8023 * var_B;
  // truncate results exceeding 0..1
  f_R = (f_R > 1.0 ? 1.0 : (f_R < 0 ? 0 : f_R));
  f_G = (f_G > 1.0 ? 1.0 : (f_G < 0 ? 0 : f_G));
  f_B = (f_B > 1.0 ? 1.0 : (f_B < 0 ? 0 : f_B));

  pRGB->R = (uint8)(f_R * 255.0);
  pRGB->G = (uint8)(f_G * 255.0);
  pRGB->B = (uint8)(f_B * 255.0);
}
void color_standard_rgb_to_xy(float *pX, float *pY, float *pZ, color_rgb_t *pRGB)
{
	//sR, sG and sB (Standard RGB) input range = 0 �� 255
	//X, Y and Z output refer to a D65/2�� standard illuminant.
	float var_R, var_G, var_B;
	var_R = ( pRGB->R / 255.0 );
	var_G = ( pRGB->G / 255.0 );
	var_B = ( pRGB->B / 255.0);

	if ( var_R > 0.04045 )
		var_R = pow(( ( var_R + 0.040 ) / 1.22 ), 1.5);
	else
		var_R = var_R / 1.8023;
	if ( var_G > 0.04045 )
		var_G = pow(( ( var_G + 0.040 ) / 1.22 ), 1.5);
	else
		var_G = var_G / 1.8023;
	if ( var_B > 0.04045 )
		var_B = pow(( ( var_B + 0.050 ) / 1.09 ) , 1.5);
	else
		var_B = var_B / 1.8023;

	*pX = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
	*pY = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
	*pZ = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;
}
void color_rgb_to_rgbw( color_rgb_t *pRGB, uint16 *W )
{
  uint8 w;
  if ( MAX3(pRGB->R,pRGB->G,pRGB->B) == 0 )
  {
    *W = 0;
    return;
  }
  w = (uint16)( ( (uint32)MIN3(pRGB->R,pRGB->G,pRGB->B) * (uint32)( (uint32)pRGB->R + pRGB->G + pRGB->B ) ) / ( (uint32)MAX3(pRGB->R,pRGB->G,pRGB->B) * 3 ) );
  pRGB->R += w;
  pRGB->G += w;
  pRGB->B += w;
  *W = MIN3(pRGB->R,pRGB->G,pRGB->B);
  *W = (*W > PWM_FULL_DUTY_CYCLE) ? PWM_FULL_DUTY_CYCLE : *W;
  pRGB->R -= *W;
  pRGB->G -= *W;
  pRGB->B -= *W;
}

void color_rgb_to_hsl(color_hsl_t *pHSL, color_rgb_t *pRGB)
{
	//R, G and B input range = 0 �� 255
	//H, S and L output range = 0 �� 1.0
	float var_R,var_G,var_B;
	float var_Min,var_Max,del_Max;
	float del_R,del_G,del_B;
	var_R = ( (float)pRGB->R / 255.0 );
	var_G = ( (float)pRGB->G / 255.0 );
	var_B = ( (float)pRGB->B / 255.0 );

	var_Min = MIN3( var_R, var_G, var_B );    //Min. value of RGB
	var_Max = MAX3( var_R, var_G, var_B );    //Max. value of RGB
	del_Max = var_Max - var_Min;             //Delta RGB value

	pHSL->L = ( var_Max + var_Min )/ 2.0;

	if ( del_Max == 0 )                     //This is a gray, no chroma...
	{
		pHSL->H = 0;
		pHSL->S = 0;
	}
	else                                    //Chromatic data...
	{
	   if ( pHSL->L < 0.5 )
		   pHSL->S = del_Max / ( var_Max + var_Min );
	   else
		   pHSL->S = del_Max / ( 2 - var_Max - var_Min );

	   del_R = ( ( ( var_Max - var_R ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
	   del_G = ( ( ( var_Max - var_G ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
	   del_B = ( ( ( var_Max - var_B ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;

	   if      ( var_R == var_Max )
			pHSL->H = del_B - del_G;
	   else if ( var_G == var_Max )
			pHSL->H = ( 1.0 / 3.0 ) + del_R - del_B;
	   else if ( var_B == var_Max )
			pHSL->H = ( 2.0 / 3.0 ) + del_G - del_R;

		if ( pHSL->H < 0 ) pHSL->H += 1.0;
		if ( pHSL->H > 1.0 ) pHSL->H -= 1.0;
	}
}
void color_hsl_to_rgb(color_rgb_t *pRGB, color_hsl_t *pHSL)
{
//H, S and L input range = 0 �� 1.0
//R, G and B output range = 0 �� 255
	float var_2,var_1;
	if ( pHSL->S == 0 )
	{
	   pRGB->R = (uint8)(pHSL->L * 255);
	   pRGB->G = (uint8)(pHSL->L * 255);
	   pRGB->B = (uint8)(pHSL->L * 255);
	}
	else
	{
	   if ( pHSL->L < 0.5 )
			var_2 = pHSL->L * ( 1.0 + pHSL->S );
	   else
			var_2 = ( pHSL->L + pHSL->S ) - ( pHSL->S * pHSL->L );
	   var_1 = 2.0 * pHSL->L - var_2;
	   pRGB->R = (uint8)(255.0 * Hue_2_RGB( var_1, var_2, pHSL->H + ( 1.0 / 3.0 ) ));
	   pRGB->G = (uint8)(255.0 * Hue_2_RGB( var_1, var_2, pHSL->H ));
	   pRGB->B = (uint8)(255.0 * Hue_2_RGB( var_1, var_2, pHSL->H - ( 1.0 / 3.0 ) ));
	}
}
void color_rgb_to_hsv(color_hsv_t *pHSV,color_rgb_t *pRGB)
{
	//R, G and B input range = 0 �� 255
	//H, S and V output range = 0 �� 1.0
	float var_R,var_G,var_B;
	float var_Min,var_Max,del_Max;
	float del_R,del_G,del_B;
	var_R = ( (float)pRGB->R / 255.0 );
	var_G = ( (float)pRGB->G / 255.0 );
	var_B = ( (float)pRGB->B / 255.0 );

	var_Min = MIN3( var_R, var_G, var_B );    //Min. value of RGB
	var_Max = MAX3( var_R, var_G, var_B );    //Max. value of RGB
	del_Max = var_Max - var_Min;            //Delta RGB value

	pHSV->V = var_Max;

	if ( del_Max == 0 )                     //This is a gray, no chroma...
	{
		pHSV->H = 0;
		pHSV->S = 0;
	}
	else                                    //Chromatic data...
	{
		pHSV->S = del_Max / var_Max;

		del_R = ( ( ( var_Max - var_R ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
		del_G = ( ( ( var_Max - var_G ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
		del_B = ( ( ( var_Max - var_B ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;

		if ( var_R == var_Max )
			pHSV->H = del_B - del_G;
		else if ( var_G == var_Max )
			pHSV->H = ( 1.0 / 3.0 ) + del_R - del_B;
		else if ( var_B == var_Max )
			pHSV->H = ( 2.0 / 3.0 ) + del_G - del_R;
		if ( pHSV->H < 0 )
			pHSV->H += 1.0;
		if ( pHSV->H > 1.0 )
			pHSV->H -= 1.0;
	}
}

void color_hsv_to_rgb(color_rgb_t *pRGB,color_hsv_t *pHSV)
{
	//H, S and V input range = 0 �� 1.0
	//R, G and B output range = 0 �� 255
	int var_i;
	float var_h;
	float var_1,var_2,var_3;
	float var_r,var_g,var_b;
	if ( pHSV->S == 0 )
	{
		pRGB->R = (uint8)(pHSV->V * 255.0);
		pRGB->G = (uint8)(pHSV->V * 255.0);
		pRGB->B = (uint8)(pHSV->V * 255.0);
	}
	else
	{
		var_h = pHSV->H * 6.0;
		if ( var_h == 6 )
			var_h = 0;      //H must be < 1
		var_i = (int)(var_h);             //Or ... var_i = floor( var_h )
		var_1 = pHSV->V * ( 1 - pHSV->S );
		var_2 = pHSV->V * ( 1 - pHSV->S * ( var_h - var_i ) );
		var_3 = pHSV->V * ( 1 - pHSV->S * ( 1 - ( var_h - var_i ) ) );
		switch(var_i)
		{
		case 0:
			var_r = pHSV->V;
			var_g = var_3;
			var_b = var_1;
			break;
		case 1:
			var_r = var_2;
			var_g = pHSV->V;
			var_b = var_1;
			break;
		case 2:
			var_r = var_1;
			var_g = pHSV->V;
			var_b = var_3;
			break;
		case 3:
			var_r = var_1;
			var_g = var_2;
			var_b = pHSV->V;
			break;
		case 4:
			var_r = var_3;
			var_g = var_1;
			var_b = pHSV->V;
			break;
		default:
			var_r = pHSV->V;
			var_g = var_1;
			var_b = var_2;
			break;
		}
		pRGB->R = (uint8)(var_r * 255.0);
		pRGB->G = (uint8)(var_g * 255.0);
		pRGB->B = (uint8)(var_b * 255.0);
	}
}


static float Hue_2_RGB( float v1, float v2, float vH )//Function Hue_2_RGB
{
   if ( vH < 0 )
	   vH += 1;
   if( vH > 1 )
	   vH -= 1;
   if ( ( 6.0 * vH ) < 1.0 )
	   return ( v1 + ( v2 - v1 ) * 6.0 * vH );
   if ( ( 2.0 * vH ) < 1.0 )
	   return ( v2 );
   if ( ( 3.0 * vH ) < 2.0 )
	   return ( v1 + ( v2 - v1 ) * ( ( 2.0 / 3.0 ) - vH ) * 6.0 );
   return ( v1 );
}static void saturation_to_xy(uint16 *x, uint16 *y, uint8 sat)
{
  float xDelta, yDelta;
  uint16 localSat;

  localSat = 255 - sat;

  xDelta = (int32)*x - WHITE_POINT_X;
  yDelta = (int32)*y - WHITE_POINT_Y;

  xDelta = xDelta * (float)localSat/0xFF;
  yDelta = yDelta * (float)localSat/0xFF;

  *x = (uint16) (*x - xDelta);
  *y = (uint16) (*y - yDelta);

  return;
}
static uint8 XyToSat(uint16 x, uint16 y, uint8 hue)
{
  uint32 xyDeltas, hueDeltas;
  //we try to avoid polor to cartesan conversion using proportional scaling
  xyDeltas = ((uint32)DISTANCE(x, WHITE_POINT_X) + DISTANCE(y, WHITE_POINT_Y));
  hueDeltas = ((uint32)DISTANCE(hueToX[hue], WHITE_POINT_X) + DISTANCE(hueToY[hue], WHITE_POINT_Y));
  return ( (hueDeltas) ? ((xyDeltas * COLOR_SAT_MAX) / hueDeltas) : COLOR_SAT_MAX );
}


