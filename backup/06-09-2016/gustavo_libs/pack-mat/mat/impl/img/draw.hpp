/*
 * draw.hpp
 *
 *  Created on: Sep 14, 2012
 *      Author: gustavo
 */

#ifndef IMPL_DRAW_HPP
#define IMPL_DRAW_HPP

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../util/common_macros.h"
#include "../math/math_macros.h"
#include "../util/mem.h"

#include "font/font.h"

#define draw_pixel draw_pixel_fg

namespace impl {
	namespace img {

		template<typename T> class draw {
			public:
				int __width;
				int __height;
				T * __data;
				int __channels;
				int __stride;

				T * __fg;
				float __alpha_fg;

				T * __bg;
				float __alpha_bg;

				/* = Constructors =========================================== */
			public:
				draw() {

					this->__width = 0;
					this->__height = 0;
					this->__data = 0;
					this->__channels = 0;
					this->__stride = 0;

					this->__fg = 0;
					this->__alpha_fg = 1;

					this->__bg = 0;
					this->__alpha_bg = 1;
				}
				draw( int width, int height, T * data, int channels ) {
					__init( width, height, data, channels, width );
				}
				draw( int width, int height, T * data, int channels, int stride ) {
					__init( width, height, data, channels, stride );
				}

				virtual ~draw() {
					__release();
				}

				void init( int width, int height, T * data, int channels ) {
					__init( width, height, data, channels, width );
				}
				void init( int width, int height, T * data, int channels, int stride ) {
					__init( width, height, data, channels, stride );
				}

			private:
				void __init( int width, int height, T * data, int channels, int stride ) {
					this->__width = width;
					this->__height = height;
					this->__data = data;
					this->__channels = channels;
					this->__stride = stride;

					this->__fg = new T[channels];
					for (int i = 0; i < channels; ++i)
						__fg[i] = 255;
					this->__alpha_fg = 1.0;

					this->__bg = new T[channels];
					for (int i = 0; i < channels; ++i)
						__bg[i] = 0;
					this->__alpha_bg = 1.0;
				}

			protected:
				void __release() {
					if (__fg) delete[] __fg;
					if (__bg) delete[] __bg;

					__fg = 0;
					__bg = 0;
				}

			public:
				/* = Foreground ============================================= */
				template<typename Tc> void set_foreground( Tc g, float alpha = 1.0 ) {

					if (__channels != 1) return;
					__fg[0] = (T) g;
					this->__alpha_fg = alpha;

				}
				template<typename Tc> void set_foreground( Tc r, Tc g, Tc b, float alpha = 1.0 ) {

					if (__channels != 3) return;
					__fg[0] = (T) r;
					__fg[1] = (T) g;
					__fg[2] = (T) b;
					this->__alpha_fg = alpha;

				}
				template<typename Tc> void set_foreground( Tc * color, float alpha = 1.0 ) {

					for (int i = 0; i < __channels; ++i)
						__fg[i] = (T) color[i];
					this->__alpha_fg = alpha;

				}

				void set_foreground_alpha( float alpha ) {
					this->__alpha_fg = alpha;
				}
				void set_background_alpha( float alpha ) {
					this->__alpha_bg = alpha;
				}

				/* = Background ============================================= */
				template<typename Tc> void set_background( Tc g, float alpha = 1.0 ) {

					if (__channels != 1) return;
					__bg[0] = (T) g;
					this->__alpha_bg = alpha;

				}
				template<typename Tc> void set_background( Tc r, Tc g, Tc b, float alpha = 1.0 ) {

					if (__channels != 3) return;
					__bg[0] = (T) r;
					__bg[1] = (T) g;
					__bg[2] = (T) b;
					this->__alpha_bg = alpha;

				}
				template<typename Tc> void set_background( Tc * color, float alpha = 1.0 ) {

					for (int i = 0; i < __channels; ++i)
						__bg[i] = (T) color[i];
					this->__alpha_bg = alpha;

				}

				/* = Drawing Methods ======================================== */
				void draw_px_fg( int x, int y ) {

					if (x < 0 || x >= __width) return;
					if (y < 0 || y >= __height) return;

					int idx = (x + y * __stride) * __channels;

					if (__alpha_fg == 1.0) {

						for (int i = 0; i < __channels; ++i) {
							__data[idx + i] = __fg[i];
						}

					} else {

						float a = __alpha_fg;
						float c = 1.0 - a;

						int i;
						for (i = 0; i < __channels; ++i) {
							__data[idx + i] = (a * __fg[i]) + (c * __data[idx + i]);
						}

					}
				}
				void draw_px_bg( int x, int y ) {

					if (x < 0 || x >= __width) return;
					if (y < 0 || y >= __height) return;

					int idx = (x + y * __stride) * __channels;

					if (__alpha_bg == 1.0) {

						for (int i = 0; i < __channels; ++i) {
							__data[idx + i] = __bg[i];
						}

					} else {

						float a = __alpha_bg;
						float c = 1.0 - a;

						int i;
						for (i = 0; i < __channels; ++i) {
							__data[idx + i] = (a * __bg[i]) + (c * __data[idx + i]);
						}

					}
				}

				/* = Drawing Methods ======================================== */
				void segment( int x0, int y0, int x1, int y1 ) {
					// = draw vertical line
					if (x0 == x1) {
						if (y0 > y1) __swap( y0, y1, int );
						for (; y0 <= y1; y0++)
							draw_px_fg( x0, y0 );
						return;
					}

					// = draw horizontal line
					if (y0 == y1) {
						if (x0 > x1) __swap( x0, x1, int );
						for (; x0 <= x1; x0++)
							draw_px_fg( x0, y0 );
						return;
					}

					// = draw non vertical and non horizontal line
					int dx = __abs(x1 - x0);
					int dy = -__abs(y1 - y0);

					int sx = x0 < x1 ? 1 : -1;
					int sy = y0 < y1 ? 1 : -1;

					int err = dx + dy;
					int e2; // error value e_xy

					for (;;) { // loop

						draw_px_fg( x0, y0 );

						if (x0 == x1 && y0 == y1) break;
						e2 = 2 * err;
						if (e2 >= dy) {
							err += dy;
							x0 += sx;
						} // e_xy+e_x > 0
						if (e2 <= dx) {
							err += dx;
							y0 += sy;
						} // e_xy+e_y < 0
					}
				}

				void line( double a, double b, double c ) {
					double x0, y0;
					double x1, y1;

					/**
					 * ax + by + c = 0
					 *
					 * for x=0 then
					 * 		by = -c
					 *		y = -c/b
					 *
					 * for x = width
					 *		ax = -c
					 *
					 * for y = 0 then
					 * 		ax = -c
					 * 		x = -c/a
					 */

					x0 = 0;
					y0 = -c / b;
					if (y0 < 0) {
						y0 = 0;
						x0 = -c / a;
						if (x0 < 0) return;
					} else if (y0 >= __height) {
						y0 = __height - 1;
						x0 = -(c + (b * y0)) / a;
						if (x0 < 0) return;
					}

					x1 = __width;
					y1 = (-c - x1 * a) / b;
					if (y1 < 0) {
						y1 = 0;
						x1 = -c / a;
						if (x1 < 0) return;
					} else if (y1 >= __height) {
						y1 = __height - 1;
						x1 = -(c + (b * y1)) / a;
						if (x1 < 0) return;
					}

					y0 = imp__round(y0);
					x1 = imp__round(x1);

					segment( (int) x0, (int) y0, (int) x1, (int) y1 );

				}

				void circle( int xm, int ym, int ray ) {
					int x = -ray, y = 0, err = 2 - 2 * ray;
					do {

						draw_px_fg( xm - x, ym + y ); /*   I. Quadrant */
						draw_px_fg( xm - y, ym - x ); /*  II. Quadrant */
						draw_px_fg( xm + x, ym - y ); /* III. Quadrant */
						draw_px_fg( xm + y, ym + x ); /*  IV. Quadrant */

						ray = err;
						if (ray > x) err += ++x * 2 + 1; /* e_xy+e_x > 0 */
						if (ray <= y) err += ++y * 2 + 1; /* e_xy+e_y < 0 */
					} while (x < 0);
				}
				void rectangle( int x0, int y0, int x1, int y1 ) {
					/* horizontal */
					segment( x0, y0, x1, y0 );
					segment( x0, y1, x1, y1 );
					/* vertical */
					segment( x0, y0, x0, y1 );
					segment( x1, y0, x1, y1 );
				}
				void rectangle_fill( int x0, int y0, int x1, int y1 ) {
					for (; y0 <= y1; y0++) {
						segment( x0, y0, x1, y0 );
					}
				}
				void square( int x, int y, int ray ) {
					int x1, x2, y1, y2;

					x1 = x - ray;
					x2 = x + ray;
					y1 = y - ray;
					y2 = y + ray;

					rectangle( x1, y1, x2, y2 );
				}
				void square_fill( int x, int y, int ray ) {
					int x1, x2, y1, y2;

					x1 = x - ray;
					x2 = x + ray;
					y1 = y - ray;
					y2 = y + ray;

					rectangle_fill( x1, y1, x2, y2 );
				}
				void cross( int x, int y, int ray ) {
					segment( x - ray, y, x + ray, y );
					segment( x, y - ray, x, y + ray );
				}
				void ellipse_rect( int x0, int y0, int x1, int y1 ) {
					int a = __abs(x1 - x0);
					int b = abs( y1 - y0 );
					int b1 = b & 1; /* values of diameter */
					long dx = 4 * (1 - a) * b * b;
					long dy = 4 * (b1 + 1) * a * a; /* error increment */
					long err = dx + dy + b1 * a * a, e2; /* error of 1.step */

					if (x0 > x1) {
						x0 = x1;
						x1 += a;
					} /* if called with swapped points */
					if (y0 > y1) y0 = y1; /* .. exchange them */
					y0 += (b + 1) / 2;
					y1 = y0 - b1; /* starting pixel */
					a *= 8 * a;
					b1 = 8 * b * b;

					do {
						draw_px_fg( x1, y0 ); /*   I. Quadrant */
						draw_px_fg( x0, y0 ); /*  II. Quadrant */
						draw_px_fg( x0, y1 ); /* III. Quadrant */
						draw_px_fg( x1, y1 ); /*  IV. Quadrant */
						e2 = 2 * err;
						if (e2 >= dx) {
							x0++;
							x1--;
							err += dx += b1;
						} /* x step */
						if (e2 <= dy) {
							y0++;
							y1--;
							err += dy += a;
						} /* y step */
					} while (x0 <= x1);

					while (y0 - y1 < b) { /* too early stop of flat ellipses a=1 */
						draw_px_fg( x0 - 1, y0 ); /* -> finish tip of ellipse */
						draw_px_fg( x1 + 1, y0++ );
						draw_px_fg( x0 - 1, y1 );
						draw_px_fg( x1 + 1, y1-- );
					}
				}
				void ellipse( int xc, int yc, int a, int b, float deg ) {
					float t;
					float inc;
					int x0, x1;
					int y0, y1;

					float cosrad, sinrad;
					float cost, sint;

					if (deg == 0) {
						int a2 = a >> 1;
						int b2 = b >> 1;

						int x0 = xc - a2;
						int x1 = xc + a2;
						int y0 = yc - b2;
						int y1 = yc + b2;

						ellipse_rect( x0, y0, x1, y1 );
						return;
					}

					deg = -(deg * M_PI) / 180;

					inc = 0.01;
					t = 0.0;
					cosrad = cos( deg );
					sinrad = sin( deg );

					cost = cos( t );
					sint = sin( t );

					x0 = xc + a * cost * cosrad - b * sint * sinrad;
					y0 = yc + a * cost * sinrad + b * sint * cosrad;

					t += inc;

					do {
						cost = cos( t );
						sint = sin( t );

						x1 = xc + a * cost * cosrad - b * sint * sinrad;
						y1 = yc + a * cost * sinrad + b * sint * cosrad;

						x0 = x1;
						y0 = y1;
						t += inc;

						segment( x0, y0, x1, y1 );

					} while (t <= (M_PI + M_PI));
				}

				void print_label( int col, int row, char fill_ground, const char * imp_image_label ) {
					int i, x, y, f, len;
					int w, h;

					Font __font_ = font_get_6x11();

					len = strlen( imp_image_label );

					for (y = 0; y < __font_.char_height; y++) {
						h = row + y;
						if (h >= __height || h < 0) continue;

						for (x = 0; x < len; x++) {

							f = __font_.fontdata[imp_image_label[x] * __font_.char_height + y];

							for (i = 0; i < __font_.char_width; i++) {

								w = col + (__font_.char_width * x) + i;

								if (w >= __width || w < 0) continue;

								if (f & (__font_.mask >> i)) {

									draw_px_fg( w, h );

								} else {
									if (fill_ground) {

										draw_px_bg( w, h );
									}

								}
							}

						}
					}
				}
				void text( int x, int y, char fill_ground, const char * imp_image_label, ... ) {
					char msg[256];
					va_list ap;

					va_start( ap, imp_image_label );
					vsprintf( msg, imp_image_label, ap );
					va_end( ap );

					//_draw_print_text(img, x, y, fillbg, msg);
					print_label( x, y, fill_ground, msg );
				}
		};

