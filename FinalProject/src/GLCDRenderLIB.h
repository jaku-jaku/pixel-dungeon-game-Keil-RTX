#define MAP_TEXEL_SIZE 20
#define MAP_TEXEL_H MAP_TEXEL_SIZE
#define MAP_TEXEL_W MAP_TEXEL_SIZE
#define MAP_TEXEL_AREA MAP_TEXEL_W*MAP_TEXEL_H

//U16 encoding 
typedef enum TEXEL_CONTENT {
							TC_PATH	=0,
							TC_WALL	=1,
							
							TC_PLAYER	=1<<1, 
							
							TC_ZOMBIE	=1<<2, 
							TC_GHOST	=1<<3,
							TC_RAT		=1<<4, 
							
							TC_POTION		=1<<5, 
							TC_TIME_CAP		=1<<6,
							TC_BULLET_CLIP	=1<<7,
							TC_PORTAL 		=1<<8,
							
							TC_TRAP_NEEDLE =1<<9,
							TC_TRAP_NEEDLE_STATUS = 1<<10,
							TC_TRAP_FIRE_LASER_EXIST =1<<11,
							TC_TRAP_FIRE_LASER =1<<12, //1 fire, 0 laser
							TC_TRAP_FIRE_LASER_STATUS = 1<<13,
							
							//BIT 14, 15:
							// if exist fire/laser trap: orientation of laser
							// if its player or zombie....: BLINK_cnter (2 times) blink at odd number
							TC_14_15_SPECIAL = 3<<14, 
							
							TC_UNKNOWN = 65535 //1111 1111 1111 1111
							} TEXEL_CONTENT_t;


void draw_TEXEL(unsigned int x_, unsigned int y_,  unsigned int dir_, TEXEL_CONTENT_t type_);

 
