/*
 * File:   qr_householder.hpp
 * Author: gustavo
 *
 * Created on August 22, 2012, 12:46 AM
 */

#ifndef IMPL_QR_HOUSEHOLDER_HPP
#define	IMPL_QR_HOUSEHOLDER_HPP

#include "math_macros.h"

namespace impl {
	namespace math {

		/**
		 *
		 * This function computes the QR Factorization of a rectangular matrix A using
		 * householder transformations.
		 *
		 * A = QR
		 *
		 * All matrices are row major stored.
		 *
		 * @param m     - number of rows.
		 * @param n     - number of columns.
		 * @param A     - input matrix. After the execution, A is replaced by H + U.
		 *                H is the matrix with the householder reflector vectors and
		 *                U = R - diagonal(R).
         * @param sta   - stride of A.
		 * @param Rdiag - vector with the diagonal of R with size(R) = min(m,n).
		 *                If Rdiag is NULL Rdiag and incrd are ignored.
		 * @incrd       - the increment between the elements of Rdiag.
		 * @param Qt    - the matrix Q transposed. If Qt is NULL Qt is not computed and
		 *                stq is ignored.
		 * @param stq   - stride of Q.
		 * @param R     - the matrix R. If R is NULL, R is not formed and str is
		 *                ignored.
		 * @param str   - stride of R.
		 * @return 1 if A is a singular matrix and 0 otherwise.
		 *
		 *
		 *
		 * @Algorithm
		 *
		 * This code is based on algorithm described in the book:
		 *
		 * Numerical Recipes - The Art of Scientific Computing. William H. Press,
		 * Saul A. Teukolsky, William T. Vetterling, Brian P. Flannery - 3th edition
		 *
		 * @Step (01)
		 *
		 * The algorithm compute the upper triangular matrix R using A,
		 * store diagonal elements in a separated vector and saves the householder
		 * reflection vectors in the lower triangular of the matrix A.
		 *
		 * The matrix A has the form:
		 *
		 *	    ( h11     a12     a13 ... a1n )
		 *	    ( h21     h22     a23 ... a2n )
		 *      ( h31     h32     h33 ... a3n )
		 *	A =	( h41     h42     h43 ... a4n )
		 *	    (  :       :       :       :  )
		 *	    (  :       :       :       :  )
		 *	    ( hm1 ... hm2     hm3 ... hmn )
		 *
		 *      ( h11      0       0  ...  0  )   (  0      a12     a13 ... a1n )
		 *      ( h21     h22      0  ...  0  )   (  0       0      a23 ... a2n )
		 *      ( h31     h32     h33 ...  0  )   (  0       0       0  ... a3n )
		 *  A = ( h41     h42     h43 ...  0  ) + (  0       0       0  ... a4n )
		 *      (  :       :       :       :  )   (  :       :       :       :  )
		 *      (  :       :       :       :  )   (  :       :       :       :  )
		 *      ( hm1 ... hm2     hm3 ... hmn )   (  0  ...  0       0  ...  0  )
		 *
		 * diagonal(R) = ( r11 r22 r33 ... )
		 *
		 * @Step (02)
		 *
		 * Form Qt explicitly. The matrix Qt is computed explicitly using the
		 * householder reflection vectors stored in the lower part of A.
		 *
		 * @Step (03)
		 *
		 * Form R explicitly. Sets the diagonal(R) with the real elements, clear the
		 * lower triangular part of the matrix R and fills the upper triangular
		 * part of R with the upper triangular part of A.
		 *
		 *
		 */
		template<typename T> int qr_householder_full(int m, int n, T * A, int sta,	// Matrix A parameters
				T * Qt, int stq, T * R, int str) {

			int j, i, k, k_id, id;
			int mn_min = __min(m, n);
			int singular = 0;

			T norm, dot, sigma;
			T sum, tau;

			T scale;
			T rdiag[mn_min];
			T hh_dot[mn_min];

			/* = For each element in the diagonal of A do: ====================== */
			for (k = 0; k < mn_min; ++k) {

				/* = Find the max absolute value of column r[k:m][k] ============ */
				scale = 0.0;
				for (i = k, id = i * sta + i; i < m; ++i, id += sta)
					scale = __max(scale, __abs(A[id]));

				if (scale == 0.0) {
					/* = Matrix is singular ===================================== */
					rdiag[k] = 0.0;
					hh_dot[k] = 0.0;
					singular = 1;

				} else {

					/* = Compute the index of the pivot in A ==================== */
					k_id = k * sta + k;

					/* = Compute the norm of column r[k:m][k] =================== */

					// normalize the vector for safe calculation
					for (i = k, id = k_id; i < m; ++i, id += sta)
						A[id] /= scale;

					// accumulate the dot product
					dot = 0.0;
					for (i = k, id = k_id; i < m; ++i, id += sta)
						dot += A[id] * A[id];
					norm = sqrt(dot);

					/* = Select the correct signal to avoid cancellation ======== */
					if (A[k_id] < 0) sigma = -norm;
					else sigma = norm;

					/* = Compute the Householder reflector axis and save it in r[k:m][k] = */
					A[k_id] = A[k_id] + sigma;

					/* = Store the k diagonal of A ============================== */
					rdiag[k] = -sigma * scale;

					/* = Compute and store the half norm of householder reflector ==
					 * axis for future normalization
					 *
					 * w     - is the householder reflector axis and is stored
					 *         in A[k:m][k]
					 * dot   - is the dot product of original vector A[k:m][k]
					 *         normalized by scale
					 * norm  - is the norm of the original vector A[k:m][k]
					 *         normalized by scale
					 * sigma - is the norm with the correct signal to avoid
					 *         cancellation
					 *
					 * dot = sqr(a1) + sqr(a2) + ... sqr(am)
					 * norm = sqrt( dot )
					 *
					 *
					 * w has the form:
					 *
					 * [w1 w2 w3 ... wm]' = [ (a1 + sigma) a2 a3 ... am ]'
					 *
					 * w1 = a1 + sigma .: a1 = w1-sigma
					 *
					 *           dot = sqr(a1) + sqr(a2) + sqr(a3) + ... + sqr(am)
					 * dot - sqr(a1) = sqr(a2) + sqr(a3) + ... + sqr(am)
					 *
					 * <w,w> = sqr(w1) + sqr(w2) + sqr(w3) + ... + sqr(wm)
					 *       = sqr(w1) + sqr(a2) + sqr(a3) + ... + sqr(am)
					 *       = sqr(w1) + dot - sqr(a1)
					 *       = sqr(w1) + dot - sqr(w1-sigma)                        , r1 = w1-sigma
					 *       = sqr(w1) + dot - ( sqr(w1) - 2 w1 sigma + sqr(sigma) )
					 *       = sqr(w1) + dot - sqr(w1) + 2 w1 sigma - sqr(sigma)
					 *       = dot + 2 w1 sigma - dot                               , sqr(sigma) = sqr(norm) = dot
					 *      .:
					 * <w,w> = 2 w1 sigma
					 *
					 * w1 is stored in A[k][k]
					 *
					 * Store the half of inner product of householder reflector axis
					 * hh_dot[k] = w1 sigma = <w,w>/2
					 *
					 */
					hh_dot[k] = A[k_id] * sigma;

					/* = Update Ak = Qk' x Ak ======================================
					 *
					 * The Householder transformation matrix is:
					 * H = I - 2(ww'/w'w)
					 *
					 * The update of A is made by A2 = H1 x A1
					 *
					 * HA = ( I - 2(ww')/w'w) )A
					 *    =   A - 2(ww'A)/w'w
					 *    =   A - cww'A              , c = 2/w'w
					 *    =   A - cwu'               , u = w'A
					 *
					 * in this algorithm, c can be computed using hh_dot.
					 *
					 *  c = 2/ <w,w> = 2 / 2*hh_dot    , hh_dot = <w,w>/2
					 *    = 1/hh_dot
					 *
					 * the variable u represents a inner product of w and the
					 * columns of A
					 *
					 *  tau = uc = u/hh_hot
					 *
					 **/
					for (j = 1; j < n - k; ++j) {

						sum = 0.0;
						for (i = k, id = k_id; i < m; ++i, id += sta) {
							sum += A[id] * A[id + j];
						}

						tau = sum / hh_dot[k];

						for (i = k, id = k_id; i < m; ++i, id += sta) {
							A[id + j] -= tau * A[id];
						}
					}
				}
			}

			if (Qt) {
				/* = Form the matrix Qt from the householder vectors ======== */
				// construct the identity matrix
				for (i = 0; i < m; i++) {
					for (j = 0; j < m; j++)
						Qt[i * stq + j] = 0.0;
					Qt[i * stq + i] = 1.0;
				}

				// compute Qt with successive householder matrix multiplications
				for (k = 0; k < mn_min; ++k) {

					if (hh_dot[k] != 0.0) { // ignore

						for (j = 0; j < m; j++) {
							sum = 0.0;
							for (i = k; i < m; i++)
								sum += A[i * sta + k] * Qt[i * stq + j];
							sum /= hh_dot[k];
							for (i = k; i < m; i++)
								Qt[i * stq + j] -= sum * A[i * sta + k];
						}
					}
				}
			}

			if (R) {
				/* = Form matrix R explicity ======================================== */
				for (i = 0; i < mn_min; i++) {

					R[i * str + i] = rdiag[i];

					// Fill the lower triangular part with zeros
					for (j = i + 1; j < m; j++)
						R[j * str + i] = 0.0;

					//m_print(m, n, R, n);

					// Fill the upper triangular part with A
					for (j = i + 1; j < n; j++)
						R[i * str + j] = A[i * sta + j];

					//m_print(m, n, R, n);
				}
			}

			return singular;

		}

