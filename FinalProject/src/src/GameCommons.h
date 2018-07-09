#include <LPC17xx.h>
#include <RTL.h>
#include "Controller.h"
#include "GLCD.h"
#include <stdbool.h>

#define DEBUG_MSG(x) printf("%s", x)

#ifndef SRC_GAMECOMMONS_H
#define SRC_GAMECOMMONS_H

/*******************/
/* Macros */
/*******************/
//settings
#define BULLET_SPEED            10 //per tick
#define BULLET_MAX_QUANT        8
#define BULLET_DEFUALT_QUANT    5
#define BULLET_W                5
#define BULLET_H                5
#define POTENTIO_SENSITIVITY    200
#define ENEMIES_MAX_QUANT       4
#define ENEMY_MOVING_SPEED      1
#define ENEMY_UPDATE_ITV        50
#define HIRO_MAX_HEALTH         3
#define INPUT_UPDATE_PERIOD     20
#define HIRO_PROPERTY_WIN       (1<<6)
#define HIRO_PROPERTY_INVINCIBLE    (1<<4)
#define INVINCIBLE_DURATION     2000 //ms
#define TIME_TICK_SCALE         1000

#define REWARDS_MAX_QUANT       3
#define TRAPS_MAX_QUANT         2
#define TIME_TICK_TRAP_SCALE    100
#define TRAP_SPIKE_DURATION     20 //*0.1s

#define MAP_SCRN_W              16
#define MAP_SCRN_H              12
#define MAP_VARIATION           5

#define MAP_TEXEL_SIZE          20
#define MAP_TEXEL_H             MAP_TEXEL_SIZE
#define MAP_TEXEL_W             MAP_TEXEL_SIZE
#define MAP_TEXEL_AREA          MAP_TEXEL_W*MAP_TEXEL_H

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
    TC_PORTAL 		=1<<7,
    TC_BULLET_CLIP	=1<<8,

    TC_TRAP_NEEDLE =1<<9,
    TC_TRAP_NEEDLE_STATUS = 1<<10,
    TC_TRAP_FIRE_LASER_EXIST =1<<11,
    TC_TRAP_FIRE_LASER =1<<12, //1 fire, 0 laser
    TC_TRAP_FIRE_LASER_STATUS = 1<<13,//1 on, 1 off

    //detailed types:
            TC_TRAP_NEEDLE_OFF =1<<9,
    TC_TRAP_NEEDLE_ON  =3<<9,
    TC_TRAP_FIRE_OFF = 		(1<<10) + (1<<11) + (0<<12) + 1,//110 + wall
    TC_TRAP_FIRE_ON  = 		(1<<10) + (1<<11) + (1<<12) + 1,//111 + wall
    TC_TRAP_FIRE_ON_PATH = 	(1<<10) + (1<<11) + (1<<12) + 0,//111 + path
    TC_TRAP_LASER_OFF = 	(1<<10) + (0<<11) + (0<<12) + 1,//100 + wall
    TC_TRAP_LASER_ON  = 	(1<<10) + (0<<11) + (1<<12) + 1,//101 + wall
    TC_TRAP_LASER_ON_PATH = (1<<10) + (0<<11) + (1<<12) + 0,//101 + path

    //BIT 14, 15:
    // if exist fire/laser trap: orientation of laser
    // if its player or zombie....: BLINK_cnter (2 times) blink at odd number
            TC_14_15_SPECIAL = 3<<14,

    TC_UNKNOWN = 65535 //1111 1111 1111 1111
} TEXEL_CONTENT_t;

/*******************/
/* Structs & Enums */
/*******************/
typedef struct OBJ_U8{
    uint8_t x;
    uint8_t y;
    //Enemy:: 2BIT 0-1:ORIENTATION; (*May Vary: bit 2&3 Health; 4`5`6:Enemy Type[TEXT_CONTENT]); 7:Valid/Invalid
    //Hiro:: 2BIT 0-1:ORIENTATION; bit 2&3 Health; 4:Invincible-Mode 6:AT_END_POSITION(WIN) ; 7:Valid/Invalid
    uint8_t property;// 0b000000000 ||DIRECTION: ..00:x++ ..01:y++ ..10:x-- ..11:y-- | 1.... Valid, 0... Invalid
} ObjectU8_t;