		template<typename T> class COLOR {
			public:
				T * px;
				int channels;
				float alpha;

				COLOR() {
					this->px = 0;
					this->channels = 0;
					this->alpha = 0;
				}

				COLOR( T g, float alpha = 1.0 ) {
					this->px = 0;
					this->channels = 0;
					this->alpha = 0;
					set( g, alpha );
				}
				COLOR( T r, T g, T b, float alpha = 1.0 ) {
					this->px = 0;
					this->channels = 0;
					this->alpha = 0;
					set( r, g, b, alpha );
				}
				COLOR( int ch, T * color, float alpha = 1.0 ) {
					this->px = 0;
					this->channels = 0;
					this->alpha = 0;
					set( ch, color, alpha );
				}

				void set( T g, float alpha = 1.0 ) {
					__alloc( 1 );
					this->channels = 1;
					this->px[0] = g;
					this->alpha = alpha;
				}
				void set( T r, T g, T b, float alpha = 1.0 ) {
					__alloc( 3 );
					this->channels = 3;
					this->px[0] = r;
					this->px[1] = g;
					this->px[2] = b;
					this->alpha = alpha;
				}
				void set( int ch, T * color, float alpha = 1.0 ) {
					__alloc( ch );

					this->channels = ch;

					int i;
					for (i = 0; i < ch; ++i) {
						this->px[i] = color[i];
					}
					this->alpha = alpha;
				}

				virtual ~COLOR() {
					delete[] px;
				}
			private:
				void __alloc( int channels ) {
					if (this->px) delete[] this->px;
					this->px = new T[channels];
					this->channels = channels;
				}
		};

