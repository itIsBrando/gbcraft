#ifndef TEXT_H
#define TEXT_H

#include "defines.h"
#include "bg.h"


/**
 * @param bg Background that we will target
 * @param startTile tile number where the font is located
 */
void text_init(BG_REGULAR *bg, uint startTile);


/**
 * Prints a string
 * @param x tile x
 * @param y tile y
 */
void text_print(char *string, uint x, uint y);


/**
 * Prints an unsigned integer
 * @param x tile x
 * @param y tile y
 */
void text_uint(uint num, uint x, uint y);

/**
 * Reports an error
 */
void text_error(char *string);

#endif