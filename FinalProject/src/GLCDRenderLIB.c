#include "GLCDRenderLIB.h"
#include <LPC17xx.h>
#include "stdio.h"
#include "uart.h"
#include "glcd.h"

#define W     White
#define B     Black
#define D     DarkGrey
#define I     LightGrey
#define F     DarkGreen
#define G     Green
#define O     Olive
#define L     Blue
#define N     Navy
#define U     DarkCyan
#define C     Cyan
#define M     Maroon
#define P     Purple
#define R     Red
#define E     Magenta
#define Y     Yellow


unsigned short bitmap_player[MAP_TEXEL_AREA] = 
								{W,W,W,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,
								 W,W,B,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,B,W,W,W,
								 W,W,B,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,B,W,W,
								 W,W,B,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,B,W,W,
								 B,B,B,B,B,B,B,B,B,B,B,B,Y,Y,Y,Y,Y,B,W,W,
								 B,M,M,M,M,M,M,M,M,M,B,B,B,B,B,B,B,B,B,B,
								 B,M,M,M,M,M,M,M,M,M,B,W,W,B,D,D,D,G,G,B,
								 B,M,M,M,M,M,M,M,M,M,B,W,W,B,D,D,D,G,G,B,
								 B,M,M,M,M,M,M,M,M,M,B,W,W,B,D,D,D,G,G,B,
								 B,M,M,M,M,M,M,M,M,M,B,W,W,B,D,D,D,G,G,B,
								 B,M,M,M,M,M,M,M,M,M,B,W,W,B,B,B,B,B,B,B,
								 B,M,M,M,M,M,M,M,M,M,B,W,W,W,W,W,W,W,W,W,
								 B,M,M,M,M,M,M,M,M,M,B,W,W,W,W,W,W,W,W,W,
								 B,M,M,M,M,M,M,M,M,M,B,W,W,W,W,W,W,W,W,W,
								 B,M,M,M,M,M,M,M,M,M,B,W,W,W,W,W,W,W,W,W,
								 B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,W,W,W,W,W,
								 W,W,B,Y,Y,Y,Y,Y,Y,B,W,W,W,W,W,W,W,W,W,W,
								 W,W,B,Y,Y,Y,Y,Y,Y,B,W,W,W,W,W,W,W,W,W,W,
								 W,W,B,Y,Y,Y,Y,Y,Y,B,W,W,W,W,W,W,W,W,W,W,
								 W,W,B,B,B,B,B,B,B,B,W,W,W,W,W,W,W,W,W,W}; // player
								 
unsigned short bitmap_zombie[MAP_TEXEL_AREA] = 
								{W,W,W,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,
								 W,W,B,F,F,F,F,F,F,F,F,F,F,F,F,F,B,W,W,W,
								 W,W,B,F,F,F,F,F,F,F,F,F,F,F,F,F,F,B,W,W,
								 W,W,B,F,F,F,F,F,F,F,F,F,F,F,F,F,F,B,W,W,
								 B,B,B,B,B,B,B,B,B,B,B,B,F,F,F,F,F,B,W,W,
								 B,F,F,F,F,F,F,F,F,F,B,B,B,B,B,B,B,B,W,W,
								 B,F,F,F,F,F,F,F,F,F,B,W,W,W,W,W,W,W,W,W,
								 B,F,F,F,F,F,F,F,F,F,B,W,W,W,W,W,W,W,W,W,
								 B,F,F,F,F,F,F,F,F,F,B,W,W,W,W,W,W,W,W,W,
								 B,F,F,F,F,F,F,F,F,F,B,W,W,W,W,W,W,W,W,W,
								 B,F,F,F,F,F,F,F,F,F,B,W,W,W,W,W,W,W,W,W,
								 B,F,F,F,F,F,F,F,F,F,B,W,W,W,W,W,W,W,W,W,
								 B,F,F,F,F,F,F,F,F,F,B,W,W,W,W,W,W,W,W,W,
								 B,F,F,F,F,F,F,F,F,F,B,W,W,W,W,W,W,W,W,W,
								 B,F,F,F,F,F,F,F,F,F,B,B,B,B,B,B,B,B,W,W,
								 B,B,B,B,B,B,B,B,B,B,B,B,F,F,F,F,F,B,W,W,
								 W,W,B,F,F,F,F,F,F,F,F,F,F,F,F,F,F,B,W,W,
								 W,W,B,F,F,F,F,F,F,F,F,F,F,F,F,F,F,B,W,W,
								 W,W,B,F,F,F,F,F,F,F,F,F,F,F,F,F,B,W,W,W,
								 W,W,W,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W}; //zombie
								