		template<typename T> class draw_context {
			public:
				int width;
				int height;
				T * data;
				int channels;
				int stride;

				COLOR<T> fg;
				COLOR<T> bg;

				/* = Public Methods ========================================= */
			public:
				draw_context() {
					this->width = 0;
					this->height = 0;
					this->data = 0;
					this->channels = 0;
					this->stride = 0;
				}

				draw_context( int width, int height, T * data, int channels = 1 ) {
					__init( width, height, data, channels, width );
				}

				draw_context( int width, int height, T * data, int channels, int stride ) {
					__init( width, height, data, channels, stride );
				}

				void init( int width, int height, T * data, int channels = 1 ) {
					__init( width, height, data, channels, width );
				}

				void init( int width, int height, T * data, int channels, int stride ) {
					__init( width, height, data, channels, stride );
				}

			private:
				void __init( int width, int height, T * data, int channels, int stride ) {
					this->width = width;
					this->height = height;
					this->data = data;
					this->channels = channels;
					this->stride = stride;

					int i;
					T color[channels];
					for (i = 0; i < channels; ++i)
						color[i] = 255;

					this->fg.set( channels, color, 1.0 );

					for (i = 0; i < channels; ++i)
						color[i] = 0;

					this->bg.set( channels, color, 1.0 );

				}

			public:

				void set_foreground( T g, float alpha = 1.0 ) {
					fg.set( g, alpha );
				}
				void set_foreground( T r, T g, T b, float alpha = 1.0 ) {
					fg.set( r, g, b, alpha );
				}
				void set_foreground( T * color, float alpha = 1.0 ) {
					fg.set( this->channels, color, alpha );
				}

				void set_background( T g, float alpha = 1.0 ) {
					bg.set( g, alpha );
				}
				void set_background( T r, T g, T b, float alpha = 1.0 ) {
					bg.set( r, g, b, alpha );
				}
				void set_background( T * color, float alpha = 1.0 ) {
					bg.set( this->channels, color, alpha );
				}
		};