typedef struct OBJ_U16{
    uint16_t x;
    uint16_t y;
    uint8_t property;// 0b000000000 ||DIRECTION: ..00:x++ ..01:y++ ..10:x-- ..11:y-- | 1.... Valid, 0... Invalid
} ObjectU16_t;

enum {GM_MENU, GM_INIT, GM_RUNNING, GM_PAUSE, GM_END_S, GM_END_F};

/********************/
/* Global Constants */
/********************/
//encoding initial object positions
static const uint16_t Temp_rewards_pos[MAP_VARIATION][9] = 
{{7,3, TC_PORTAL,8,1, TC_PORTAL, 1,8, TC_POTION},
{7,3, TC_PORTAL,10,11, TC_PORTAL, 9,5, TC_POTION},
{10,11, TC_PORTAL,8,1, TC_PORTAL, 11,9, TC_POTION},
{7,3, TC_PORTAL,4,7, TC_PORTAL, 11,7, TC_POTION},
{7,3, TC_PORTAL,8,1, TC_PORTAL, 1,8, TC_POTION}};
static const uint8_t m_START_END[MAP_VARIATION][4] = 
{{1,1,11,9},
{1,5,9,9},
{13,3,3,9},
{6,11,4,3},
{1,11,11,9}};
//encoding map informations
static const uint16_t MAP[MAP_VARIATION][MAP_SCRN_H] = 
{
{32767, 16385, 24053, 21781, 21845, 21829, 22397, 20485, 22397, 21825, 24029, 16385},
{32767, 16385, 24061, 21761, 22013, 21509, 21981, 20817, 22389, 21509, 22525, 16385},
{32767, 16385, 22397, 21829, 32093, 16705, 32093, 17489, 21981, 21765, 24061, 16385},
{32767, 16641, 23933, 20549, 24053, 16401, 24029, 21829, 21885, 21569, 22493, 16385},
{32767, 16385, 24061, 21765, 21853, 20817, 22359, 17493, 23901, 20801, 24543, 16385}
};
/*******************/
/* Global Variables*/
/*******************/
static uint8_t      m_GAME_STATUS               = GM_MENU;
static uint8_t 			m_GAME_INDEX								= 0;
static uint16_t     map_TEXEL[MAP_TEXEL_AREA]   = {0};
static ObjectU8_t   Hiro;
static ObjectU16_t  Bullets[BULLET_MAX_QUANT];//NOTE: bullet in world coord
static ObjectU8_t   Enemies[ENEMIES_MAX_QUANT];
static ObjectU8_t   Portals[REWARDS_MAX_QUANT];
static ObjectU8_t   Traps[TRAPS_MAX_QUANT];
static uint8_t      Bullet_quant                = BULLET_DEFUALT_QUANT;

static uint16_t     prevPotentio                = 0;
static uint8_t      curJoyCMD                   = JS_IDLE;
static uint32_t     m_Hiro_LastDMGTime          = 0;
static uint32_t     m_GAME_startTime            = 0;
static uint16_t     m_GAME_ACCTime              = 0;
static uint8_t      screenClear                 = 0;
static uint8_t      Portal_InUse                = 0;
static uint16_t     prev_time_needle            = 0;

/*******************/
/*  Mutexes */
/*******************/
static OS_MUT mutex_Hiro;
static OS_MUT mutex_Enemies;
static OS_MUT mutex_map_TEXEL;
static OS_MUT mutex_bullets;
static OS_MUT mutex_m_START_END;
static OS_MUT mutex_input;
static OS_MUT mutex_GMstatus;
static OS_MUT mutex_Portals;
static OS_MUT mutex_Traps;

#endif //SRC_GAMECOMMONS_H