unsigned short bitmap_potion[MAP_TEXEL_AREA] = 
								{W,W,W,W,W,W,W,B,B,B,B,W,W,W,W,W,W,W,W,W,
								 W,W,W,W,W,W,B,B,B,B,B,W,W,W,W,W,W,W,W,W,
								 W,W,W,W,W,B,B,R,R,R,B,B,W,W,W,W,W,W,W,W,
								 W,W,W,W,B,B,R,R,R,R,R,B,W,W,W,W,W,W,W,W,
								 W,W,W,B,B,R,R,R,R,R,R,B,B,W,W,W,W,W,W,W,
								 W,W,B,B,R,R,R,R,R,R,R,C,B,W,W,W,W,W,W,W,
								 W,B,B,R,R,R,R,R,R,R,R,C,B,B,B,B,B,B,B,B,
								 B,B,R,R,R,R,R,R,R,R,R,C,C,B,B,B,B,B,B,B,
								 B,B,R,R,R,R,R,R,R,R,R,C,C,C,C,C,C,C,B,B,
								 B,B,R,R,R,R,R,R,R,R,R,C,C,C,C,C,C,C,B,B,
								 B,B,R,R,R,R,R,R,R,R,R,C,C,C,C,C,C,C,B,B,
								 B,B,R,R,R,R,R,R,R,R,R,C,C,C,C,C,C,C,B,B,
								 B,B,R,R,R,R,R,R,R,R,R,C,C,B,B,B,B,B,B,B,
								 W,B,B,R,R,R,R,R,R,R,R,C,B,B,B,B,B,B,B,B,
								 W,W,B,B,R,R,R,R,R,R,R,C,B,W,W,W,W,W,W,W,
								 W,W,W,B,B,R,R,R,R,R,R,B,B,W,W,W,W,W,W,W,
								 W,W,W,W,B,B,R,R,R,R,R,B,W,W,W,W,W,W,W,W,
								 W,W,W,W,W,B,B,R,R,R,B,B,W,W,W,W,W,W,W,W,
								 W,W,W,W,W,W,B,B,B,B,B,W,W,W,W,W,W,W,W,W,
								 W,W,W,W,W,W,W,B,B,B,B,W,W,W,W,W,W,W,W,W}; //potion
								 
					
unsigned short bitmap_ghost[MAP_TEXEL_AREA] = 
								{B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,W,W,
								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,W,
								 B,B,U,U,U,U,U,U,U,U,U,U,U,U,B,B,W,W,W,W,
								 B,B,U,U,U,U,U,U,U,U,U,U,U,U,U,B,B,W,W,W,
								 B,B,U,U,U,U,U,U,U,U,U,W,W,W,W,U,B,B,W,W,
								 W,W,B,B,U,U,U,U,U,U,U,W,L,L,W,U,U,B,B,W,
								 W,W,B,B,U,U,U,U,U,U,U,W,L,L,W,U,U,U,B,B,
								 W,W,B,B,U,U,U,U,U,U,U,W,W,W,W,U,U,U,B,B,
								 B,B,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,B,B,
								 B,B,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,B,B,
								 B,B,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,B,B,
								 B,B,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,B,B,
								 W,W,B,B,U,U,U,U,U,U,U,W,W,W,W,U,U,U,B,B,
								 W,W,B,B,U,U,U,U,U,U,U,W,L,L,W,U,U,U,B,B,
								 W,W,B,B,U,U,U,U,U,U,U,W,L,L,W,U,U,B,B,W,
								 B,B,U,U,U,U,U,U,U,U,U,W,W,W,W,U,B,B,W,W,
								 B,B,U,U,U,U,U,U,U,U,U,U,U,U,U,B,B,W,W,W,
								 B,B,U,U,U,U,U,U,U,U,U,U,U,U,B,B,W,W,W,W,
								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,W,
								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,W,W}; //ghost
								 
