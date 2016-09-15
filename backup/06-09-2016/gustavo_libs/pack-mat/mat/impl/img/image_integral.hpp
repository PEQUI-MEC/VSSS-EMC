/*
 * image_integral.hpp
 *
 *  Created on: Sep 23, 2012
 *      Author: gustavo
 */

#ifndef IMPL_IMAGE_INTEGRAL_HPP
#define IMPL_IMAGE_INTEGRAL_HPP

#include <math.h>

namespace impl {
	namespace img {
		/**
		 * \brief Calculates the integral image.
		 *
		 * Calculates the integral (Summed Table Area) of an image.<br>
		 * <br>
		 * This function calculates integral image as following:<br>
		 *
		 * \f[
		 * IntImg(X,Y) = \sum_{x < X, y < Y} Img(x,y)
		 * \f]
		 *
		 * @param Img input matrix
		 * @param IntImg the matrix that will be the integral image. If \c *IntImg is equal to NULL, \c *IntImg will be allocated in this function.
		 *
		 */
		template<typename T, typename Ti> void image_integral(int rows, int columns, T * img, int st_img, Ti * int_img, int st_int) {

			int r, c, _r, _ri, i, ii;
			double sum;

			// Computes the first row of the integral image
			sum = 0;
			for (i = 0; i < columns; i++) {
				sum = sum + img[i];
				int_img[i] = sum;
			}

			// For each row do:
			for (r = 1, _r = r * st_img, _ri = r * st_int; r < rows; r++, _r += st_img, _ri += st_int) {

				sum = 0;

				for (c = 0; c < columns; c++) {
					i = _r + c;
					ii = _ri + c;

					sum = sum + img[i];
					int_img[ii] = int_img[ii - st_int] + sum;

				}
			}
		}
		template<typename T, typename Ti> void image_integral2(int rows, int columns, T * img, int st_img, Ti * int2_img, int st_int2) {

			int r, c, _r, _ri, i, ii;
			double sum;

			// Computes the first row of the integral image
			sum = 0;
			for (i = 0; i < columns; i++) {

				sum = sum + (img[i] * img[i]);
				int2_img[i] = sum;

			}

			// For each row do:
			for (r = 1, _r = r * st_img, _ri = r * st_int2; r < rows; r++, _r += st_img, _ri += st_int2) {

				sum = 0;

				for (c = 0; c < columns; c++) {
					i = _r + c;
					ii = _ri + c;

					sum = sum + (img[i] * img[i]);
					int2_img[ii] = int2_img[ii - st_int2] + sum;

				}
			}
		}

		/**
		 * \brief Calculates the integral image.
		 *
		 * Calculates the integral (Summed Table Area) of an image.<br>
		 * <br>
		 * This function calculates integral image as following:<br>
		 *
		 * \f[
		 * IntImg(X,Y) = \sum_{x < X, y < Y} Img(x,y)
		 * \f]
		 *
		 * \f[
		 * SqIntImg(X,Y) = \sum_{x < X, y < Y} Img(x,y)^{2}
		 * \f]
		 *
		 * @param Img input matrix
		 * @param IntImg the matrix that will be the integral image. If \c *IntImg is equal to NULL, \c *IntImg will be allocated in this function.
		 * @param SqIntImg integral image of squared pixels. If \c *SqIntImg is equal to NULL, \c *SqIntImg will be allocated in this function.
		 *
		 */
		template<typename T, typename Ti, typename Ti2> void image_integral2(int rows, int columns, T * img, int st_img, Ti * int_img, int st_int, Ti2 * int2_img, int st_int2) {

			int r, c, _r, _ri, _ri2, i, ii, ii2;
			double sum, sqsum;

			sum = 0;
			sqsum = 0;

			for (i = 0; i < columns; i++) {

				sum = sum + img[i];
				int_img[i] = sum;

				sqsum = sqsum + (img[i] * img[i]);
				int2_img[i] = sqsum;

			}
			for (r = 1, _r = r * st_img, _ri = r * st_int, _ri2 = r * st_int2; r < rows; r++, _r += st_img, _ri += st_int, _ri2 += st_int2) {
				sum = 0;
				sqsum = 0;
				for (c = 0; c < columns; c++) {
					i = _r + c;
					ii = _ri + c;
					ii2 = _ri2 + c;

					sum = sum + img[i];
					int_img[ii] = int_img[ii - st_int] + sum;

					sqsum = sqsum + (img[i] * img[i]);
					int2_img[ii2] = int2_img[ii2 - st_int2] + sqsum;
				}
			}
		}

