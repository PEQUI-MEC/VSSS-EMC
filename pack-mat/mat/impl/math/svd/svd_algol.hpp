/*
 * svd_algol.hpp
 *
 *  Created on: Oct 20, 2012
 *      Author: gustavo
 */

#ifndef IMPL_SVD_ALGOL_ALG_HPP
#define	IMPL_SVD_ALGOL_ALG_HPP

#include <malloc.h> /* for array allocation */
#include <math.h>    /* for 'fabs'           */

//#include "mat_primitives.hpp"

/*
 *  sources.c
 *
 *
 *  Created by Georgy Gimel'farb on 11/03/2011.
 *  Copyright 2011 __Dept of Computer Science, UoA__. All rights reserved
 *        (modification of svd.c and pgm-ppm i/o subroutines)
 */

/*  svd.c -- Singular Value Decomposition. Translated to 'C' from the
 *           original Algol code in "Handbook for Automatic Computation,
 *           vol. II, Linear Algebra", Springer-Verlag, by C. Bond
 *
 *                              (C) 2000, C. Bond. All rights reserved.
 *
 *  This is almost an exact translation from the original, except that
 *  an iteration counter is added to prevent stalls. This corresponds
 *  to similar changes in other translations.
 *
 *  Returns an error code = 0, if no errors and 'k' if a failure to
 *  converge at the 'kth' singular value.
 * --------------------------------------------------------------------------------
 *  Modified by Georgy Gimel'farb, 04/02/2010, to fix errors of the
 *       initial version, which failed for non-equal m and n (see //===gg comments)
 *---------------------------------------------------------------------------------
 *  Corrected typo "w" instead of the true "q" in the comments below
 *                                                by Sina Masoud-Ansari, 01/04/2011
 * --------------------------------------------------------------------------------
 *
 *     m - number of rows in input matrix a[m][n];      [!!! NOTE]: m should be >= n
 *     n - number of columns in input matrix a[m][n]
 *
 *     withu should be set to 1 if the matrix u[][] in the decomposition is desired,
 *           and to 0 otherwise.
 *     withv should be set to 1 if the matrix v[][] in thedecomposition is desired,
 *           and to 0 otherwise.
 *     eps   error threshold (e.g. 1.e-6)
 *     tol   tolerance threshold (e.g. 1.e-6)
 *	   a[m][n] - the rectangular input matrix mxn to be decomposed: a = u * d * v^T
 *	             a[i][j] -- the element of a with row i and column j
 *     u[m][n] - the output column-ortogonal matrix (n top eigenvectors of a * a^T)
 *               if withu has been set to 1; u[i][j] -- the element of u with row i
 *               and column j. Otherwise it is used as a temporary array. It may
 *      		 coincide with a. If an error exit is made, the columns of the matrix
 *               u corresponding to indices of correct singular values should be
 *               correct.
 *     q[n]    - the output non-negative singular values
 *               [the unordered diagonal elements of the diagonal matrix d]
 *               If an error exit is made, the singular values should be correct for
 *               indices retval+1, retval+2, ..., n.
 *     v[n][n] - the output column-orthogonal matrix (n eigenvectors of a^T * a)
 *	             if matv has been set to 1: v[i][j] -- the element of V with row i
 *               and column j. Otherwise it is not referenced. It may also coincide
 *               with the matrix a if u is not needed. If an error exit is made, the
 *      		 columns of V corresponding to indices of correct singular values
 *               should be correct.
 *     The output value retval is set to:
 *      	     zero for normal return,
 *               k if the k-th singular value has not been determined after 30
 *                    iterations, and
 * [!!!]         -1 if the number of rows m in the matrix a is smaller than the
 *                    number of its columns.
 *
 *      NOTE(!!!): m should be >= n
 *
 *     Output:
 *         a  is unaltered (unless overwritten by U or V).
 *         w  contains the n (non-negative) singular values of aA (the diagonal
 *            elements d[i][i] of d).  NOTE: they are unordered [!!!].
 ***/

/**
 * Fonte:
 * http://www.cs.auckland.ac.nz/courses/compsci369s1c/resources/
 * http://www.cs.auckland.ac.nz/courses/compsci369s1c/resources/for-assig1/sources.c
 *
 *
 * NOTA:
 *
 * fiz o teste para double e float e agora está funcionando.
 *
 * Para o caso de T == float, as variáveis:
 *
 * double c, f, g, h, s, x, y, z;
 * double * e;
 *
 * também deveriam ser float, no entanto o resultado não fica correto.
 * os autovalores menores são bem afetados enquando que os maiores não são
 * tão afetados.
 *
 * Por esse motivo as variáves mencionadas são declaradas como double
 * independente do tipo T.
 *
 */

