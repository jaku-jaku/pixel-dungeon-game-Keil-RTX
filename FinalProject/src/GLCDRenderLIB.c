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
								 

//unsigned short bitmap_time_capsule[] = {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,B,B,B,B,B,W,W,W,W,W,W,W,W,
//								 W,W,W,W,W,B,B,Y,Y,Y,Y,Y,B,B,W,W,W,W,W,W,
//								 W,W,W,W,B,Y,Y,Y,Y,Y,Y,Y,Y,Y,B,W,W,W,W,W,
//								 W,W,W,B,Y,Y,Y,Y,Y,B,Y,Y,Y,Y,Y,B,W,W,W,W,
//								 W,W,B,Y,Y,Y,Y,Y,Y,B,Y,Y,Y,Y,Y,Y,B,W,W,W,
//								 W,W,B,Y,Y,Y,Y,Y,Y,B,Y,Y,Y,Y,Y,Y,B,W,W,W,
//								 W,B,Y,Y,Y,Y,Y,Y,Y,B,Y,Y,Y,Y,Y,Y,Y,B,W,W,
//								 W,B,Y,Y,Y,Y,Y,Y,Y,B,Y,Y,Y,Y,Y,Y,Y,B,W,W,
//								 W,B,Y,Y,Y,Y,Y,B,B,B,B,B,B,B,Y,Y,Y,B,W,W,
//								 W,B,Y,Y,Y,Y,Y,Y,Y,B,Y,Y,Y,Y,Y,Y,Y,B,W,W,
//								 W,B,Y,Y,Y,Y,Y,Y,Y,B,Y,Y,Y,Y,Y,Y,Y,B,W,W,
//								 W,W,B,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,B,W,W,W,
//								 W,W,B,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,B,W,W,W,
//								 W,W,W,B,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,B,W,W,W,W,
//								 W,W,W,W,B,Y,Y,Y,Y,Y,Y,Y,Y,Y,B,W,W,W,W,W,
//								 W,W,W,W,W,B,B,Y,Y,Y,Y,Y,B,B,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,B,B,B,B,B,W,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}; //time capsule


unsigned short bitmap_portal[] = {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
								 W,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,
								 W,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,B,W,
								 W,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,B,W,
								 W,B,W,W,B,B,B,B,B,B,B,B,B,B,B,B,W,W,B,W,
								 W,B,W,W,B,P,P,P,P,P,P,P,P,P,P,B,W,W,B,W,
								 W,B,W,W,B,P,P,P,P,P,P,P,P,P,P,B,W,W,B,W,
								 W,B,W,W,B,P,P,P,P,P,P,P,P,P,P,B,W,W,B,W,
								 W,B,W,W,B,P,P,P,B,B,B,B,P,P,P,B,W,W,B,W,
								 W,B,W,W,B,P,P,P,B,W,W,B,P,P,P,B,W,W,B,W,
								 W,B,W,W,B,P,P,P,B,W,W,B,P,P,P,B,W,W,B,W,
								 W,B,W,W,B,P,P,P,B,B,B,B,P,P,P,B,W,W,B,W,
								 W,B,W,W,B,P,P,P,P,P,P,P,P,P,P,B,W,W,B,W,
								 W,B,W,W,B,P,P,P,P,P,P,P,P,P,P,B,W,W,B,W,
								 W,B,W,W,B,P,P,P,P,P,P,P,P,P,P,B,W,W,B,W,
								 W,B,W,W,B,B,B,B,B,B,B,B,B,B,B,B,W,W,B,W,
								 W,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,B,W,
								 W,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,B,W,
								 W,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,
								 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}; //portal

//unsigned short bitmap_bullet_clip[] = {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
//								 W,W,W,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,
//								 W,W,W,B,G,G,G,G,G,G,G,G,G,G,B,G,G,B,W,W,
//								 W,W,W,B,G,G,G,G,G,G,G,G,G,G,B,G,G,B,W,W,
//								 W,W,W,B,B,B,B,B,B,B,B,B,B,B,B,G,G,B,W,W,
//								 W,W,W,B,G,G,G,G,G,G,G,G,G,G,B,G,G,B,W,W,
//								 W,W,W,B,G,G,G,G,G,G,G,G,G,G,B,G,G,B,W,W,
//								 W,W,W,B,G,G,G,G,G,G,G,G,G,G,B,G,G,B,W,W,
//								 W,W,W,B,B,B,B,B,B,B,B,B,B,B,B,G,G,B,W,W,
//								 W,W,W,B,G,G,G,G,G,G,G,G,G,G,B,G,G,B,W,W,
//								 W,W,W,B,G,G,G,G,G,G,G,G,G,G,B,G,G,B,W,W,
//								 W,W,W,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,
//								 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}; //bullet clip
//																		 