		template<typename T> void draw_pixel_fg( struct draw_context<T> * draw, int x, int y ) {

			COLOR<T>& color = draw->fg;

			if (x < 0 || x >= draw->width) return;
			if (y < 0 || y >= draw->height) return;

			int idx = (x + y * draw->stride) * draw->channels;

			if (color.alpha == 1.0) {

				int i;
				for (i = 0; i < draw->channels; ++i) {
					draw->data[idx + i] = color.px[i];
				}

			} else {

				float a = color.alpha;
				float c = 1.0 - a;

				int i;
				for (i = 0; i < draw->channels; ++i) {
					draw->data[idx + i] = (a * color.px[i]) + (c * draw->data[idx + i]);
				}

			}
		}

		template<typename T> void draw_pixel_bg( struct draw_context<T> * draw, int x, int y ) {

			int i;
			COLOR<T>& color = draw->bg;

			if (x < 0 || x >= draw->width) return;
			if (y < 0 || y >= draw->height) return;

			int idx = (x + y * draw->stride) * draw->channels;

			if (color.alpha == 1.0) {
				for (i = 0; i < draw->channels; ++i) {
					draw->data[idx + i] = color.px[i];
				}
			} else {

				float a = color.alpha;
				float c = 1 - a;

				for (i = 0; i < draw->channels; ++i) {
					draw->data[idx + i] = (a * color.px[i]) + (c * draw->data[idx + i]);
				}
			}
		}