		template<typename T, typename Tm> void image_integral_to_area(  //
				int rows, int columns,  //
				T * imint, int sti,  //
				Tm * AREA, int sta,  //
				int ray) {

			// Deslocamento e numero de pixels em relacao ao centro da janela
			//	 _		_
			// 	|A		B
			//  |D		C
			//
			// area = C - D - B + A

			int shift = ray * sti;
			int shift_a = -(shift + sti) - (ray + 1);
			int shift_b = -(shift + sti) + ray;
			int shift_c = shift + ray;
			int shift_d = shift - (ray + 1);

			// top left border
			for (int r = 0, _ri = r * sti, _ra = r * sta; r <= ray; ++r, _ri += sti, _ra += sta) {
				int _ic = _ri + shift_c;
				for (int c = 0; c <= ray; ++c) {
					AREA[_ra + c] = imint[_ic + c];
				}
			}
			// top middle border
			for (int r = 0, _ri = r * sti, _ra = r * sta; r <= ray; ++r, _ri += sti, _ra += sta) {
				int _ic = _ri + shift_c;
				int _id = _ri + shift_d;
				for (int c = ray + 1; c <= columns - ray; ++c) {
					AREA[_ra + c] = imint[_ic + c] - imint[_id + c];
				}
			}
			// top right border
			for (int r = 0, _ri = r * sti, _ra = r * sta; r <= ray; ++r, _ri += sti, _ra += sta) {

				double AC = imint[_ri + shift + columns - 1];
				int _id = _ri + shift_d;

				for (int c = columns - ray; c < columns; ++c) {
					AREA[_ra + c] = AC - imint[_id + c];
				}
			}
			// left middle border
			for (int r = ray + 1, _ri = r * sti, _ra = r * sta; r < rows - ray; ++r, _ri += sti, _ra += sta) {

				int _ib = _ri + shift_b;
				int _ic = _ri + shift_c;

				for (int c = 0; c <= ray; ++c) {
					AREA[_ra + c] = imint[_ic + c] - imint[_ib + c];
				}
			}
			// image center
			for (int r = ray + 1, _ri = r * sti, _ra = r * sta; r < rows - ray; ++r, _ri += sti, _ra += sta) {

				int _ia = _ri + shift_a;
				int _ib = _ri + shift_b;
				int _ic = _ri + shift_c;
				int _id = _ri + shift_d;

				for (int c = ray + 1; c < columns - ray; ++c) {
					AREA[_ra + c] = imint[_ic + c] - imint[_id + c] - imint[_ib + c] + imint[_ia + c];
				}
			}
			// right middle border
			for (int r = ray + 1, _ri = r * sti, _ra = r * sta; r < rows - ray; ++r, _ri += sti, _ra += sta) {

				double AC = imint[_ri + shift + columns - 1];
				double AB = imint[_ri - (shift + sti) + columns - 1];

				int _ia = _ri + shift_a;
				int _id = _ri + shift_d;

				for (int c = columns - ray; c < columns; ++c) {
					AREA[_ra + c] = AC - imint[_id + c] - AB + imint[_ia + c];
				}
			}
			// lower left border
			int _ic = (rows - 1) * sti + ray;
			for (int r = rows - ray, _ri = r * sti, _ra = r * sta; r < rows; ++r, _ri += sti, _ra += sta) {

				int _ib = _ri + shift_b;

				for (int c = 0; c <= ray; ++c) {
					AREA[_ra + c] = imint[_ic + c] - imint[_ib + c];
				}
			}

			// lower middle border
			_ic = (rows - 1) * sti + ray;
			int _id = (rows - 1) * sti - ray - 1;
			for (int r = rows - ray, _ri = r * sti, _ra = r * sta; r < rows; ++r, _ri += sti, _ra += sta) {

				int _ia = _ri + shift_a;
				int _ib = _ri + shift_b;

				for (int c = ray + 1; c < columns - ray; ++c) {
					AREA[_ra + c] = imint[_ic + c] - imint[_id + c] - imint[_ib + c] + imint[_ia + c];
				}
			}
			// lower right border
			double AC = imint[(rows - 1) * sti + columns - 1];  // Área acumulada no ponto C
			_id = (rows - 1) * sti - ray - 1;
			for (int r = rows - ray, _ri = r * sti, _ra = r * sta; r < rows; ++r, _ri += sti, _ra += sta) {

				int _ia = _ri + shift_a;
				double AB = imint[_ri - (shift + sti) + columns - 1];

				for (int c = columns - ray; c < columns; ++c) {
					AREA[_ra + c] = AC - AB - imint[_id + c] + imint[_ia + c];

				}
			}
		}

		/**
		 * \brief Computes the mean of the region [ w w ] using integral image.
		 * w = ray*2 + 1
		 *
		 * @param rows
		 * @param columns
		 * @param imint
		 * @param sti
		 * @param mean
		 * @param stm
		 * @param ray
		 */
		template<typename T, typename Tm> void image_integral_to_mean(  //
				int rows, int columns,  //
				T * imint, int sti,  //
				Tm * mean, int stm,  //
				int ray) {

			int w = (ray << 1) + 1;
			double N = w * w;

			image_integral_to_area(rows, columns, imint, sti, mean, stm, ray);

			for (int r = 0, _r = r * stm; r < rows; ++r, _r += stm) {
				for (int c = 0; c < columns; ++c) {
					mean[_r + c] /= N;
				}

			}

		}

