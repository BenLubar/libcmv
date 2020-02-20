#define COMPILING_LIBCMV

#include <cmv.h>
#include "sprite.h"

/* draw 3 rows of a tile (one quarter of a full tile) */
static void draw3(uint32_t *buf, size_t offset, size_t stride,
		color_argb_t fg1, color_argb_t fg2, color_argb_t bg,
		cmv_sprite_rows_t map1, cmv_sprite_rows_t map2)
{
	size_t x, y;
	uint32_t *ptr;

	for (y = 0; y < 3; y++)
	{
		ptr = &buf[offset];
		for (x = 0; x < 10; x++) {
			*ptr++ = (map1 & (1 << x)) ?
				((map2 & (1 << x)) ? fg2 : fg1) : bg;
		}
		offset += stride;
		map1 >>= 10;
		map2 >>= 10;
	}
}

void cmv_draw_tile(uint32_t *buf, size_t offset, size_t stride,
		cmv_glyph_t ch, cmv_attr_t attr)
{
	uint8_t fg = (attr.bright << 3) | attr.fg;
	uint32_t fg1 = palette[fg];
	uint32_t fg2 = palette[fg | 16];
	uint32_t bg = palette[attr.bg];
	const uint32_t *c1 = &curses_charmap[ch][0];
	const uint32_t *c2 = ch == 1 || ch == 2 ?
		&curses_face[0] : &curses_empty[0];
	size_t stride3 = stride * 3;

	draw3(buf, offset, stride, fg1, fg2, bg, *c1++, *c2++);
	offset += stride3;
	draw3(buf, offset, stride, fg1, fg2, bg, *c1++, *c2++);
	offset += stride3;
	draw3(buf, offset, stride, fg1, fg2, bg, *c1++, *c2++);
	offset += stride3;
	draw3(buf, offset, stride, fg1, fg2, bg, *c1++, *c2++);
}
