//
// Created by Jack Xu on 2018-07-09.
//

uint8_t GetCurrentGameSTATUS()
{
    uint8_t state;
    os_mut_wait (&mutex_map_TEXEL, 0xffff);
    state = m_GAME_STATUS;
    os_mut_release (&mutex_map_TEXEL);
    return state;
}

uint8_t Get_Enemy_Property(uint8_t x, uint8_t y, uint16_t type)
{
    uint8_t i, cx, cy, cp;
    for(i=0;i<ENEMIES_MAX_QUANT;i++)
    {
        os_mut_wait (&mutex_Enemies, 0xffff);
        cx = Enemies[i].x;
        cy = Enemies[i].y;
        cp = Enemies[i].property;
        os_mut_release (&mutex_Enemies);
        if(cx == x && cy == y && (cp&((1<<7)+((type&(7<<2))<<1)))) //if found, return its property
        {
            return cp;//property
        }
    }
    return 0;//Empty
}

void Kill_Enemies_U16(uint16_t bx, uint16_t by)
{
    uint8_t i, cx, cy, cp;

    bx/=MAP_TEXEL_W;
    by/=MAP_TEXEL_H;
    for(i=0;i<ENEMIES_MAX_QUANT;i++)
    {
        os_mut_wait (&mutex_Enemies, 0xffff);
        cx = Enemies[i].x;
        cy = Enemies[i].y;
        cp = Enemies[i].property;
        os_mut_release (&mutex_Enemies);
        if(cx == bx && cy == by && (cp&(1<<7))) //if found, return its property
        {
            cp = (cp>>2)&3;//get current health
            cp = (cp==0)?0:(cp-1);
            if(cp <= 0)
            {
                os_mut_wait (&mutex_Enemies, 0xffff);
                Enemies[i].x = 0;
                Enemies[i].y = 0;
                cp = Enemies[i].property;//record the property for erasing the texture
                Enemies[i].property = 0;
                os_mut_release (&mutex_Enemies);
                Reset_TEXEL_CONTENT(cx, cy, ((cp>>4)&7)<<2);//erase on screen
            }else
            {
                os_mut_wait (&mutex_Enemies, 0xffff);
                Enemies[i].property &= (~(3<<2));//clear health
                Enemies[i].property += (cp<<2);//update health
                os_mut_release (&mutex_Enemies);
            }
        }
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