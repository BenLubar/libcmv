#pragma once

#include <stdint.h>

/*
bits n=0..9: pixels in row 0, col n
bits n=10..19: pixels in row 1, col (n-10)
bits n=20..29: pixels in row 2, col (n-20)
bits n=30..31: unused

colors are 0=bg, 1=fg for most tiles

faces get an additional sprite that differentiates between fg2 and non-fg2
all fg2 pixels are marked as fg in the original sprite
*/
typedef uint32_t cmv_sprite_rows_t;

/* a sprite is 10 columns by 12 rows */
typedef uint32_t cmv_sprite_t[4];

/* sprites for each CP437 character */
extern const cmv_sprite_t curses_charmap[256];

/* used on CP437 characters 1 and 2 (empty face and filled face) */
extern const cmv_sprite_t curses_face;
/* empty sprite for convenience in rendering non-face characters */
extern const cmv_sprite_t curses_empty;

/*
ordered as alpha, red, green, blue when read as octets.
platform-dependent representation.
*/
typedef uint32_t color_argb_t;

/*
there are 16 colors in the palette, followed by dimmed versions.

0 black        8 dark grey
1 blue         9 light blue
2 green       10 light green
3 cyan        11 light cyan
4 red         12 light red
5 magenta     13 light magenta
6 brown       14 yellow
7 grey        15 white
*/
extern const color_argb_t palette[32];
