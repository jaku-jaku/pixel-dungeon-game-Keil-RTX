#include <LPC17xx.h>
#include <RTL.h>
#include <stdlib.h>
#include "stdio.h"
#include "uart.h"
#include "GLCD.h"
#include "led.h"
#include "delay.h"
#include "Controller.h"
#include "GLCDRenderLIB.h"
#include <stdbool.h>
#define DEBUG_MSG(x) printf("LOG :: %s", x)

#define MAP_SCRN_W 16
#define MAP_SCRN_H 12
//#define MAP_TEXEL_SIZE 20
//#define MAP_TEXEL_H MAP_TEXEL_SIZE
//#define MAP_TEXEL_W MAP_TEXEL_SIZE


//settings
#define BULLET_SPEED 3 //per tick
#define BULLET_MAX_QUANT 8
#define BULLET_DEFUALT_QUANT 5
#define BULLET_W 5
#define BULLET_H 5
#define POTENTIO_SENSITIVITY 200 
#define ENEMIES_MAX_QUANT 4
#define ENEMY_MOVING_SPEED 1

typedef struct OBJ_U8{
	uint8_t x;
	uint8_t y;
	//2BIT 0-1:ORIENTATION; (*May Vary: 2:Health(1:2xhealth, 0:1xhealth) 3-6:Enemy Type[TEXT_CONTENT]); 7:Valid/Invalid 
	uint8_t property;// 0b000000000 ||DIRECTION: ..00:x++ ..01:y++ ..10:x-- ..11:y-- | 1.... Valid, 0... Invalid
} ObjectU8_t;

typedef struct OBJ_U16{
	uint16_t x;
	uint16_t y;
	uint8_t property;// 0b000000000 ||DIRECTION: ..00:x++ ..01:y++ ..10:x-- ..11:y-- | 1.... Valid, 0... Invalid
} ObjectU16_t;

ObjectU8_t Hiro;
ObjectU16_t Bullets[BULLET_MAX_QUANT];//NOTE: bullet in world coord
ObjectU8_t Enemies[ENEMIES_MAX_QUANT];
uint8_t Bullet_quant = BULLET_DEFUALT_QUANT; 
OS_MUT mutex_Hiro;
OS_MUT mutex_Enemies;
OS_MUT mutex_map_TEXEL;
OS_MUT mutex_bullets;
//OS_MUT mutex_zombies;
const uint8_t Temp_enemy_pos[8] = {1,8,7,3,8,1,13,10};
const uint16_t MAP[MAP_SCRN_H] = {32767, 16385, 24053, 21781, 21845, 21829, 22397, 20485, 22397, 21825, 24029, 16385};
uint16_t map_TEXEL[MAP_TEXEL_AREA] = {0};
uint16_t prevPotentio = 0;

__task void Task_MenuUpdate()
{
	for(;;)
	{
		os_tsk_pass();
	}
}

void clearTEXEL(unsigned int x, unsigned int y, unsigned int val)
{
	GLCD_Bargraph(x*MAP_TEXEL_SIZE, y*MAP_TEXEL_SIZE, MAP_TEXEL_SIZE, MAP_TEXEL_SIZE, val);
}

TEXEL_CONTENT_t Get_TEXEL_CONTENT(uint8_t x, uint8_t y)
{
		uint16_t j = TC_UNKNOWN;
		if(x < MAP_SCRN_W && y < MAP_SCRN_H)
		{
			os_mut_wait (&mutex_map_TEXEL, 0xffff);
			j = map_TEXEL[x+y*MAP_SCRN_W];
			os_mut_release (&mutex_map_TEXEL);
		}
		return j;
}

TEXEL_CONTENT_t Get_TEXEL_CONTENT_16(uint16_t x, uint16_t y)
{
	  uint16_t j = TC_UNKNOWN;
		x/=MAP_TEXEL_W;
		y/=MAP_TEXEL_H;
		if(x < MAP_SCRN_W && y < MAP_SCRN_H)
		{
			os_mut_wait (&mutex_map_TEXEL, 0xffff);
			j = map_TEXEL[x+y*MAP_SCRN_W];
			os_mut_release (&mutex_map_TEXEL);
		}
		return j;
}

