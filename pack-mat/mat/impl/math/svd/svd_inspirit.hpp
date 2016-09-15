/*
 * svd_inspirit.hpp
 *
 *  Created on: Oct 20, 2012
 *      Author: gustavo
 */

#ifndef IMPL_SVD_INSPIRIT_ALG_HPP
#define IMPL_SVD_INSPIRIT_ALG_HPP

/**
 * Fonte:
 * http://code.google.com/p/in-spirit/
 * http://code.google.com/p/in-spirit/source/browse/trunk/projects/ASSURF/clib/svdcmp.c
 *
 *
 * CODIGOS AINDA PRECISAM SEREM TESTADOS!
 */

static const double TINY_NEAR_ZERO = 1.0E-12;

inline int IMIN(register int a, register int b) {
	return (((a) < (b)) ? (a) : (b));
}

inline int IMAX(register int a, register int b) {
	return (((a) < (b)) ? (b) : (a));
}

inline double FMAX(register double a, register double b) {
	return (((a) < (b)) ? (b) : (a));
}

inline double fast_sqrt(const double x) {
	if (x < 1e-8) return 0.0;

	/* 64-bit version */
	union {
			double x;
			long long i;
	} u;

	double xhalf = (double) 0.5 * x;

	/* convert floating point value in RAW integer */
	u.x = x;

	/* gives initial guess y0 */
	u.i = 0x5fe6ec85e7de30daLL - (u.i >> 1);

	/* two Newton steps */
	u.x = u.x * ((double) 1.5 - xhalf * u.x * u.x);
	u.x = u.x * ((double) 1.5 - xhalf * u.x * u.x);

	return x * u.x;
}

void MultiplyMat(const double *m1, const double *m2, register double *res, const int M1, const int N1, const int M2, const int N2);
int PseudoInverse(register double *inv, const double *matx, const int M, const int N);
int invert3x3(const double *src, double *inv);
int mossSVD(register double *U, register double *W, register double *V, const double *matx, const int M, const int N);

static double at, bt, ct;
#define PYTHAG(a,b) ((at=fabs(a)) > (bt=fabs(b)) ? \
    (ct=bt/at,at*fast_sqrt((double)(1.0)+ct*ct)) : (bt ? (ct=at/bt,bt*fast_sqrt((double)(1.0)+ct*ct)): (double)(0.0)))

#define SIGN(a,b) ((b) >= (double)(0.0) ? fabs(a) : -fabs(a))