/**
 * \brief Computes the Singular Value Decomposition (SVD) of a m-by-n matrix.
 *
 * This function realizes the Singular Value Decomposition (SVD) of a
 * m-by-n matrix, where m > n.
 *
 * [!!! NOTE]: m should be >= n
 *
 *
 * @param m number of rows in input matrix a[m][n]
 * @param n number of columns in input matrix a[m][n]
 *
 * @param withu	should be set to 1 if the matrix u[][] in the decomposition is desired, and to 0 otherwise.
 * @param withv	should be set to 1 if the matrix v[][] in thedecomposition is desired, and to 0 otherwise.
 * @param eps	error threshold (e.g. 1.e-6)
 * @param tol   tolerance threshold (e.g. 1.e-6)
 * @param A		A[m][n] - the rectangular input matrix mxn to be decomposed: a = u * d * v^T
 *	            A[i][j] - the element of a with row i and column j
 * @param U		U[m][n] - the output column-ortogonal matrix (n top eigenvectors of a * a^T)
 *              if withu has been set to 1; U[i][j] -- the element of u with row i
 *              and column j. Otherwise it is used as a temporary array. It may
 *      		coincide with a. If an error exit is made, the columns of the matrix
 *              u corresponding to indices of correct singular values should be
 *              correct.
 * @param Q		Q[n] - the output non-negative singular values
 *              [the unordered diagonal elements of the diagonal matrix d]
 *              If an error exit is made, the singular values should be correct for
 *              indices retval+1, retval+2, ..., n.
 * @param V		V[n][n] - the output column-orthogonal matrix (n eigenvectors of a^T * a)
 *	            if matv has been set to 1: V[i][j] -- the element of V with row i
 *              and column j. Otherwise it is not referenced. It may also coincide
 *              with the matrix a if u is not needed. If an error exit is made, the
 *				columns of V corresponding to indices of correct singular values
 *              should be correct.
 * @return		The output value retval is set to:
 *      	    zero for normal return,
 *              k if the k-th singular value has not been determined after 30 iterations, and
 * [!!!]        -1 if the number of rows m in the matrix a is smaller than the number of its columns.
 *
 * @return
 * <p>
 * 0 for normal execution.
 * <p>
 * k if the k-th singular value has not been determined after 30 iterations.
 * <p>
 * -1 if the number of rows m in the matrix a is smaller than the number of its columns.
 *
 */
