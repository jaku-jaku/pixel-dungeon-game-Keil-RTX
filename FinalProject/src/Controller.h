enum JOYSTICK_STATUS {JS_LEFT = 1, JS_RIGHT = 1<<1, JS_UP = 1<<2, JS_DOWN = 1<<3, JS_PRESSED = 1<<4, JS_IDLE = 0};

//JS
void initJoystick(void);
unsigned int readJoystick(void);
void logJoystick(unsigned int joyCommand);

//Potentiameter
void initPotentiometer(void);
unsigned int readPotentiometer(void);
void logPotetiometer(void);

//PB
void initPushBtn(void);
uint8_t readPushBtn(void);