		/**
		 *
		 * @param rows
		 * @param columns
		 * @param imint
		 * @param sti
		 * @param imint2
		 * @param sti2
		 * @param mean
		 * @param stm
		 * @param std
		 * @param sts
		 * @param s
		 */
		template<typename Ti, typename Ti2, typename Tm, typename Ts> void image_integral_to_mean_and_std(  //
				int rows, int columns,  //
				Ti * imint, int sti,  //
				Ti2 * imint2, int sti2,  //
				Tm * mean, int stm,  //
				Ts * std, int sts,  //
				int ray) {

			// calcula direto a média usando a imagem integra.
			image_integral_to_mean(rows, columns, imint, sti, mean, stm, ray);

			// std = area^2 inicialmente.
			image_integral_to_area(rows, columns, imint2, sti, std, sts, ray);

			int w = (ray << 1) + 1;
			double N = w * w;

			for (int r = 0, _rs = r * sts, _rm = r * stm; r < rows; ++r, _rs += sts, _rm += stm) {
				for (int c = 0; c < columns; ++c) {

					double a2 = std[_rs + c];
					double u = mean[_rm + c];
					double var = (a2 - N * u * u) / (N - 1);

					if (var == 0) std[_rs + c] = 0;
					else std[_rs + c] = sqrt(var);
				}

			}

//			int r, c;
//			int _ri, ii;
//			int _ri2, ii2;
//			int _rm, im;
//			int _rs, is;
//
//			int shift_int = ray * sti;
//			int shift_int2 = ray * sti2;
//			int width = (ray << 1) + 1;
//			int N = width * width;
//			int rayplus1 = ray + 1;
//
//			_ri = ray * sti;
//			_ri2 = ray * sti2;
//			_rm = ray * stm;
//			_rs = ray * sts;
//			for (c = ray; c < columns - ray; ++c) {
//				ii = _ri + c;
//				ii2 = _ri2 + c;
//				im = _rm + c;
//				is = _rs + c;
//
//				double area = imint[ii + shift_int + ray] - imint[ii + shift_int - rayplus1];
//				double u = area / N;
//				mean[im] = u;
//
//				double area2 = imint2[ii2 + shift_int2 + ray] - imint2[ii2 + shift_int2 - rayplus1];
//				double var = (area2 - N * u * u) / (N - 1);
//				std[is] = sqrt(var);
//
//			}
//
//			for (r = ray; r < rows - ray; ++r) {
//				_ri = r * sti;
//				_ri2 = r * sti2;
//				_rm = r * stm;
//				_rs = r * sts;
//
//				ii = _ri + ray;
//				ii2 = _ri2 + ray;
//				im = _rm + ray;
//				is = _rs + ray;
//
//				double area = imint[ii + shift_int + ray] - imint[ii - shift_int + ray];
//				double u = area / N;
//				mean[im] = u;
//
//				double area2 = imint2[ii2 + shift_int2 + ray] - imint2[ii2 - shift_int2 + ray];
//				double var = (area2 - N * u * u) / (N - 1);
//				std[is] = sqrt(var);
//
//			}
//
//			for (r = ray + 1, _ri = r * sti, _ri2 = r * sti2, _rm = r * stm, _rs = r * sts;  //
//					r < rows - ray;  //
//					++r, _ri += sti, _ri2 += sti2, _rm += stm, _rs += sts) {
//
//				for (c = ray + 1; c < columns - ray; ++c) {
//
//					ii = _ri + c;
//					ii2 = _ri2 + c;
//					im = _rm + c;
//					is = _rs + c;
//
//					double A;
//					double B;
//					double C;
//					double D;
//
//					A = imint[ii - shift_int - sti - rayplus1];
//					B = imint[ii - shift_int - sti + ray];
//					C = imint[ii + shift_int - rayplus1];
//					D = imint[ii + shift_int + ray];
//
//					double area = D - C - B + A;
//					double u = area / N;
//					mean[im] = u;
//
//					A = imint2[ii2 - shift_int2 - sti2 - rayplus1];
//					B = imint2[ii2 - shift_int2 - sti2 + ray];
//					C = imint2[ii2 + shift_int2 - rayplus1];
//					D = imint2[ii2 + shift_int2 + ray];
//
//					double area2 = D - C - B + A;
//					double var = (area2 - N * u * u) / (N - 1);
//					std[is] = sqrt(var);
//
//				}
//			}

		}

	}
}
#endif /* IMPL_IMAGE_INTEGRAL_HPP */
