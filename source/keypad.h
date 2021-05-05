#ifndef KEYPAD_H
#define KEYPAD_H

#include "defines.h"

typedef enum KEYPAD_BITS {
	KEY_A		=	(1<<0),	/*keypad A button */
	KEY_B		=	(1<<1),	/*keypad B button */
	KEY_SELECT	=	(1<<2),	/*keypad SELECT button */
	KEY_START	=	(1<<3),	/*keypad START button */
	KEY_RIGHT	=	(1<<4),	/*dpad RIGHT */
	KEY_LEFT	=	(1<<5),	/*dpad LEFT */
	KEY_UP		=	(1<<6),	/*dpad UP */
	KEY_DOWN	=	(1<<7),	/*dpad DOWN */
	KEY_R		=	(1<<8),	/*Right shoulder button */
	KEY_L		=	(1<<9),	/*Left shoulder button */

	KEY_IRQ_ENABLE	=	(1<<14),	/*Enable keypad interrupt */
	KEY_IRQ_OR		=	(0<<15),	/*interrupt logical OR mode */
	KEY_IRQ_AND		=	(1<<15),	/*interrupt logical AND mode */
	KEY_DPAD 		=	(KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT) /*!< mask all dpad buttons */
} KEYPAD_BITS;


#define REG_KEYPAD (*(vu16*)0x04000130)
#define REG_KEYCNT (*(vu16*)0x04000132)


void key_scan();


/**
 * Gets all the keys that are currently pressed
 */
uint key_pressed();


/**
 * Gets all the keys that are pressed but were not pressed for the last `key_scan()`
 */
uint key_pressed_no_repeat();


/**
 * Waits a certain number of frames before a keys can be recognized again
 * @param frames number of calls to `key_scan` before key presses will be recognized
 */
uint key_pressed_repeat_after(uint frames);

#endif