unsigned short bitmap_rat[MAP_TEXEL_AREA] = 
								{B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,W,W,
								 B,E,E,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,W,
								 B,E,E,B,D,D,D,D,D,D,D,D,D,D,B,B,W,W,W,W,
								 B,E,E,B,D,D,D,D,D,D,D,D,D,D,D,B,B,W,W,W,
								 B,E,E,B,D,D,D,D,D,D,D,D,D,D,D,D,B,B,W,W,
								 B,E,E,B,D,D,D,D,D,D,D,D,D,D,D,D,D,B,B,W,
								 B,E,E,B,D,D,D,D,D,D,D,W,W,W,W,D,D,D,B,B,
								 B,E,E,E,B,D,D,D,D,D,D,W,B,B,W,D,D,D,B,B,
								 B,E,E,E,E,B,B,D,D,D,D,W,B,B,W,D,D,D,B,B,
								 B,B,E,E,E,E,E,B,D,D,D,D,W,W,D,D,E,E,B,B,
								 W,W,B,B,E,E,E,B,D,D,D,D,W,W,D,D,E,E,B,B,
								 W,W,W,B,B,B,B,D,D,D,D,W,B,B,W,D,D,D,B,B,
								 W,W,W,B,D,D,D,D,D,D,D,W,B,B,W,D,D,D,B,B,
								 W,W,W,B,D,D,D,D,D,D,D,W,W,W,W,D,D,D,B,B,
								 W,W,W,B,D,D,D,D,D,D,D,D,D,D,D,D,D,B,B,W,
								 W,W,B,B,D,D,D,D,D,D,D,D,D,D,D,D,B,B,W,W,
								 W,W,B,D,D,D,D,D,D,D,D,D,D,D,D,B,B,W,W,W,
								 W,W,B,B,D,D,D,D,D,D,D,D,D,D,B,B,W,W,W,W,
								 W,W,W,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,W,
								 W,W,W,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,W,W}; //rat
								 
								 
void draw_TEXEL(unsigned int x_, unsigned int y_, unsigned int dir_, TEXEL_CONTENT_t type_)
{
	switch(type_)
	{
		case PLAYER:
			GLCD_Bitmap_ROT (x_*MAP_TEXEL_W, y_*MAP_TEXEL_H, MAP_TEXEL_W, MAP_TEXEL_H, dir_, (unsigned char*)bitmap_player);
			break;
		case ZOMBIE:
			GLCD_Bitmap_ROT (x_*MAP_TEXEL_W, y_*MAP_TEXEL_H, MAP_TEXEL_W, MAP_TEXEL_H, dir_, (unsigned char*)bitmap_zombie);
			break;
		case GHOST:
			GLCD_Bitmap_ROT (x_*MAP_TEXEL_W, y_*MAP_TEXEL_H, MAP_TEXEL_W, MAP_TEXEL_H, dir_, (unsigned char*)bitmap_ghost);
			break;
		case RAT:
			GLCD_Bitmap_ROT (x_*MAP_TEXEL_W, y_*MAP_TEXEL_H, MAP_TEXEL_W, MAP_TEXEL_H, dir_, (unsigned char*)bitmap_rat);
			break;
		case POTION:
			GLCD_Bitmap_ROT (x_*MAP_TEXEL_W, y_*MAP_TEXEL_H, MAP_TEXEL_W, MAP_TEXEL_H, dir_, (unsigned char*)bitmap_potion);
			break;
	}
}
								 