		template<typename T> int qr_householder_vectors( //
				int m, int n, T * A, int sta,		// Matrix A parameters
				T * Rdiag, int incrd,				// Diagonal of matrix R
				T * HH_dot, int inchhdot			// Half dot product of Householder vectors. Used to compute matrix Q explicitly.
				) {

			int j, i, k, k_id, id;
			int mn_min = __min(m, n);
			int singular = 0;

			T norm, dot, sigma;
			T sum, tau;

			T scale;
			T rdiag[mn_min];
			T hh_dot[mn_min];

			for (k = 0; k < mn_min; ++k) {

				scale = 0.0;
				for (i = k, id = i * sta + i; i < m; ++i, id += sta)
					scale = __max(scale, __abs(A[id]));

				if (scale == 0.0) {
					rdiag[k] = 0.0;
					hh_dot[k] = 0.0;
					singular = 1;

				} else {

					k_id = k * sta + k;

					for (i = k, id = k_id; i < m; ++i, id += sta)
						A[id] /= scale;

					dot = 0.0;
					for (i = k, id = k_id; i < m; ++i, id += sta)
						dot += A[id] * A[id];
					norm = sqrt(dot);

					if (A[k_id] < 0) sigma = -norm;
					else sigma = norm;

					A[k_id] = A[k_id] + sigma;

					rdiag[k] = -sigma * scale;

					hh_dot[k] = A[k_id] * sigma;

					for (j = 1; j < n - k; ++j) {

						sum = 0.0;
						for (i = k, id = k_id; i < m; ++i, id += sta) {
							sum += A[id] * A[id + j];
						}

						tau = sum / hh_dot[k];

						for (i = k, id = k_id; i < m; ++i, id += sta) {
							A[id + j] -= tau * A[id];
						}
					}
				}
			}

			if (Rdiag) {
				for (i = 0, id = 0; i < mn_min; ++i, id += incrd)
					Rdiag[id] = rdiag[i];
			}

			if (HH_dot) {
				for (i = 0, id = 0; i < mn_min; ++i, id += inchhdot)
					HH_dot[id] = hh_dot[i];
			}

			return singular;

		}