uint8_t PathExist_Screen_Space(uint16_t x, uint16_t y)
{
		return (!(Get_TEXEL_CONTENT_16(x, y)&TC_WALL));
}

void Set_TEXEL_CONTENT(uint8_t x, uint8_t y, uint16_t type)
{
		if(x < MAP_SCRN_W && y < MAP_SCRN_H)
		{
			os_mut_wait (&mutex_map_TEXEL, 0xffff);
			map_TEXEL[x+y*MAP_SCRN_W] |= type;
			os_mut_release (&mutex_map_TEXEL);
		}
}

void Reset_TEXEL_CONTENT(uint8_t x, uint8_t y, uint16_t type)
{
		if(x < MAP_SCRN_W && y < MAP_SCRN_H)
		{
			os_mut_wait (&mutex_map_TEXEL, 0xffff);
			map_TEXEL[x+y*MAP_SCRN_W] &= (~type);
			os_mut_release (&mutex_map_TEXEL);
		}
}

void Set_TEXEL_CONTENT_16(uint16_t x, uint16_t y, uint16_t type)
{
		x/=MAP_TEXEL_W;
		y/=MAP_TEXEL_H;
		if(x < MAP_SCRN_W && y < MAP_SCRN_H)
		{
			os_mut_wait (&mutex_map_TEXEL, 0xffff);
			map_TEXEL[x+y*MAP_SCRN_W] |= type;
			os_mut_release (&mutex_map_TEXEL);
		}
}

void Reset_TEXEL_CONTENT_16(uint16_t x, uint16_t y, uint16_t type)
{
		x/=MAP_TEXEL_W;
		y/=MAP_TEXEL_H;
		if(x < MAP_SCRN_W && y < MAP_SCRN_H)
		{
			os_mut_wait (&mutex_map_TEXEL, 0xffff);
			map_TEXEL[x+y*MAP_SCRN_W] &= (~type);
			os_mut_release (&mutex_map_TEXEL);
		}
}

TEXEL_CONTENT_t Get_Enemy_Orientation(uint8_t x, uint8_t y, uint16_t type)
{
	uint8_t i;
	for(i=0;i<ENEMIES_MAX_QUANT;i++)
	{	
		os_mut_wait (&mutex_Enemies, 0xffff);
		if(Enemies[i].x == x && Enemies[i].y ==y && (Enemies[i].property&((1<<7|type<<1))))//check if valid & same type
		{
			return (Enemies[i].property & 3);//direction mask
		}
		os_mut_release (&mutex_Enemies);
	}
	return 0;//default direction
}


