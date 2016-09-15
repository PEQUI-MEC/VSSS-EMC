/*
 * io.hpp
 *
 *  Created on: Sep 17, 2012
 *      Author: gustavo
 */

#ifndef IMPL_IO_HPP
#define IMPL_IO_HPP

#include <stdio.h>

#include "../util/mem.h"
#include "../util/msg.h"

#include "../math/mbop.hpp"

namespace impl {
	namespace img {

		template<typename T> void save_mat_as_gray( int width, int height, T * data, int stride, const char * fname ) {

			T * dt_norm = mem_allocn( width*height, T );
			math::m_copy( height, width, data, stride, dt_norm, width );

			math::m_normalize( height, width, dt_norm, width, (T) 0, (T) 255 );

			save_pgm( width, height, dt_norm, stride, fname );

			mem_free( dt_norm );
		}

		template<typename T> int read_pgm( int * width, int * height, T ** data, const char * fname ) {

			int idx;
			int size;
			unsigned char ch;

			int ret;

			FILE * file;

			file = fopen( fname, "rb" );

			if (file == 0) {
//				fclose(file);
				__error( "File binary open error." );
			}

			/* Ler cabecalho */
			if (fscanf( file, "P%c\n", &ch ) != 1 || ch != '5') {
				fclose( file );
				__error( "Invalid file." );
			}

			/* Desconsidera comentarios */
			ch = getc(file);
			while (ch == '#') {
				do {
					ch = getc(file);
				} while (ch != '\n'); /* Le ate o final da linha */
				ch = getc(file);
			}
			ungetc( ch, file );
			ret = fscanf( file, "%d %d\n%d\n", width, height, &idx );
			if (ret <= 0) __error( "Problems reading the file!" );
			//ret = fscanf(file, "%d\n", &idx);

			/* Salva os parametros da imagem */
			size = *width * *height;
			*data = mem_allocn( size, T );

			/* Leitura dos pixels */
			for (idx = 0; idx < size; idx++) {
				ret = fread( &ch, sizeof(char), 1, file );
				(*data)[idx] = (T) ch;
			}

			fclose( file );
			return 1;
		}

		template<typename T> int save_pgm( int width, int height, T * data, int stride, const char * fname ) {
			FILE* fp;

			if ((fp = fopen( fname, "w" )) == NULL) {
				__error( "Could not open file %s for writing.", fname );
			}

			fprintf( fp, "P5\n# CREATOR: ImpPack.\n%d %d\n255\n", width, height );

			for (int r = 0, _r = r * stride; r < height; ++r, _r += stride) {
				for (int c = 0; c < width; ++c) {
					int idx = _r + c;
					putc( data[ idx ], fp );
				}
			}

//			int h, w, idx;
//			for (h = 0; h < height; ++h) {
//				for (w = 0; w < width; ++w) {
//					idx = (h * stride + w);
//					putc( data[idx], fp );
//				}
//			}

//			for (index = 0; index < size; ++index) {
//				putc(data[index], fp);
//			}

			fflush( fp );
			fclose( fp );
			return 1;
		}

		template<typename T> int save_ppm( int width, int height, T * data, int stride, const char * fname ) {
			FILE* fp;

			if ((fp = fopen( fname, "w" )) == NULL) {
				__error( "Could not open file %s for writing.", fname );
			}

			fprintf( fp, "P6\n# CREATOR: ImpPack.\n%d %d\n255\n", width, height );

			int idx;
			int h, w;

			for (h = 0; h < height; ++h) {
				for (w = 0; w < width; ++w) {
					idx = (h * stride + w) * 3;
					putc( data[idx + 0], fp );
					putc( data[idx + 1], fp );
					putc( data[idx + 2], fp );
				}
			}

			/*for (index = 0; index < size; ++index) {

			 putc(data[0], fp);
			 putc(data[1], fp);
			 putc(data[2], fp);

			 data += 3;

			 //				putc(data[index * 3 + 0], fp);
			 //				putc(data[index * 3 + 1], fp);
			 //				putc(data[index * 3 + 2], fp);
			 }*/

			fflush( fp );
			fclose( fp );
			return 1;
		}

		template<typename T> int read_ppm( int * width, int * height, T ** data, const char * fname ) {

			int idx;
			int size;
			unsigned char ch;

			int ret;
			FILE * file;

			file = fopen( fname, "rb" );

			if (file == 0) {
//				fclose(file);
				__error( "File binary open error." );
			}

			/* Ler cabecalho */
			if (fscanf( file, "P%c\n", &ch ) != 1 || ch != '6') {
				fclose( file );
				__error( "Invalid file." );
			}

			/* Desconsidera comentarios */
			ch = getc(file);
			while (ch == '#') {
				do {
					ch = getc(file);
				} while (ch != '\n'); /* Le ate o final da linha */
				ch = getc(file);
			}
			ungetc( ch, file );
			ret = fscanf( file, "%d %d\n%d\n", width, height, &idx );
			if (ret <= 0) __error( "Problems reading the file!" );
			//fscanf(file, "%d\n", &idx);

			/* Salva os parametros da imagem */
			size = *width * *height * 3;
			*data = mem_allocn(size, T);

			if (data == NULL) {
				fclose( file );
				__error( "Unable to alloc memory to image." );
			}

			/* Leitura dos pixels */

			for (idx = 0; idx < size; idx++) {
				ret = fread( &ch, sizeof(char), 1, file );

				(*data)[idx] = (T) ch;
			}

			fclose( file );
			return 1;
		}

	}
}
#endif /* IMPL_IO_HPP */
