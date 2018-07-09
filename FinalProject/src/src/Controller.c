#include <lpc17xx.h>
#include "Controller.h"
#include "stdio.h"

#define IO1_BITS_LED_3 0x0000000B<<28
#define IO2_BITS_LED_5 0x0000007C
#define IO2_BITS_PUSHBTN_1 1<<10

#define IO1_BITS_JOYS_4DIR 15<<23
#define IO1_BITS_JOYS_1PRESS 1<<20

// Global Constants
volatile uint16_t g_prevVal_Potentiometer = 0; //Tracking readings, prevent repeatly printing same values
volatile uint8_t g_prevVal_JoyStick = JS_IDLE; //Update screen if any input

/***************
***  JS ***
****************/
void initJoystick()
{
	LPC_GPIO1->FIODIR &= ~(IO1_BITS_JOYS_1PRESS|IO1_BITS_JOYS_4DIR); //masking joysticks as input
}

unsigned int readJoystick()
{
	unsigned int input = LPC_GPIO1->FIOPIN;
	//printf("in: %d", ((input&IO1_BITS_JOYS_4DIR)>>22));
	unsigned int result = JS_IDLE;
	if(!(input & (1<<23)))
	{
		result |= JS_UP;
	}
	else if (!(input & (1<<25)))
	{
		result |= JS_DOWN;
	}
	else if (!(input & (1<<26)))
	{
		result |= JS_LEFT;
	}
	else if (!(input & (1<<24)))
	{
		result |= JS_RIGHT;
	}
	
	if (!(input & (1<<20)))
	{
		result |= JS_PRESSED;
	}
	return result;
}

void logJoystick(unsigned int joyCommand)
{
	if(g_prevVal_JoyStick == joyCommand)
		return;
	g_prevVal_JoyStick = joyCommand;
	if(joyCommand != JS_IDLE)
	{
		printf("[JOYSTICK]\n");
		switch ((joyCommand) & (~JS_PRESSED))//Check direction
		{
			case JS_UP:
				printf("[DIR] UP\n");
				break;
			case JS_DOWN:
				printf("[DIR] DOWN\n");
				break;
			case JS_LEFT:
				printf("[DIR] LEFT\n");
				break;
			case JS_RIGHT:
				printf("[DIR] RIGHT\n");
				break;
			default:
				printf("[DIR] NULL\n");
				break;
		}
		
		if(joyCommand & JS_PRESSED)
			printf("[BTN] PRESSED\n");
		else
			printf("[BTN] NOT PRESSED\n");
	}else
	{
		printf("[JOYSTICK] IDLE\n");
	}
}

/***************
***  Potentiometer ***
****************/
void initPotentiometer()
{
	LPC_PINCON->PINSEL1 &= ~(0x03<<18);
	LPC_PINCON->PINSEL1 |= (0x01<<18);
	LPC_SC->PCONP |= (1<<12);
	LPC_ADC->ADCR = (1 << 2) |     // select AD0.2 pin
									(4 << 8) |     // ADC clock is 25MHz/5
									(1 << 21);     // enable ADC
}

unsigned int readPotentiometer()
{
	uint32_t ADCR12BIT = 0;
	LPC_ADC->ADCR |= (1<<24); //start conversion
	while(!((LPC_ADC->ADGDR) & (1<<31))); // wait for conversion to be done
	ADCR12BIT = (LPC_ADC->ADGDR>>4); //bit shift
	ADCR12BIT &= 0x0FFF; //bit masking 12 bit
	return  ADCR12BIT;
}

void logPotetiometer()
{
		uint32_t curVal = readPotentiometer();
		if(g_prevVal_Potentiometer != curVal)
		{
			g_prevVal_Potentiometer = curVal;
			printf("[Potentiometer] %d\n", curVal);
		}
}

/***************
***  pushbutton ***
****************/
void initPushBtn()
{
	LPC_GPIO2->FIODIR &= ~(IO2_BITS_PUSHBTN_1); //masking push button as input
}

uint8_t readPushBtn()
{
	unsigned int input = LPC_GPIO2->FIOPIN;
	return !(input & IO2_BITS_PUSHBTN_1);
}


