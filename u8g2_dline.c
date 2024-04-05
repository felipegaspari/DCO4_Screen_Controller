#include "u8g2.h"

void u8g2_DrawDLine(u8g2_t *u8g2, u8g2_uint_t x1, u8g2_uint_t y1, u8g2_uint_t x2, u8g2_uint_t y2, u8g2_int_t d)
{
u8g2_uint_t tmp;
u8g2_uint_t x,y;
u8g2_uint_t dx, dy;
u8g2_int_t err;
u8g2_int_t ystep;

uint8_t swapxy = 0;

/* no intersection check at the moment, should be added... */

if ( x1 > x2 ) dx = x1-x2; else dx = x2-x1;
if ( y1 > y2 ) dy = y1-y2; else dy = y2-y1;

if ( dy > dx )
{
  swapxy = 1;
  tmp = dx; dx =dy; dy = tmp;
  tmp = x1; x1 =y1; y1 = tmp;
  tmp = x2; x2 =y2; y2 = tmp;
}
if ( x1 > x2 )
{
  tmp = x1; x1 =x2; x2 = tmp;
  tmp = y1; y1 =y2; y2 = tmp;
}
err = dx >> 1;
if ( y2 > y1 ) ystep = 1; else ystep = -1;
y = y1;

#ifndef  U8G2_16BIT
if ( x2 == 255 )
x2--;
#else
if ( x2 == 0xffff )
x2--;
#endif

for( x = x1; x <= x2; x++ )
{
  if ( swapxy == 0 )
  {
    if(d==0){
      /* solid line */
       u8g2_DrawPixel(u8g2, x, y);
    }else if(d==1){
      /* dotted line */
      if(x%2==0) u8g2_DrawPixel(u8g2, x, y);
    }else if(d>1){
      /* dashed line */
      if((x/d)%2==0) u8g2_DrawPixel(u8g2, x, y);
    }else if(d<0){
      /* dashed line inverted */
      if((x/-d)%2!=0) u8g2_DrawPixel(u8g2, x, y); 
    }
  }
  else
  {
    if(d==0){
      /* solid line */
      u8g2_DrawPixel(u8g2, y, x);
    }else if(d==1){
      /* dotted line */
      if(x%2==0) u8g2_DrawPixel(u8g2, y, x);
    }else if(d>1){
      /* dashed line */
      if((x/d)%2==0) u8g2_DrawPixel(u8g2, y, x);
    }else if(d<0){
      /* dashed line inverted */
      if((x/-d)%2!=0) u8g2_DrawPixel(u8g2, y, x);
    }
  }
  err -= (uint8_t)dy;
  if ( err < 0 )
  {
    y += (u8g2_uint_t)ystep;
    err += (u8g2_uint_t)dx;
  }
}
}