		template<typename T> void draw_segment_bresenham( struct draw_context<T> * draw, int x0, int y0, int x1, int y1 ) {

			// = draw vertical line
			if (x0 == x1) {
				if (y0 > y1) __swap( y0, y1, int );
				for (; y0 <= y1; y0++)
					draw_pixel( draw, x0, y0 );
				return;
			}

			// = draw horizontal line
			if (y0 == y1) {
				if (x0 > x1) __swap( x0, x1, int );
				for (; x0 <= x1; x0++)
					draw_pixel( draw, x0, y0 );
				return;
			}

			// = draw non vertical and non horizontal line
			int dx = __abs(x1 - x0);
			int dy = -__abs(y1 - y0);

			int sx = x0 < x1 ? 1 : -1;
			int sy = y0 < y1 ? 1 : -1;

			int err = dx + dy;
			int e2; // error value e_xy

			for (;;) { // loop

				draw_pixel( draw, x0, y0 );

				if (x0 == x1 && y0 == y1) break;
				e2 = 2 * err;
				if (e2 >= dy) {
					err += dy;
					x0 += sx;
				} // e_xy+e_x > 0
				if (e2 <= dx) {
					err += dx;
					y0 += sy;
				} // e_xy+e_y < 0
			}
		}

		template<typename T> void draw_circle_bresenham( struct draw_context<T> * draw, int xm, int ym, int ray ) {
			int x = -ray, y = 0, err = 2 - 2 * ray; /* II. Quadrant */
			do {

				draw_pixel( draw, xm - x, ym + y ); /*   I. Quadrant */
				draw_pixel( draw, xm - y, ym - x ); /*  II. Quadrant */
				draw_pixel( draw, xm + x, ym - y ); /* III. Quadrant */
				draw_pixel( draw, xm + y, ym + x ); /*  IV. Quadrant */

				ray = err;
				if (ray > x) err += ++x * 2 + 1; /* e_xy+e_x > 0 */
				if (ray <= y) err += ++y * 2 + 1; /* e_xy+e_y < 0 */
			} while (x < 0);
		}

		template<typename T> void draw_rectangle( struct draw_context<T> * draw, int x0, int y0, int x1, int y1 ) {

			/* horizontal */
			draw_segment_bresenham( draw, x0, y0, x1, y0 );
			draw_segment_bresenham( draw, x0, y1, x1, y1 );
			/* vertical */
			draw_segment_bresenham( draw, x0, y0, x0, y1 );
			draw_segment_bresenham( draw, x1, y0, x1, y1 );
		}

		template<typename T> void draw_rectangle_fill( struct draw_context<T> * draw, int x0, int y0, int x1, int y1 ) {

			for (; y0 <= y1; y0++) {
				draw_segment_bresenham( draw, x0, y0, x1, y0 );
			}

		}

		template<typename T> void draw_square( struct draw_context<T> * draw, int x, int y, int ray ) {
			int x1, x2, y1, y2;

			x1 = x - ray;
			x2 = x + ray;
			y1 = y - ray;
			y2 = y + ray;

			draw_rectangle( draw, x1, y1, x2, y2 );

		}

		template<typename T> void draw_square_fill( struct draw_context<T> * draw, int x, int y, int ray ) {
			int x1, x2, y1, y2;

			x1 = x - ray;
			x2 = x + ray;
			y1 = y - ray;
			y2 = y + ray;

			draw_rectangle_fill( draw, x1, y1, x2, y2 );

		}

		template<typename T> void draw_cross( struct draw_context<T> * draw, int x, int y, int ray ) {

			draw_segment_bresenham( draw, x - ray, y, x + ray, y );
			draw_segment_bresenham( draw, x, y - ray, x, y + ray );

		}