		template<typename T> void qr_householder_compute_R(int m, int n, T * A, int sta, T * R, int str, T * rdiag) {

			int i, j;
			int mn_min;

			mn_min = __min(m, n);

			/* = Form matrix R explicity ======================================== */
			for (i = 0; i < mn_min; i++) {

				R[i * str + i] = rdiag[i];

				// Fill the lower triangular part with zeros
				for (j = i + 1; j < m; j++)
					R[j * str + i] = 0.0;

				//m_print(m, n, R, n);

				// Fill the upper triangular part with A
				for (j = i + 1; j < n; j++)
					R[i * str + j] = A[i * sta + j];

				//m_print(m, n, R, n);
			}
		}

		template<typename T> void qr_householder_compute_Qt(int m, int n, T * A, int sta, T * Qt, int stq, T * hh_dot) {

			int i, j, k;
			int mn_min;
			T sum;

			mn_min = __min(m, n);

			/* = Form the matrix Qt from the householder vectors ======== */
			// construct the identity matrix
			for (i = 0; i < m; i++) {
				for (j = 0; j < m; j++)
					Qt[i * stq + j] = 0.0;
				Qt[i * stq + i] = 1.0;
			}

			// compute Qt with successive householder matrix multiplications
			for (k = 0; k < mn_min; ++k) {

				if (hh_dot[k] != 0.0) { // ignore

					for (j = 0; j < m; j++) {
						sum = 0.0;
						for (i = k; i < m; i++)
							sum += A[i * sta + k] * Qt[i * stq + j];
						sum /= hh_dot[k];
						for (i = k; i < m; i++)
							Qt[i * stq + j] -= sum * A[i * sta + k];
					}
				}
			}

		}

		template<typename T> void qr_householder_compute_Q(int m, int n, T * A, int sta, T * Qt, int stq, T * hh_dot) {

			int i, j, k;
			int mn_min;
			T sum;

			mn_min = __min(m, n);

			/* = Form the matrix Qt from the householder vectors ======== */
			// construct the identity matrix
			for (i = 0; i < m; i++) {
				for (j = 0; j < m; j++)
					Qt[i * stq + j] = 0.0;
				Qt[i * stq + i] = 1.0;
			}

			// compute Qt with successive householder matrix multiplications
			for (k = 0; k < mn_min; ++k) {

				if (hh_dot[k] != 0.0) { // ignore

					for (j = 0; j < m; j++) {
						sum = 0.0;
						for (i = k; i < m; i++)
							sum += A[i * sta + k] * Qt[i * stq + j];
						sum /= hh_dot[k];
						for (i = k; i < m; i++)
							Qt[j * stq + i] -= sum * A[i * sta + k];
					}
				}
			}

		}

		template<typename T> void qr_solve(int m, int n, T * R, int str, T * Qt, int stq, int mb, int nb, T * B, int stb, int mx, int nx, T * X, int stx) {
            // Needs to be implemented.
		}

	}
}

#endif	/* IMPL_QR_HOUSEHOLDER_HPP */