__task void Task_Render()
{
	for(;;)
	{
		uint8_t i, j, cp, enemy_p;
		uint16_t cx, cy, type;
		//GLCD_Clear(Blue);
		GLCD_SetTextColor(Black);//wall
		GLCD_SetBackColor(White);//path
		os_mut_wait (&mutex_Hiro, 0xffff);
		cp = Hiro.property;
		os_mut_release (&mutex_Hiro);
		for(i =0; i<MAP_SCRN_W; i++)
		{
			for(j = 0; j<MAP_SCRN_H; j++)
			{
				type = Get_TEXEL_CONTENT(i, j);
				enemy_p = (((type&TC_ZOMBIE)|(type&TC_GHOST))|type&TC_RAT);//if enemyy exist
				if(type&TC_WALL)//WALL
				{
					clearTEXEL(i, j, 1024);
				}
				else if(type&TC_PLAYER)//PLAYER
				{
					draw_TEXEL(i, j, cp, TC_PLAYER);
				}
				else if(!enemy_p)//PATH
				{
					clearTEXEL(i,j, 0);
				}
				
				if(enemy_p)
				{
					enemy_p = (type&TC_14_15_SPECIAL)>>14;
					if(enemy_p%2==1)//BLINK
					{
						clearTEXEL(i,j, 0);
						Reset_TEXEL_CONTENT(i,j,(1<<14));
					}else
					{
						if(enemy_p==2)
						{
							Set_TEXEL_CONTENT(i,j,1<<14);
						}
						if(type&TC_ZOMBIE)
						{
							enemy_p = Get_Enemy_Orientation(i, j, TC_ZOMBIE);
							draw_TEXEL(i, j, enemy_p, TC_ZOMBIE);
						}
						if(type&TC_GHOST)
						{
							enemy_p = Get_Enemy_Orientation(i, j, TC_GHOST);
							draw_TEXEL(i, j, enemy_p, TC_GHOST);
						}
						if(type&TC_RAT)
						{
							enemy_p = Get_Enemy_Orientation(i, j, TC_RAT);
							draw_TEXEL(i, j, enemy_p, TC_RAT);
						}
					}
				}
			}
		}
		
		//draw Bullets
		GLCD_SetTextColor(Magenta);//bullet clr
		j = 0;//trakcing num of bullet

		for(i = 0; i< BULLET_MAX_QUANT; i++)
		{
			os_mut_wait (&mutex_bullets, 0xffff);
			cp = Bullets[i].property;//exist 
			cx = Bullets[i].x;
			cy = Bullets[i].y;
			j = Bullet_quant;
			os_mut_release (&mutex_bullets);
			
			if(cp>>7)//if exist, render it
			{
				GLCD_Bargraph(cx, cy, BULLET_W, BULLET_H, 1024);
			}
		}
		
		//indicate amount of bullet left
		for(i = 0; i< j; i++)
		{
			LED_set(BULLET_MAX_QUANT - i - 1);
		}
		for(; j< BULLET_MAX_QUANT; j++)
		{
			LED_clear(BULLET_MAX_QUANT- j -1);
		}
		
		os_tsk_pass();
	}
}

__task void Task_InputHandler()
{
	os_itv_set (30);
	for(;;)
	{
		uint8_t joyCommand, texel_property, triggerBullet = 0;
		int8_t x, y, p;
		uint16_t potentioReading;
		int delta = 0;
		os_itv_wait ();
		//read current position
		os_mut_wait (&mutex_Hiro, 0xffff);
		x = Hiro.x;
		y = Hiro.y;
		p = Hiro.property;
		os_mut_release (&mutex_Hiro);

		//read & parse joystick
		joyCommand = readJoystick();		
		if(joyCommand != JS_IDLE)
		{
			switch ((joyCommand) & (~JS_PRESSED))//Check direction
			{
				case JS_UP:
					y--;
					break;
				case JS_DOWN:
					y++;
					break;
				case JS_LEFT:
					x--;
					break;
				case JS_RIGHT:
					x++;
					break;
				default:
					break;
			}
			if(joyCommand & JS_PRESSED)
			{
				triggerBullet = 1;
			}
		}
		//potentiometer => Character rotation
		potentioReading = readPotentiometer();
		delta = (potentioReading-prevPotentio); 
		if(((delta<0)?(-delta):delta)>POTENTIO_SENSITIVITY)
		{
			delta = Hiro.property+(delta<0?1:-1);
			delta = delta<0?3:delta;
			p = delta%4;
			prevPotentio = potentioReading;
		}
		//check if the new position is valid, (walkable check)
		texel_property = Get_TEXEL_CONTENT(x, y);
		if(!(texel_property&TC_WALL))//Not wall
		{
				//if valid, update position
				os_mut_wait (&mutex_Hiro, 0xffff);
				Reset_TEXEL_CONTENT(Hiro.x, Hiro.y, TC_PLAYER);
				Hiro.x = x;
				Hiro.y = y;
				Hiro.property = p;
				os_mut_release (&mutex_Hiro);
				Set_TEXEL_CONTENT(x, y, TC_PLAYER);
		}
		//Bullet Releasing check
		if(triggerBullet && Bullet_quant > 0)
		{
			os_mut_wait (&mutex_bullets, 0xffff);
			//NOTE::Here texel_property is reused as for loop counter
			for(texel_property = 0; texel_property < BULLET_MAX_QUANT; texel_property++)
			{
				//if bullet is not busy
				if(Bullets[texel_property].property == 0)
				{
					Bullets[texel_property].x = x*MAP_TEXEL_W+(MAP_TEXEL_SIZE-BULLET_W)/2;
					Bullets[texel_property].y = y*MAP_TEXEL_H+(MAP_TEXEL_SIZE-BULLET_H)/2;
					Bullets[texel_property].property = (1<<7)|p;//enable the bullet
					Bullet_quant--;
					break;
				}
			}
			os_mut_release (&mutex_bullets); 
		}
		
		//End of one cycle
		os_tsk_pass();
	}
}

