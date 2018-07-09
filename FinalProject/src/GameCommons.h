//
// Created by Jack Xu on 2018-07-09.
//

#ifndef SRC_GAMESETTINGS_H
#define SRC_GAMESETTINGS_H
//settings
#define BULLET_SPEED 10 //per tick
#define BULLET_MAX_QUANT 8
#define BULLET_DEFUALT_QUANT 5
#define BULLET_W 5
#define BULLET_H 5
#define POTENTIO_SENSITIVITY 200
#define ENEMIES_MAX_QUANT 4
#define ENEMY_MOVING_SPEED 1
#define ENEMY_UPDATE_ITV 50
#define HIRO_MAX_HEALTH 3
#define INPUT_UPDATE_PERIOD 20
#define HIRO_PROPERTY_WIN (1<<6)
#define HIRO_PROPERTY_INVINCIBLE (1<<4)
#define INVINCIBLE_DURATION 2000 //ms
#define TIME_TICK_SCALE 1000

#define REWARDS_MAX_QUANT 3
#define TRAPS_MAX_QUANT 1
#define TIME_TICK_TRAP_SCALE 100
#define TRAP_SPIKE_DURATION 10 //*0.1s

#define MAP_SCRN_W 16
#define MAP_SCRN_H 12


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

//encoding initial object positions
const uint8_t Temp_enemy_pos[8] = {1,8,7,3,8,1,13,10};
const uint16_t Temp_rewards_pos[9] = {7,3, TC_PORTAL,8,1, TC_PORTAL, 1,8, TC_POTION};
const uint8_t m_START_END[4] = {1,1,13,11};
const uint16_t MAP[MAP_SCRN_H] = {32767, 16385, 24053, 21781, 21845, 21829, 22397, 20485, 22397, 21825, 24029, 16385};

#endif //SRC_GAMESETTINGS_H
