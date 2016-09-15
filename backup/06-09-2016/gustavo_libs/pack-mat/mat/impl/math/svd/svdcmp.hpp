/*
 * svdcmp.hpp
 *
 *  Created on: Oct 20, 2012
 *      Author: gustavo
 */

#ifndef IMPL_SVDCMP_ALG_HPP
#define IMPL_SVDCMP_ALG_HPP

#include <math.h>
#include <malloc.h>

#include "../math_macros.h"

#define SIGN( a, b ) ( (b) >= 0.0 ? fabs(a) : -fabs(a) )

static double pythag(double a, double b) {

	double absa, absb;

	absa = fabs(a);
	absb = fabs(b);
	if (absa > absb) {
		return (absa * sqrt(1.0 + absb * absb / (absa * absa)));
	} else {
		return ((absb == 0.0) ? (0.0) : (absb * sqrt(1.0 + absa * absa / (absb * absb))));
	}
}

/**
 * Fonte:
 * Desconhecida!
 *
 *
 * CODIGOS AINDA PRECISAM SEREM TESTADOS!
 */

template<typename T> int svdcmp(int rows, int columns, int withv, int withu, T *a, T *w, T *v) {

	int flag, i, its, j, jj, k, l, nm, status;
	T c, f, h, s, x, y, z;
	T anorm = 0.0, g = 0.0, scale = 0.0;
	T *rv1;

	if (rows < columns) return (0);

	l = nm = 0;

	rv1 = (T *) malloc(columns * sizeof(T));

	// Householder's reduction to bidiagonal form.
	for (i = 0; i < columns; i++) {
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i < rows) {
			for (k = i; k < rows; k++)
				scale += fabs(a[k * columns + i]);
			if (scale) {
				for (k = i; k < rows; k++) {
					a[k * columns + i] /= scale;
					s += a[k * columns + i] * a[k * columns + i];
				}
				f = a[i * columns + i];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i * columns + i] = f - g;
				if (i != columns - 1) {
					for (j = l; j < columns; j++) {
						for (s = 0.0, k = i; k < rows; k++)
							s += a[k * columns + i] * a[k * columns + j];
						f = s / h;
						for (k = i; k < rows; k++)
							a[k * columns + j] += f * a[k * columns + i];
					}
				}
				for (k = i; k < rows; k++)
					a[k * columns + i] *= scale;
			}
		}
		w[i] = scale * g;
		g = s = scale = 0.0;
		if ((i < rows) && (i != (columns - 1))) {
			for (k = l; k < columns; k++)
				scale += fabs(a[i * columns + k]);
			if (scale) {
				for (k = l; k < columns; k++) {
					a[i * columns + k] /= scale;
					s += a[i * columns + k] * a[i * columns + k];
				}
				f = a[i * columns + l];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i * columns + l] = f - g;
				for (k = l; k < columns; k++)
					rv1[k] = a[i * columns + k] / h;
				if (i != rows - 1) {
					for (j = l; j < rows; j++) {
						for (s = 0.0, k = l; k < columns; k++)
							s += a[j * columns + k] * a[i * columns + k];
						for (k = l; k < columns; k++)
							a[j * columns + k] += s * rv1[k];
					}
				}
				for (k = l; k < columns; k++)
					a[i * columns + k] *= scale;
			}
		}
		anorm = __max( anorm, ( fabs(w[i]) + fabs(rv1[i]) ) );
	}

	// accumulation of right-hand transformations
	if (withv) {
		for (i = columns - 1; i >= 0; i--) {
			if (i < columns - 1) {
				if (g) {
					/* isn't l == n to start?? */
					for (j = l; j < columns; j++)
						v[j * columns + i] = (a[i * columns + j] / a[i * columns + l]) / g;
					for (j = l; j < columns; j++) {
						for (s = 0.0, k = l; k < columns; k++)
							s += a[i * columns + k] * v[k * columns + j];
						for (k = l; k < columns; k++)
							v[k * columns + j] += s * v[k * columns + i];
					}
				}
				for (j = l; j < columns; j++)
					v[i * columns + j] = v[j * columns + i] = 0.0;
			}
			v[i * columns + i] = 1.0;
			g = rv1[i];
			l = i;
		}
	}

	if (withu) {
		for (i = columns - 1; i >= 0; i--) {
			l = i + 1;
			g = w[i];
			if (i < columns - 1) for (j = l; j < columns; j++)
				a[i * columns + j] = 0.0;
			if (g) {
				g = 1.0 / g;
				if (i != columns - 1) {
					for (j = l; j < columns; j++) {
						for (s = 0.0, k = l; k < rows; k++)
							s += a[k * columns + i] * a[k * columns + j];
						f = (s / a[i * columns + i]) * g;
						for (k = i; k < rows; k++)
							a[k * columns + j] += f * a[k * columns + i];
					}
				}
				for (j = i; j < rows; j++)
					a[j * columns + i] *= g;
			} else {
				for (j = i; j < rows; j++)
					a[j * columns + i] = 0.0;
			}
			++a[i * columns + i];
		}
	}

	// diagonalization of the bidiagonal form
	status = 1;
	for (k = columns - 1; k >= 0; k--) {
		for (its = 0; its < 30; its++) {
			flag = 1;
			for (l = k; l >= 0; l--) {
				nm = l - 1;
				if (fabs(rv1[l]) + anorm == anorm) {
					flag = 0;
					break;
				}
				if (fabs(w[nm]) + anorm == anorm) break;
			}
			if (flag) {
				c = 0.0;
				s = 1.0;
				for (i = l; i < k; i++) {
					f = s * rv1[i];
					if (fabs(f) + anorm != anorm) {
						g = w[i];
						h = pythag(f, g);
						w[i] = h;
						h = 1.0 / h;
						c = g * h;
						s = (-f * h);
						for (j = 0; j < rows; j++) {
							y = a[j * columns + nm];
							z = a[j * columns + i];
							a[j * columns + nm] = y * c + z * s;
							a[j * columns + i] = z * c - y * s;
						}
					}
				}
			}
			z = w[k];
			if (l == k) {
				if (z < 0.0) {
					w[k] = -z;
					for (j = 0; j < columns; j++)
						v[j * columns + k] = (-v[j * columns + k]);
				}
				break;
			}
			if (its == 29) status = 0;
			x = w[l];
			nm = k - 1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
			g = pythag(f, 1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g, f)))-h)) / x;
			c = s = 1.0;
			for (j = l; j < nm; j++) {
				i = j + 1;
				g = rv1[i];
				y = w[i];
				h = s * g;
				g = c * g;
				z = pythag(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y = y * c;
				for (jj = 0; jj < columns; jj++) {
					x = v[jj * columns + j];
					z = v[jj * columns + i];
					v[jj * columns + j] = x * c + z * s;
					v[jj * columns + i] = z * c - x * s;
				}
				z = pythag(f, h);
				w[j] = z;
				if (z) {
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = (c * g) + (s * y);
				x = (c * y) - (s * g);
				for (jj = 0; jj < rows; jj++) {
					y = a[jj * columns + j];
					z = a[jj * columns + i];
					a[jj * columns + j] = y * c + z * s;
					a[jj * columns + i] = z * c - y * s;
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}
	free(rv1);
	return (1);
}

#undef SIGN
#undef PYTHAG

#endif /* IMPL_SVDCMP_ALG_HPP */