__task void Task_CharMapUpdate()
{
	for(;;)
	{
		
		//End of one cycle
		os_tsk_pass();
	}
}

__task void Task_ProjectilesUpdate()
{
	for(;;)
	{
		uint8_t i, j;
		uint16_t bx, by;
		//projectile collision check
		
		//Here isWalkable is used as for loop counter
		for(i = 0; i < BULLET_MAX_QUANT; i++)
		{
				//copy data
			  os_mut_wait (&mutex_bullets, 0xffff);
				j = Bullets[i].property;
				bx = Bullets[i].x;
				by = Bullets[i].y;
				os_mut_release (&mutex_bullets); 

				//check
				//if bullet is a active
				if(j != 0)//|DIRECTION: ..00:x++ ..01:y++ ..10:x-- ..11:y-- 
				{
					//apply motion
   				switch(j&3)
					{
						case 0: 
							bx += BULLET_SPEED;
							break;
						case 1:
							by += BULLET_SPEED;
							break;
						case 2:
							bx -= BULLET_SPEED;
							break;
						case 3:
							by -= BULLET_SPEED;
							break;
					}
					
					j = 1;
					j = Get_TEXEL_CONTENT_16(bx, by);
					//collision check
					if(j&TC_WALL)
					{
							//return bullet
							os_mut_wait (&mutex_bullets, 0xffff);
							Bullets[i].property = 0;
							Bullets[i].x = 0;
							Bullets[i].y = 0;
						  Bullet_quant++;
							os_mut_release (&mutex_bullets); 
					}
					if((j&TC_GHOST)|(j&TC_ZOMBIE)|(j&TC_RAT))
					{
							Set_TEXEL_CONTENT_16(bx, by, TC_14_15_SPECIAL);
						  //return bullet
							os_mut_wait (&mutex_bullets, 0xffff);
							Bullets[i].property = 0;
							Bullets[i].x = 0;
							Bullets[i].y = 0;
						  Bullet_quant++;
							os_mut_release (&mutex_bullets); 
					}
					else
					{
							os_mut_wait (&mutex_bullets, 0xffff);
							Bullets[i].x = bx;
							Bullets[i].y = by;
							os_mut_release (&mutex_bullets); 
					}
				}
		}
		
		
		os_tsk_pass();
	}
}

__task void Task_TrapUpdate()
{
	for(;;)
	{
		os_tsk_pass();
	}
}

