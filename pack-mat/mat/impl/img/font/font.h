/* 
 * File:   font_6x11.h
 * Author: gustavo
 *
 * Created on August 24, 2010, 11:31 AM
 */

#ifndef FONT_H
#define	FONT_H

typedef struct _font {
		int char_height;
		int char_width;
		int char_start;
		int mask;
		unsigned char * fontdata;
} Font;

Font font_get_6x11(void);
Font font_get_sun12x22(void);

#define FONT_6x11   font_get_6x11()
#define FONT_8x16   font_get_8x16()
#define FONT_sun12x22   font_get_sun12x22()

#endif	/* FONT_H */
