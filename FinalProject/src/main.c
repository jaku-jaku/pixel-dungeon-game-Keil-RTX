#include <LPC17xx.h>
#include <RTL.h>
#include "stdio.h"
#include "uart.h"
#include "GLCD.h"
#include "led.h"
#include "delay.h"
#include "Controller.h"
#include "GLCDRenderLIB.h"
#include <stdbool.h>
#define DEBUG_MSG(x) printf("LOG :: %s", x)

#define MAP_SCRN_H 16
#define MAP_SCRN_W 12
//#define MAP_TEXEL_SIZE 20
//#define MAP_TEXEL_H MAP_TEXEL_SIZE
//#define MAP_TEXEL_W MAP_TEXEL_SIZE


//settings
#define BULLET_SPEED 1 //per tick
#define BULLET_MAX_QUANT 8
#define BULLET_DEFUALT_QUANT 5
#define BULLET_W 5
#define BULLET_H 5
#define POTENTIO_SENSITIVITY 200 

typedef struct OBJ{
	uint8_t x;
	uint8_t y;
	uint8_t property;// 0b000000000 ||DIRECTION: ..00:x++ ..01:y++ ..10:x-- ..11:y-- | 1.... Valid, 0... Invalid
} Object_t;

Object_t Hiro;
Object_t Bullets[BULLET_MAX_QUANT];//NOTE: bullet in world coord
uint8_t Bullet_quant = BULLET_DEFUALT_QUANT; 
OS_MUT mutex_Hiro;
OS_MUT mutex_map;
OS_MUT mutex_bullets;
uint16_t map[MAP_SCRN_W] = {32767, 16385, 24053, 21781, 21845, 21829, 22397, 20485, 22397, 21825, 24029, 16385};
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
	GLCD_Bargraph(y*MAP_TEXEL_SIZE, x*MAP_TEXEL_SIZE, MAP_TEXEL_SIZE, MAP_TEXEL_SIZE, val);
}

void renderBullet(unsigned int x, unsigned int y, unsigned int size)
{
	uint8_t i, j;
	GLCD_SetTextColor(Red);
	GLCD_SetBackColor(Green);
	for(i = 0; i<size; i++)
	{
		for(j = 0; j<size; j++)
		{
			GLCD_PutPixel(x+i, y+j);
		}
	}
}

uint8_t PathExist_TEXEL_Space(uint8_t x, uint8_t y)
{
		int j = 0;
		os_mut_wait (&mutex_map, 0xffff);
		j = (map[y]&(1<<x))?0:1;
		os_mut_release (&mutex_map);
		return j;
}

uint8_t PathExist_Screen_Space(uint16_t x, uint16_t y)
{
		int j= 0;
		os_mut_wait (&mutex_map, 0xffff);
		j = (map[((uint8_t)((y+1)/MAP_TEXEL_H))]&(1<<((uint8_t)((x+1)/MAP_TEXEL_W))))?0:1;
		os_mut_release (&mutex_map);
		return j;
}

__task void Task_Render()
{
	for(;;)
	{
		uint8_t i, j, isWall, cx, cy, cp;
		//GLCD_Clear(Blue);
		GLCD_SetTextColor(Black);//wall
		cx = Hiro.x;
		cy = Hiro.y;
		cp = Hiro.property;
		GLCD_SetBackColor(White);//path
		os_mut_wait (&mutex_Hiro, 0xffff);
		os_mut_release (&mutex_Hiro);
		for(i =0; i<MAP_SCRN_W; i++)
		{
			for(j = 0; j< MAP_SCRN_H; j++)
			{
				isWall = PathExist_TEXEL_Space(j, i)?0:1;

				if(isWall)
				{
					clearTEXEL(i,j, 1024);
				}else if(cx == j && cy == i)
				{
					draw_TEXEL(cx, cy, cp, PLAYER);
				}else
				{
					clearTEXEL(i,j, 0);
				}
			}
		}
		
		//draw Bullets
		GLCD_SetBackColor(Magenta);//bullet clr
		isWall = 0;//trakcing num of bullet
		for(i = 0; i< BULLET_MAX_QUANT; i++)
		{
			j = 0;//doesnt exist
			os_mut_wait (&mutex_bullets, 0xffff);
			if((Bullets[i].property)>>7)
			{
				j = 1;//exist 
				cx = Bullets[i].x;
				cy = Bullets[i].y;
			}
			isWall = Bullet_quant;
			os_mut_release (&mutex_bullets);
			if(j)
				GLCD_Bargraph(cx, cy, BULLET_W, BULLET_H, 0);
		}
		
		//indicate amount of bullet left
		for(i = 0; i< isWall; i++)
		{
			LED_set(BULLET_MAX_QUANT - i - 1);
		}
		for(j = isWall; j< BULLET_MAX_QUANT; j++)
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
		uint8_t joyCommand, isWalkable, triggerBullet = 0;
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
		isWalkable = PathExist_TEXEL_Space(x, y);
		if(isWalkable)
		{
			//also check if position within the map
			if(x>=0 && x<MAP_SCRN_H && y>=0 && y<MAP_SCRN_W)
			{
				//if valid, update position
				os_mut_wait (&mutex_Hiro, 0xffff);
				Hiro.x = x;
				Hiro.y = y;
				Hiro.property = p;
				os_mut_release (&mutex_Hiro);
			}
		}
		//Bullet Releasing check
		if(triggerBullet && Bullet_quant > 0)
		{
			os_mut_wait (&mutex_bullets, 0xffff);
			//Here isWalkable is used as for loop counter
			for(isWalkable = 0; isWalkable < BULLET_MAX_QUANT; isWalkable++)
			{
				//if bullet is not busy
				if(Bullets[isWalkable].property == 0)
				{
					Bullets[isWalkable].x = x*MAP_TEXEL_W+(MAP_TEXEL_SIZE-BULLET_W)/2;
					Bullets[isWalkable].y = y*MAP_TEXEL_H+(MAP_TEXEL_SIZE-BULLET_H)/2;
					Bullets[isWalkable].property = (1<<7)|p;//enable the bullet
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
		uint8_t i, j, bx, by;
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

					j = PathExist_Screen_Space(bx, by);
					//collision check
					if(j)
					{
							os_mut_wait (&mutex_bullets, 0xffff);
							Bullets[i].x = bx;
							Bullets[i].y = by;
							os_mut_release (&mutex_bullets); 
					}else
					{
						  //return bullet
							os_mut_wait (&mutex_bullets, 0xffff);
							Bullets[i].property = 0;
							Bullets[i].x = 0;
							Bullets[i].y = 0;
						  Bullet_quant++;
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
		os_tsk_pass();
	}
}



void start_tasks()
{
	os_mut_init (&mutex_Hiro);
	os_mut_init (&mutex_map);
	//
	os_tsk_create(Task_MenuUpdate, 1);
	os_tsk_create(Task_Render, 1);
	os_tsk_create(Task_InputHandler, 1);
	os_tsk_create(Task_CharMapUpdate,1);
	os_tsk_create(Task_ProjectilesUpdate, 1);
	os_tsk_create(Task_TrapUpdate, 1);
	os_tsk_create(Task_EnemyUpdate, 1);
	os_tsk_delete_self();
}

void init()
{
	LED_setup();
	initJoystick();
	initPotentiometer();
	initPushBtn();
	GLCD_Init();

	//Game Objs Settings
	//Hiro 's initial position
	Hiro.x = 1;
	Hiro.y = 1;
	prevPotentio= readPotentiometer();
}
int main( void ) 
{
	init();
	DEBUG_MSG("Dungeon Begins");
	os_sys_init(start_tasks);
}
