#define MAP_TEXEL_SIZE 20
#define MAP_TEXEL_H MAP_TEXEL_SIZE
#define MAP_TEXEL_W MAP_TEXEL_SIZE
#define MAP_TEXEL_AREA MAP_TEXEL_W*MAP_TEXEL_H

typedef enum TEXEL_CONTENT {PLAYER, ZOMBIE, GHOST, RAT, POTION} TEXEL_CONTENT_t;
void draw_TEXEL(unsigned int x_, unsigned int y_,  unsigned int dir_, TEXEL_CONTENT_t type_);

 
