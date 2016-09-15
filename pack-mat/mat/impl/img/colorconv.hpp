/*
 * color_convert.hpp
 *
 *  Created on: Sep 14, 2012
 *      Author: gustavo
 */

#ifndef IMPL_COLORCONV_HPP
#define IMPL_COLORCONV_HPP

#include "../math/math_macros.h"

#define LIMIT(x) ((x)>0xffffff?0xff: ((x)<=0xffff?0:((x)>>16)))

//void __yuv420p_to_rgb( unsigned char *image, unsigned char *temp, int x, int y, int z );
//inline void __move_420_block( int yTL, int yTR, int yBL, int yBR, int u, int v, int rowPixels, unsigned char *rgb, int bits );

namespace impl {
	namespace img {

		template<typename Trgb, typename Tgray> void rgb_to_gray(int width, int height, const Trgb * rgb, Tgray * gray) {

			double px;
			long int npixels = width * height;
			int i;
			for (i = 0; i < npixels; ++i) {


				/* Matlab coeficients */
				px = 0.2989 * (*rgb);
				rgb++;
				px += 0.5870 * (*rgb);
				rgb++;
				px += 0.1140 * (*rgb);
				rgb++;

				/* Others coeficients */
				//px = 0.2125 * (*rgb++);
				//px += 0.7154 * (*rgb++);
				//px += 0.0721 * (*rgb++);
				px = imp__round( px );
				px = __clamp(px, 0.0, 255.0);

				*gray = (Tgray) px;
				gray++;
			}
		}

		template<typename Tgray, typename Trgb> void gray_to_rgb(int width, int height, const Tgray * gray, Trgb * rgb) {

			long int npixels = width * height;
			int i;
			for (i = 0; i < npixels; ++i) {

				*rgb = (Trgb) *gray;
				++rgb;

				*rgb = (Trgb) *gray;
				++rgb;

				*rgb = (Trgb) *gray;
				++rgb;

				++gray;
			}

		}

		/**
		 *  Description:
		 *      convert a rgb image to hsv image
		 */
		template<typename Trgb, typename Thsv> void rgb2hsv(Trgb * rgb, Thsv * hsv, int width, int height) {

			int row;
			int col;
			int idx;

			Thsv M, m;
			Thsv C;

			Thsv R, G, B;
			Thsv H, S, V;

			for (row = 0; row < height; row++) {
				for (col = 0; col < width; col++) {
					idx = (row * width + col) * 3;

					/* R, G e B devem estar no intervalo [0,1] */
					R = ((float) rgb[idx]) / 255.0;
					G = ((float) rgb[idx + 1]) / 255.0;
					B = ((float) rgb[idx + 2]) / 255.0;

					// calculando Chroma
					if (R >= G && R >= B) {
						M = R;
						if (G >= B) m = B;
						else m = G;
					}
					if (G >= R && G >= B) {
						M = G;
						if (R >= B) m = B;
						else m = R;
					}
					if (B >= R && B >= G) {
						M = B;
						if (R >= G) m = G;
						else m = R;
					}
					C = M - m; // chroma

					// definindo H
					if (C == 0) {
						H = 0;
					} else {
						if (M == R) {
							H = (int) ((G - B) / C) / 6.0;
						} else {
							if (M == G) {
								H = ((B - R) / C) + 2.0;
							} else {
								// M == B
								H = ((R - G) / C) + 4.0;
							}
						}
					}
					H = (H * 60.0) / 359.0;

					// definindo V
					V = M;

					// definindo S
					if (C == 0) S = 0;
					else S = C / V;

					hsv[idx] = H;
					hsv[idx + 1] = S;
					hsv[idx + 2] = V;
				}
			}
		}

		/**
		 *  Description:
		 *      convert a hsv image to rgb image
		 */
		template<typename Thsv, typename Trgb> void hsv2rgb(Thsv * hsv, Trgb * rgb, int width, int height) {

			int row;
			int col;
			int idx;

			float R, G, B;
			float H, S, V;
			float C, X, Hl, m;

			// <editor-fold defaultstate="collapsed" desc="converting each pixel">
			for (row = 0; row < height; row++) {
				for (col = 0; col < width; col++) {
					idx = (row * width + col) * 3;

					/* H deve estar no intervalo [0,360)
					 * S, V devem estar no intervalo [0,1] */
					H = (float) hsv[idx] * 359.0;
					S = (float) hsv[idx + 1];
					V = (float) hsv[idx + 2];

					C = V * S;
					Hl = H / 60;
					X = C * (1.0 - __abs( (int) (Hl / 2.0) - 1.0 ));

					if (H < 0) {

					} else {
						if (Hl < 1) {
							R = C;
							G = X;
							B = 0;
						} else {
							if (Hl < 2) {
								R = X;
								G = C;
								B = 0;
							} else {
								if (Hl < 3) {
									R = 0;
									G = C;
									B = X;
								} else {
									if (Hl < 4) {
										R = 0;
										G = X;
										B = C;
									} else {
										if (Hl < 5) {
											R = X;
											G = 0;
											B = C;
										} else {
											R = C;
											G = 0;
											B = X;
										}
									}
								}
							}
						}
					}
					m = V - C;
					rgb[idx] = (R + m) * 255;
					rgb[idx + 1] = (G + m) * 255;
					rgb[idx + 2] = (B + m) * 255;
				}
			} // </editor-fold>
		}

