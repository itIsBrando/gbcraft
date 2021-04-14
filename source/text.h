#ifndef TEXT_H
#define TEXT_H

#include "defines.h"
#include "bg.h"


/**
 * @param bg Background that we will target
 * @param startTile tile number where the font is located
 */
void text_init(BG_REGULAR *bg, u16 startTile);


void text_write_tile(u16 tile, const u16 x, const u16 y);

/**
 * Prints a string
 * @param x tile x
 * @param y tile y
 */
void text_print(char *string, u16 x, u16 y);


/**
 * Prints an unsigned integer
 * @param x tile x
 * @param y tile y
 */
void text_uint(u16 num, u16 x, u16 y);


/**
 * Prints a signed integer
 * @param num signed word
 * @param x tile x
 * @param y tile y
 */
void text_int(s16 num, u16 x, u16 y);

/**
 * Reports an error
 */
void text_error(char *string);

#endif