//unsigned short bitmap_laser_on[MAP_TEXEL_AREA] = 
//								{B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,N,N,N,N,N,N,N,N,N,N,N,N,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B}; //laser on
//								 
//
//unsigned short bitmap_laser_off[MAP_TEXEL_AREA] = 
//								{B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,L,L,L,L,L,L,L,L,L,L,L,L,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B}; //laser off
//
//unsigned short bitmap_laser_beam[MAP_TEXEL_AREA] = 
//								{W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W,
//								 W,W,W,W,W,W,W,N,N,N,N,N,N,W,W,W,W,W,W,W}; //laser on, laser beam
								 
//unsigned short bitmap_fire_on2[MAP_TEXEL_AREA] = 
//								{Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,
//								 Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,
//								 Y,Y,R,R,R,R,R,R,Y,Y,Y,Y,R,R,R,R,R,R,Y,Y,
//								 Y,Y,R,R,R,R,R,R,Y,Y,Y,Y,R,R,R,R,R,R,Y,Y,
//								 Y,Y,R,R,R,R,R,R,Y,Y,Y,Y,R,R,R,R,R,R,Y,Y,
//								 Y,Y,R,R,R,R,R,R,Y,Y,Y,Y,R,R,R,R,R,R,Y,Y,
//								 Y,Y,R,R,R,R,R,R,Y,Y,Y,Y,R,R,R,R,R,R,Y,Y,
//								 Y,Y,R,R,R,R,R,R,Y,Y,Y,Y,R,R,R,R,R,R,Y,Y,
//								 Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,
//								 Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,
//								 Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,
//								 Y,Y,R,R,R,R,R,R,Y,Y,Y,Y,R,R,R,R,R,R,Y,Y,
//								 Y,Y,R,R,R,R,R,R,Y,Y,Y,Y,R,R,R,R,R,R,Y,Y,
//								 Y,Y,R,R,R,R,R,R,Y,Y,Y,Y,R,R,R,R,R,R,Y,Y,
//								 Y,Y,R,R,R,R,R,R,Y,Y,Y,Y,R,R,R,R,R,R,Y,Y,
//								 Y,Y,R,R,R,R,R,R,Y,Y,Y,Y,R,R,R,R,R,R,Y,Y,
//								 Y,Y,R,R,R,R,R,R,Y,Y,Y,Y,R,R,R,R,R,R,Y,Y,
//								 Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,
//								 Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y}; //fire on 2
//
//unsigned short bitmap_fire_off[MAP_TEXEL_AREA] = 
//								{B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,M,M,M,M,M,M,M,M,M,M,M,M,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B}; //fire off
//
//unsigned short bitmap_fire_on1[MAP_TEXEL_AREA] = 
//								{B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,R,R,R,R,R,R,R,R,R,R,R,R,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,
//								 B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B}; //fire on 1
								 								 
								 
unsigned short bitmap_spike_off[] = {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
									 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,
									 W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,
									 W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,
									 W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
									 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
									 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,
									 W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,
									 W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,
									 W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,W,W,B,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
									 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}; //spikes off


unsigned short bitmap_spike_on[] = {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
									 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
									 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
									 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,B,B,B,B,B,B,W,W,W,W,B,B,B,B,B,B,W,W,
									 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
									 W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}; //spikes on






void draw_TEXEL(unsigned int x_, unsigned int y_, unsigned int dir_, TEXEL_CONTENT_t type_)
{
	unsigned short* bitmap;
	switch(type_)
	{
		case TC_PLAYER:
			bitmap = bitmap_player;
			break;
		case TC_ZOMBIE:
			bitmap = bitmap_zombie;
			break;
		case TC_GHOST:
			bitmap = bitmap_ghost;
			break;
		case TC_RAT:
			bitmap = bitmap_rat;
			break;
		case TC_POTION:
			bitmap = bitmap_potion;
			break;
//		case TC_TIME_CAP:
//			bitmap = bitmap_time_capsule;
//			break;
//		case TC_BULLET_CLIP:
//			bitmap = bitmap_bullet_clip;
//			break;
		case TC_PORTAL:
			bitmap = bitmap_portal;
			break;
		case TC_TRAP_NEEDLE_OFF:
			bitmap = bitmap_spike_off;
			break;
		case TC_TRAP_NEEDLE_ON:
			bitmap = bitmap_spike_on;
			break;
//		case TC_TRAP_FIRE_OFF:
//			bitmap = bitmap_fire_off;
//			break;
//		case TC_TRAP_FIRE_ON:
//			bitmap = bitmap_fire_on1;
//			break;
//		case TC_TRAP_FIRE_ON_PATH:
//			bitmap = bitmap_fire_on2;
//			break;
//		case TC_TRAP_LASER_OFF:
//			bitmap = bitmap_laser_on;
//			break;
//		case TC_TRAP_LASER_ON:
//			bitmap = bitmap_laser_off;
//			break;
//		case TC_TRAP_LASER_ON_PATH:
//			bitmap = bitmap_laser_beam;
//			break;
	}
	GLCD_Bitmap_ROT (x_*MAP_TEXEL_W, y_*MAP_TEXEL_H, MAP_TEXEL_W, MAP_TEXEL_H, dir_, (unsigned char*)bitmap);
}
								 