		template<typename T> void draw_ellipse_rect( struct draw_context<T> * draw, int x0, int y0, int x1, int y1 ) {
			int a = __abs(x1 - x0);
			int b = abs( y1 - y0 );
			int b1 = b & 1; /* values of diameter */
			long dx = 4 * (1 - a) * b * b;
			long dy = 4 * (b1 + 1) * a * a; /* error increment */
			long err = dx + dy + b1 * a * a, e2; /* error of 1.step */

			if (x0 > x1) {
				x0 = x1;
				x1 += a;
			} /* if called with swapped points */
			if (y0 > y1) y0 = y1; /* .. exchange them */
			y0 += (b + 1) / 2;
			y1 = y0 - b1; /* starting pixel */
			a *= 8 * a;
			b1 = 8 * b * b;

			do {
				draw_pixel( draw, x1, y0 ); /*   I. Quadrant */
				draw_pixel( draw, x0, y0 ); /*  II. Quadrant */
				draw_pixel( draw, x0, y1 ); /* III. Quadrant */
				draw_pixel( draw, x1, y1 ); /*  IV. Quadrant */
				e2 = 2 * err;
				if (e2 >= dx) {
					x0++;
					x1--;
					err += dx += b1;
				} /* x step */
				if (e2 <= dy) {
					y0++;
					y1--;
					err += dy += a;
				} /* y step */
			} while (x0 <= x1);

			while (y0 - y1 < b) { /* too early stop of flat ellipses a=1 */
				draw_pixel( draw, x0 - 1, y0 ); /* -> finish tip of ellipse */
				draw_pixel( draw, x1 + 1, y0++ );
				draw_pixel( draw, x0 - 1, y1 );
				draw_pixel( draw, x1 + 1, y1-- );
			}
		}

		template<typename T> void draw_ellipse( struct draw_context<T> * draw, int xc, int yc, int a, int b, float deg ) {
			float t;
			float inc;
			int x0, x1;
			int y0, y1;

			float cosrad, sinrad;
			float cost, sint;

			if (deg == 0) {
				int a2 = a >> 1;
				int b2 = b >> 1;

				int x0 = xc - a2;
				int x1 = xc + a2;
				int y0 = yc - b2;
				int y1 = yc + b2;

				draw_ellipse_rect( draw, x0, y0, x1, y1 );
				return;
			}

			deg = -(deg * M_PI) / 180;

			inc = 0.01;
			t = 0.0;
			cosrad = cos( deg );
			sinrad = sin( deg );

			cost = cos( t );
			sint = sin( t );

			x0 = xc + a * cost * cosrad - b * sint * sinrad;
			y0 = yc + a * cost * sinrad + b * sint * cosrad;

			t += inc;

			do {
				cost = cos( t );
				sint = sin( t );

				x1 = xc + a * cost * cosrad - b * sint * sinrad;
				y1 = yc + a * cost * sinrad + b * sint * cosrad;

				x0 = x1;
				y0 = y1;
				t += inc;

				draw_segment_bresenham( draw, x0, y0, x1, y1 );

			} while (t <= (M_PI + M_PI));

		}

		template<typename T> void __print_text( struct draw_context<T> * draw, int col, int row, char fill_ground, const char * imp_image_label ) {

			int i, x, y, f, len;
			int w, h;

			int width;
			int height;

			Font __font_ = font_get_6x11();

			//Font __font_ = FONT_6x11;

			width = draw->width;
			height = draw->height;

			len = strlen( imp_image_label );

			for (y = 0; y < __font_.char_height; y++) {
				h = row + y;
				if (h >= height || h < 0) continue;

				for (x = 0; x < len; x++) {

					f = __font_.fontdata[imp_image_label[x] * __font_.char_height + y];

					for (i = 0; i < __font_.char_width; i++) {

						w = col + (__font_.char_width * x) + i;

						if (w >= width || w < 0) continue;

						if (f & (__font_.mask >> i)) {

							draw_pixel( draw, w, h );

						} else {
							if (fill_ground) {

								draw_pixel_bg( draw, w, h );
							}

						}
					}

				}
			}
		}

		template<typename T> void draw_text( struct draw_context<T> * draw, int x, int y, int fill_ground, const char * text, ... ) {

			char msg[256];
			va_list ap;

			va_start( ap, text );
			vsprintf( msg, text, ap );
			va_end( ap );

			//_draw_print_text(img, x, y, fillbg, msg);
			__print_text( draw, x, y, fill_ground, msg );

		}

	}
}
#endif /* IMPL_DRAW_HPP */