int _mossNRsvdcmp(register double *a, const int m, const int n, double *w, register double *v) {
	int flag, i, its, j, jj, k, l, nm;
	double anorm, c, f, g, h, s, scale, x, y, z, rv11[n + 1];

	const int nc = n + 1;

	register double *rv1 = rv11;
	g = scale = anorm = 0.0;
	for (i = 1; i <= n; i++) {
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i <= m) {
			for (k = i; k <= m; k++)
				scale += fabs(a[k * nc + i]);
			if (scale) {
				for (k = i; k <= m; k++) {
					a[k * nc + i] /= scale;
					s += a[k * nc + i] * a[k * nc + i];
				}
				f = a[i * nc + i];
				g = -SIGN(fast_sqrt(s), f);
				h = f * g - s;
				a[i * nc + i] = f - g;
				for (j = l; j <= n; j++) {
					for (s = 0.0, k = i; k <= m; k++)
						s += a[k * nc + i] * a[k * nc + j];
					f = s / h;
					for (k = i; k <= m; k++)
						a[k * nc + j] += f * a[k * nc + i];
				}
				for (k = i; k <= m; k++)
					a[k * nc + i] *= scale;
			}
		}
		w[i] = scale * g;
		g = s = scale = 0.0;
		if (i <= m && i != n) {
			for (k = l; k <= n; k++)
				scale += fabs(a[i * nc + k]);
			if (scale) {
				for (k = l; k <= n; k++) {
					a[i * nc + k] /= scale;
					s += a[i * nc + k] * a[i * nc + k];
				}
				f = a[i * nc + l];
				g = -SIGN(fast_sqrt(s),f);
				h = f * g - s;
				a[i * nc + l] = f - g;
				for (k = l; k <= n; k++)
					rv1[k] = a[i * nc + k] / h;
				for (j = l; j <= m; j++) {
					for (s = 0.0, k = l; k <= n; k++)
						s += a[j * nc + k] * a[i * nc + k];
					for (k = l; k <= n; k++)
						a[j * nc + k] += s * rv1[k];
				}
				for (k = l; k <= n; k++)
					a[i * nc + k] *= scale;
			}
		}
		anorm = FMAX(anorm, (fabs(w[i]) + fabs(rv1[i])));
	}

	for (i = n; i >= 1; i--) {
		if (i < n) {
			if (g) {
				for (j = l; j <= n; j++)
					v[j * nc + i] = (a[i * nc + j] / a[i * nc + l]) / g;
				for (j = l; j <= n; j++) {
					for (s = 0.0, k = l; k <= n; k++)
						s += a[i * nc + k] * v[k * nc + j];
					for (k = l; k <= n; k++)
						v[k * nc + j] += s * v[k * nc + i];
				}
			}
			for (j = l; j <= n; j++)
				v[i * nc + j] = v[j * nc + i] = 0.0;
		}
		v[i * nc + i] = (double) 1.0;
		g = rv1[i];
		l = i;
	}

	for (i = IMIN(m, n); i >= 1; i--) {
		l = i + 1;
		g = w[i];
		for (j = l; j <= n; j++)
			a[i * nc + j] = 0.0;
		if (g) {
			g = (double) 1.0 / g;
			for (j = l; j <= n; j++) {
				for (s = 0.0, k = l; k <= m; k++)
					s += a[k * nc + i] * a[k * nc + j];
				f = (s / a[i * nc + i]) * g;
				for (k = i; k <= m; k++)
					a[k * nc + j] += f * a[k * nc + i];
			}
			for (j = i; j <= m; j++)
				a[j * nc + i] *= g;
		} else for (j = i; j <= m; j++)
			a[j * nc + i] = 0.0;
		++a[i * nc + i];
	}

	for (k = n; k >= 1; k--) {
		for (its = 1; its <= 30; its++) {
			flag = 1;
			for (l = k; l >= 1; l--) {
				nm = l - 1;
				if ((double) (fabs(rv1[l]) + anorm) == anorm) {
					flag = 0;
					break;
				}
				if ((double) (fabs(w[nm]) + anorm) == anorm) break;
			}
			if (flag) {
				c = (double) 0.0;
				s = (double) 1.0;
				for (i = l; i <= k; i++) {
					f = s * rv1[i];
					rv1[i] = c * rv1[i];
					if ((double) (fabs(f) + anorm) == anorm) break;
					g = w[i];
					h = PYTHAG(f,g);
					w[i] = h;
					h = (double) 1.0 / h;
					c = g * h;
					s = -f * h;
					for (j = 1; j <= m; j++) {
						y = a[j * nc + nm];
						z = a[j * nc + i];
						a[j * nc + nm] = y * c + z * s;
						a[j * nc + i] = z * c - y * s;
					}
				}
			}
			z = w[k];
			if (l == k) {
				if (z < 0.0) {
					w[k] = -z;
					for (j = 1; j <= n; j++)
						v[j * nc + k] = -v[j * nc + k];
				}
				break;
			}
			if (its == 30) {
				//no convergence in 30 svdcmp iterations
				return 1;
			}
			x = w[l];
			nm = k - 1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / ((double) 2.0 * h * y);
			g = PYTHAG(f,(double)1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g,f)))-h)) / x;
			c = s = (double) 1.0;
			for (j = l; j <= nm; j++) {
				i = j + 1;
				g = rv1[i];
				y = w[i];
				h = s * g;
				g = c * g;
				z = PYTHAG(f,h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y *= c;
				for (jj = 1; jj <= n; jj++) {
					x = v[jj * nc + j];
					z = v[jj * nc + i];
					v[jj * nc + j] = x * c + z * s;
					v[jj * nc + i] = z * c - x * s;
				}
				z = PYTHAG(f,h);
				w[j] = z;
				if (z) {
					z = (double) 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = c * g + s * y;
				x = c * y - s * g;
				for (jj = 1; jj <= m; jj++) {
					y = a[jj * nc + j];
					z = a[jj * nc + i];
					a[jj * nc + j] = y * c + z * s;
					a[jj * nc + i] = z * c - y * s;
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}

	return 0;
}

#undef SIGN
#undef PYTHAG

int mossSVD(register double *U, register double *W, register double *V, const double *matx, const int M, const int N) {
	double nrU[(M + 1) * (N + 1)];
	double nrV[(N + 1) * (N + 1)];
	double nrW[(N + 1)];

	const int nc = N + 1;
	int i, j;

	/* copy from given matx into nrU */
	for (i = 1; i <= M; i++)
		memcpy(&(nrU[i * nc + 1]), matx + N * (i - 1), N * sizeof(double));

	if (_mossNRsvdcmp(&*nrU, M, N, &*nrW, &*nrV)) {
		//trouble in core SVD calculation
		//return 1;
	}

	/* copy results into caller's arrays */
	for (i = 1; i <= M; i++)
		memcpy(U + N * (i - 1), &(nrU[i * nc + 1]), N * sizeof(double));
	memcpy(W, &(nrW[1]), N * sizeof(double));
	for (i = 1; i <= N; i++)
		memcpy(V + N * (i - 1), &(nrV[i * nc + 1]), N * sizeof(double));

	// sort
	int sorted = 0;
	while (sorted == 0) {

		sorted = 1;
		for (i = 1; i < N; i++) {
			if (W[i - 1] < W[i]) {
				sorted = 0;

				// in the W
				double t = W[i - 1];
				W[i - 1] = W[i];
				W[i] = t;

				for (j = 0; j < M; j++) {
					// in U
					t = U[j * N + (i - 1)];
					U[j * N + (i - 1)] = U[j * N + i];
					U[j * N + i] = t;

					// in V
					if (j < N) {
						t = V[j * N + (i - 1)];
						V[j * N + (i - 1)] = V[j * N + i];
						V[j * N + i] = t;
					}
				}
			}
		}
	}

	return 0;
}

int PseudoInverse(register double *inv, const double *matx, const int M, const int N) {
	double U[M * N];
	double W[N];
	double V[N * N];
	double ans;
	int i, j, k;

	if (mossSVD(&*U, &*W, &*V, matx, M, N)) return 1;

	//if (fabs(W[N-1]) < TINY_NEAR_ZERO) return 1;

	for (i = 0; i <= N - 1; i++) {
		for (j = 0; j <= M - 1; j++) {
			ans = 0.0;
			for (k = 0; k <= N - 1; k++) {
				/* in V: row fixed at i, k goes through columns */
				/* in U^T: column fixed at j, k goes through rows ==>
				 in U: row fixed at j, k goes through columns */
				ans += V[k + N * i] * U[k + N * j] / W[k];
			}
			inv[j + M * i] = ans;
		}
	}

	return 0;
}

int invert3x3(const double *src, double *inv) {
	const double m11 = src[0], m12 = src[1], m13 = src[2], m21 = src[3], m22 = src[4], m23 = src[5], m31 = src[6], m32 = src[7], m33 = src[8];
	double det = m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) + m13 * (m21 * m32 - m22 * m31);

	if (det == 0) return 1;

	det = (double) 1.0 / det;

	inv[0] = det * (m22 * m33 - m23 * m32);
	inv[1] = det * (m13 * m32 - m12 * m33);
	inv[2] = det * (m12 * m23 - m13 * m22);

	inv[3] = det * (m23 * m31 - m21 * m33);
	inv[4] = det * (m11 * m33 - m13 * m31);
	inv[5] = det * (m13 * m21 - m11 * m23);

	inv[6] = det * (m21 * m32 - m22 * m31);
	inv[7] = det * (m12 * m31 - m11 * m32);
	inv[8] = det * (m11 * m22 - m12 * m21);

	return 0;
}

void MultiplyMat(const double *m1, const double *m2, register double *res, const int M1, const int N1, const int M2, const int N2) {
	int timesInner = IMIN(N1, M2);
	int timesRows = M1;
	int timesCols = N2;
	double sum;

	int row, col, inner;
	for (row = 0; row < timesRows; row++) {
		for (col = 0; col < timesCols; col++) {
			sum = 0;
			for (inner = 0; inner < timesInner; inner++) {
				sum += m1[row * N1 + inner] * m2[inner * N2 + col];
			}
			*(res++) = sum;
		}
	}
}

#endif /* IMPL_SVD_INSPIRIT_ALG_HPP */
