#ifndef __ZB_COLOR_H__
#define __ZB_COLOR_H__

#include "osCore.h"

typedef struct
{
	float H;
	float S;
	float V;
}color_hsv_t;

typedef struct
{
	float H;
	float S;
	float L;
}color_hsl_t;

typedef struct
{
	float x;
	float y;
}color_xy_t;

typedef struct
{
	uint8 hue;
	uint8 saturation;
	uint8 level;
}color_hue_t;

typedef struct
{
	uint8 R;
	uint8 G;
	uint8 B;
}color_rgb_t;

void color_standard_rgb_to_xy(float *pX, float *pY, float *pZ, color_rgb_t *pRGB);

float color_rgb_to_xy(color_xy_t *pXY, color_rgb_t *pRGB);
void color_xyl_to_rgb(color_rgb_t *pRGB,color_xy_t *pXY, float LinearY);


void color_huesat_to_xy(color_xy_t *pXY,color_hue_t *pHue);
void color_xy_to_huesat(color_hue_t *pHue, color_xy_t *pXY);

void color_rgb_to_rgbw( color_rgb_t *pRGB, uint16 *W );

void color_rgb_to_hsv(color_hsv_t *pHSV,color_rgb_t *pRGB);
void color_hsv_to_rgb(color_rgb_t *pRGB,color_hsv_t *pHSV);

void color_rgb_to_hsl(color_hsl_t *pHSL, color_rgb_t *pRGB);
void color_hsl_to_rgb(color_rgb_t *pRGB, color_hsl_t *pHSL);

#endif