template<typename T> int svd_algol(int m, int n, int withu, int withv, double eps, double tol, const T * A, T * Q, T * U, T * V) {
	int i, j, k, l, l1, iter, retval;
	double c, f, g, h, s, x, y, z;
	double * e;

	e = (double *) calloc(n, sizeof(double));
	retval = 0;

	// Copy 'a' to 'u'
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++)
			U[i * n + j] = A[i * n + j];
	}

	// Householder's reduction to bidiagonal form.
	g = x = 0.0;
	for (i = 0; i < n; i++) {
		e[i] = g;
		s = 0.0;
		l = i + 1;
		for (j = i; j < m; j++)
			s += (U[j * n + i] * U[j * n + i]);
		if (s < tol) g = 0.0;
		else {
			f = U[i * n + i];
			g = (f < 0) ? sqrt(s) : -sqrt(s);
			h = f * g - s;
			U[i * n + i] = f - g;
			for (j = l; j < n; j++) {
				s = 0.0;
				for (k = i; k < m; k++)
					s += (U[k * n + i] * U[k * n + j]);
				f = s / h;
				for (k = i; k < m; k++)
					U[k * n + j] += (f * U[k * n + i]);
			} // end j
		} // end s
		Q[i] = g;
		s = 0.0;
		for (j = l; j < n; j++)
			s += (U[i * n + j] * U[i * n + j]);
		if (s < tol) g = 0.0;
		else {
			f = U[i * n + i + 1];
			g = (f < 0) ? sqrt(s) : -sqrt(s);
			h = f * g - s;
			U[i * n + (i + 1)] = f - g;
			for (j = l; j < n; j++)
				e[j] = U[i * n + j] / h;
			for (j = l; j < m; j++) {
				s = 0.0;
				for (k = l; k < n; k++)
					s += (U[j * n + k] * U[i * n + k]);
				for (k = l; k < n; k++)
					U[j * n + k] += (s * e[k]);
			} // end j
		} // end s
		y = fabs(Q[i]) + fabs(e[i]);
		if (y > x) x = y;
	} // end i

	// accumulation of right-hand transformations
	if (withv) {
		for (i = n - 1; i >= 0; i--) {
			if (i < n - 1) { //============================gg
				if (g != 0.0) {
					h = U[i * n + (i + 1)] * g; //==============in other implementation: h[i][l]
					for (j = l; j < n; j++)
						V[j * n + i] = U[i * n + j] / h;
					for (j = l; j < n; j++) {
						s = 0.0;
						for (k = l; k < n; k++)
							s += (U[i * n + k] * V[k * n + j]);
						for (k = l; k < n; k++)
							V[k * n + j] += (s * V[k * n + i]);

					} // end j
				} // end g
				for (j = l; j < n; j++)
					V[i * n + j] = V[j * n + i] = 0.0;
			} // end if(i < n-1) ===========gg
			V[i * n + i] = 1.0;
			g = e[i];
			l = i;
		} // end i

	} // end withv, parens added for clarity

	if (withu) {
		for (i = n - 1; i >= 0; i--) {
			l = i + 1;
			g = Q[i];
			if (i < n - 1) //============================gg
			for (j = l; j < n; j++) // upper limit was 'n' (--> 'm' in original===gg)
				U[i * n + j] = 0.0;
			if (g != 0.0) {
				if (i != n - 1) { //============================gg
					h = U[i * n + i] * g;
					for (j = l; j < n; j++) { // upper limit was 'n' (--> 'm' in original===gg)
						s = 0.0;
						for (k = l; k < m; k++)
							s += (U[k * n + i] * U[k * n + j]);
						f = s / h;
						for (k = i; k < m; k++)
							U[k * n + j] += (f * U[k * n + i]);
					} // end j
				} // end if( i != n-1 )      =============================gg
				for (j = i; j < m; j++)
					U[j * n + i] /= g;
			} // end if (g)
			else {
				for (j = i; j < m; j++)
					U[j * n + i] = 0.0;
			}
			U[i * n + i] += 1.0;
		} // end i
	} // end withu, parens added for clarity

	// diagonalization of the bidiagonal form
	eps *= x;
	for (k = n - 1; k >= 0; k--) {
		iter = 0;

		test_f_splitting: for (l = k; l >= 0; l--) {
			if (fabs(e[l]) <= eps) goto test_f_convergence;
			if (fabs(Q[l - 1]) <= eps) goto cancellation;
		} // end l

		// cancellation of e[l] if l > 0
		cancellation: c = 0.0;
		s = 1.0;
		l1 = l - 1;
		for (i = l; i <= k; i++) {
			f = s * e[i];
			e[i] *= c;
			if (fabs(f) <= eps) goto test_f_convergence;
			g = Q[i];
			h = Q[i] = sqrt(f * f + g * g);
			c = g / h;
			s = -f / h;
			if (withu) {
				for (j = 0; j < m; j++) {
					y = U[j * n + l1];
					z = U[j * n + i];
					U[j * n + l1] = y * c + z * s;
					U[j * n + i] = -y * s + z * c;
				} // end j
			} // end withu, parens added for clarity
		} // end i
		test_f_convergence: z = Q[k];
		if (l == k) goto convergence;

		// shift from bottom 2x2 minor
		iter++;
		if (iter > 30) {
			retval = k;
			break;
		}
		x = Q[l];
		y = Q[k - 1];
		g = e[k - 1];
		h = e[k];
		f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2 * h * y);
		g = sqrt(f * f + 1.0);
		f = ((x - z) * (x + z) + h * (y / ((f < 0) ? (f - g) : (f + g)) - h)) / x;
		// next QR transformation
		c = s = 1.0;
		for (i = l + 1; i <= k; i++) {
			g = e[i];
			y = Q[i];
			h = s * g;
			g *= c;
			e[i - 1] = z = sqrt(f * f + h * h);
			c = f / z;
			s = h / z;
			f = x * c + g * s;
			g = -x * s + g * c;
			h = y * s;
			y *= c;
			if (withv) {
				for (j = 0; j < n; j++) {
					x = V[j * n + (i - 1)];
					z = V[j * n + i];
					V[j * n + (i - 1)] = x * c + z * s;
					V[j * n + i] = -x * s + z * c;
				} // end j
			} // end withv, parens added for clarity
			Q[i - 1] = z = sqrt(f * f + h * h);
			c = f / z;
			s = h / z;
			f = c * g + s * y;
			x = -s * g + c * y;
			if (withu) {
				for (j = 0; j < m; j++) {
					y = U[j * n + (i - 1)];
					z = U[j * n + i];
					U[j * n + (i - 1)] = y * c + z * s;
					U[j * n + i] = -y * s + z * c;
				} // end j
			} // end withu, parens added for clarity
		} // end i
		e[l] = 0.0;
		e[k] = f;
		Q[k] = x;
		goto test_f_splitting;
		convergence: if (z < 0.0) {
			// q[k] is made non-negative
			Q[k] = -z;
			if (withv) {
				for (j = 0; j < n; j++)
					V[j * n + k] = -V[j * n + k];
			} // end withv, parentheses added for clarity
		} // end z
	} // end k

	free(e);
	return retval;
}

#endif	/* IMPL_SVD_ALGOL_ALG_HPP */