__task void Task_EnemyUpdate()
{
	for(;;)
	{
	
		
/* NOT WORKING
		
			uint8_t i, ex, ey, ep, valid, type;
		uint16_t texel_type;
		LED_display(222);

		for(i = 0; i< ENEMIES_MAX_QUANT; i++)
		{
			os_mut_wait (&mutex_Enemies, 0xffff);
			ex = Enemies[i].x;
			ey = Enemies[i].y;
			ep = Enemies[i].property;
			os_mut_release (&mutex_Enemies);
			
			//if(1)//if exist
			{
				valid = 1;//0 update nothing, 1 update position, 2 update property, 3 update all
				//update position !!
				
					switch(ep&3)//filter first 2 bits to find correct orientation
					{
						case 0: 
							ex += ENEMY_MOVING_SPEED;
							break;
						case 1:
							ey += ENEMY_MOVING_SPEED;
							break;
						case 2:
							ex -= ENEMY_MOVING_SPEED;
							break;
						case 3:
							ey -= ENEMY_MOVING_SPEED;
							break;
					}

				//position check:
					texel_type = Get_TEXEL_CONTENT(ex, ey);
					type = ((ep>>3)&7)<<2;//filter enemy types & reposition to match texel_content format
					switch(TC_ZOMBIE)
					{
						case TC_ZOMBIE: 
							
								//if(texel_type&TC_WALL)//wall: zombie will rotate 
								{
									ep = ((ep&3+1)%4)|(ep&(~3));//CW rotation
									valid = 2;
								}
								break;
							
						case TC_GHOST:
							{
								if(texel_type&TC_WALL)
								{
									
								}
								break;
							}
						case TC_RAT:
							{
								if(texel_type&TC_WALL)
								{
								}
								break;
							}
					}

					//update data
					os_mut_wait (&mutex_Enemies, 0xffff);
	//				if(valid==1||valid==3)
					{
						Reset_TEXEL_CONTENT(Enemies[i].x, Enemies[i].y, type);
						Enemies[i].x = 0;
						Enemies[i].y = 0;
						Set_TEXEL_CONTENT(0, 0, type);
					}
//					if(valid==2||valid==3)
//					{
//						Enemies[i].property = ep;//apply
//					}
					os_mut_release (&mutex_Enemies);
					
			}
		}
		*/
		
		os_tsk_pass();
	}
}



void start_tasks()
{
	os_mut_init (&mutex_Hiro);
	os_mut_init (&mutex_map_TEXEL);
	os_mut_init (&mutex_bullets);
	os_mut_init (&mutex_Enemies);
	//
	//os_tsk_create(Task_MenuUpdate, 1);
	os_tsk_create(Task_Render, 1);
	os_tsk_create(Task_InputHandler, 1);
	os_tsk_create(Task_CharMapUpdate,1);
	os_tsk_create(Task_ProjectilesUpdate, 1);
	//os_tsk_create(Task_TrapUpdate, 1);
	os_tsk_create(Task_EnemyUpdate, 1);
	os_tsk_delete_self();
}

void init()
{
	uint8_t i, j, isPath, random, counter_enemies = 0;
	LED_setup();
	initJoystick();
	initPotentiometer();
	initPushBtn();
	GLCD_Init();

	//Game Objs Settings
	//Hiro 's initial position
	Hiro.x = 1;
	Hiro.y = 1;
	prevPotentio = readPotentiometer();
	//Load Map:
	map_TEXEL[Hiro.x + Hiro.y*MAP_SCRN_W] = TC_PLAYER;

	for(i =0; i<MAP_SCRN_W; i++)
	{
		for(j = 0; j<MAP_SCRN_H; j++)
		{
			isPath = (MAP[j]&(1<<i))?TC_WALL:TC_PATH;
			map_TEXEL[i+j*MAP_SCRN_W] |= isPath;
			//TODO: find auto generating algo
			if( i==Temp_enemy_pos[0+counter_enemies*2] && j==Temp_enemy_pos[1+counter_enemies*2] && counter_enemies <= ENEMIES_MAX_QUANT)
			{
				Enemies[counter_enemies].x = i;
				Enemies[counter_enemies].y = j;
				random  = ((i+j)%3) +2;//max 3 types //random type
				Enemies[counter_enemies].property = ((1<<(random+1))|(1<<7)); //random type + active 
				map_TEXEL[i+j*MAP_SCRN_W] |= 1<<random;
				random  = (i-j)%4;//4 orientations
				Enemies[counter_enemies].property |= random;// + random orientation
				counter_enemies++;
			}
		}
	}
	
}
int main( void ) 
{
	init();
	DEBUG_MSG("Dungeon Begins");
	os_sys_init(start_tasks);
}