		/**
		 *  Description:
		 *      convert a hsv image to visible image.
		 *      each pixel is normalized to interval [0,255].
		 */
		template<typename T> void hsv2view(T * hsv, unsigned char * view, int width, int height) {

			int row;
			int col;
			int idx;

			// <editor-fold defaultstate="collapsed" desc="converting each pixel">
			for (row = 0; row < height; row++) {
				for (col = 0; col < width; col++) {
					idx = (row * width + col) * 3;

					/* H deve estar no intervalo [0,360]
					 * S, V devem estar no intervalo [0,1] */

					//view[idx] = hsv[idx] * 255 / 360; // H - > R
					view[idx] = hsv[idx] * 255; // H - > R
					view[idx + 1] = hsv[idx + 1] * 255; // S -> G
					view[idx + 2] = hsv[idx + 2] * 255; // V -> B
				}
			} // </editor-fold>
		}

	/**
	 *  Description:
	 *      convert a yub240p image to gray image
	 */
//		void yuv420p_to_gray( unsigned char *yuv, unsigned char *gray, int width, int height ) {
//			int w, h, idx;
//			for ( w = 0; w < width; w++ ) {
//				for ( h = 0; h < height; h++ ) {
//					idx = h * width + w;
//					gray[idx] = yuv[idx];
//				}
//			}
//		}
//
//		void yuv420p_to_rgb( unsigned char *yuv, unsigned char *rgb, int width, int height ) {
//			__yuv420p_to_rgb( yuv, rgb, width, height, 3 );
//		}
//
//		void __yuv420p_to_rgb( unsigned char *image, unsigned char *temp, int x, int y, int z ) {
//			const int numpix = x * y;
//			const int bytes = z; /* (z*8) >> 3; */
//			int i, j, y00, y01, y10, y11, u, v;
//			unsigned char *pY = image;
//			unsigned char *pU = pY + numpix;
//			unsigned char *pV = pU + numpix / 4;
//			unsigned char *image2 = temp;
//			//    if(FALSE) // FIXME: make TRUE to add debugging in here
//			//	g_print("%s\n", "yuv420p->rgb");
//			for ( j = 0; j <= y - 2; j += 2 ) {
//				for ( i = 0; i <= x - 2; i += 2 ) {
//					y00 = *pY;
//					y01 = *(pY + 1);
//					y10 = *(pY + x);
//					y11 = *(pY + x + 1);
//					u = (*pU++) - 128;
//					v = (*pV++) - 128;
//
//					__move_420_block( y00, y01, y10, y11, u, v, x, image2, z * 8 );
//
//					pY += 2;
//					image2 += 2 * bytes;
//				}
//				pY += x;
//				image2 += x * bytes;
//			}
//		}
//
//		static inline void __move_420_block( int yTL, int yTR, int yBL, int yBR, int u, int v, int rowPixels, unsigned char *rgb, int bits ) {
//			const int rvScale = 91881;
//			const int guScale = -22553;
//			const int gvScale = -46801;
//			const int buScale = 116129;
//			const int yScale = 65536;
//			int r, g, b;
//
//			g = guScale * u + gvScale * v;
//			if ( 1 ) {
//				r = buScale * u;
//				b = rvScale * v;
//			} else {
//				r = rvScale * v;
//				b = buScale * u;
//			}
//
//			yTL *= yScale;
//			yTR *= yScale;
//			yBL *= yScale;
//			yBR *= yScale;
//
//			if ( bits == 24 ) {
//				/* Write out top two pixels */
//				rgb[0] = LIMIT( b + yTL );
//				rgb[1] = LIMIT( g + yTL );
//				rgb[2] = LIMIT( r + yTL );
//
//				rgb[3] = LIMIT( b + yTR );
//				rgb[4] = LIMIT( g + yTR );
//				rgb[5] = LIMIT( r + yTR );
//
//				/* Skip down to next line to write out bottom two pixels */
//				rgb += 3 * rowPixels;
//				rgb[0] = LIMIT( b + yBL );
//				rgb[1] = LIMIT( g + yBL );
//				rgb[2] = LIMIT( r + yBL );
//
//				rgb[3] = LIMIT( b + yBR );
//				rgb[4] = LIMIT( g + yBR );
//				rgb[5] = LIMIT( r + yBR );
//			} else if ( bits == 16 ) {
//				/* Write out top two pixels */
//				rgb[0] = ((LIMIT( b + yTL ) >> 3) & 0x1F) | ((LIMIT( g + yTL ) << 3) & 0xE0);
//				rgb[1] = ((LIMIT( g + yTL ) >> 5) & 0x07) | (LIMIT( r + yTL ) & 0xF8);
//
//				rgb[2] = ((LIMIT( b + yTR ) >> 3) & 0x1F) | ((LIMIT( g + yTR ) << 3) & 0xE0);
//				rgb[3] = ((LIMIT( g + yTR ) >> 5) & 0x07) | (LIMIT( r + yTR ) & 0xF8);
//
//				/* Skip down to next line to write out bottom two pixels */
//				rgb += 2 * rowPixels;
//
//				rgb[0] = ((LIMIT( b + yBL ) >> 3) & 0x1F) | ((LIMIT( g + yBL ) << 3) & 0xE0);
//				rgb[1] = ((LIMIT( g + yBL ) >> 5) & 0x07) | (LIMIT( r + yBL ) & 0xF8);
//
//				rgb[2] = ((LIMIT( b + yBR ) >> 3) & 0x1F) | ((LIMIT( g + yBR ) << 3) & 0xE0);
//				rgb[3] = ((LIMIT( g + yBR ) >> 5) & 0x07) | (LIMIT( r + yBR ) & 0xF8);
//			}
//		}
	}

}

#endif /* IMPL_COLORCONV_HPP */
