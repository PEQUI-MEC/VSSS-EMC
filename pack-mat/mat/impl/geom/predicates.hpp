/*
 * predicates.hpp
 *
 *  Created on: Sep 22, 2012
 *      Author: gustavo
 */

#ifndef IMPL_PREDICATES_HPP
#define IMPL_PREDICATES_HPP

#include "../util/types.h"
#include "../math/math_macros.h"

#define TOL 1e-300

#define X	0
#define Y	1
#define Z	2

namespace impl {
	namespace geom {

		/* ========================================================================== */

		template<typename T> T __orient2d( T * pa, T * pb, T * pc ) {
			/**
			 *  Use the determinant form. a, b and c are points in R².
			 *
			 *           | a.x  a.y  1 |
			 *  det(M) = | b.x  b.y  1 | = 2 * Area(a, b, c)
			 *           | c.x  c.y  1 |
			 */
			T adx, ady;
			T bdx, bdy;
			T orient;

			adx = pa[X] - pc[X];
			ady = pa[Y] - pc[Y];

			bdx = pb[X] - pc[X];
			bdy = pb[Y] - pc[Y];

			orient = adx * bdy - ady * bdx;

			return orient;
		}

		template<typename T> T __in_circle( T * pa, T * pb, T * pc, T * pd ) {

			T adx, ady, bdx, bdy, cdx, cdy;
			T abdet, bcdet, cadet;
			T alift, blift, clift;

			T orient;

			adx = pa[X] - pd[X];
			ady = pa[Y] - pd[Y];
			bdx = pb[X] - pd[X];
			bdy = pb[Y] - pd[Y];
			cdx = pc[X] - pd[X];
			cdy = pc[Y] - pd[Y];

			abdet = adx * bdy - bdx * ady;
			bcdet = bdx * cdy - cdx * bdy;
			cadet = cdx * ady - adx * cdy;
			alift = adx * adx + ady * ady;
			blift = bdx * bdx + bdy * bdy;
			clift = cdx * cdx + cdy * cdy;

			orient = alift * bcdet + blift * cadet + clift * abdet;
			return orient;
		}
		template<typename T> T __orient3d( T * pa, T * pb, T * pc, T * pd ) {

			/**
			 *  Use the determinant form. a, b and c are points in R².
			 *
			 *           | a.x  a.y  1 |
			 *  det(M) = | b.x  b.y  1 | = 2 * Area(a, b, c)
			 *           | c.x  c.y  1 |
			 */
			T ad[3];
			T bd[3];
			T cd[3];

			T bdxcdy, cdxbdy, cdxady, adxcdy, adxbdy, bdxady;
			T det;

			ad[X] = pa[X] - pd[X];
			bd[X] = pb[X] - pd[X];
			cd[X] = pc[X] - pd[X];

			ad[Y] = pa[Y] - pd[Y];
			bd[Y] = pb[Y] - pd[Y];
			cd[Y] = pc[Y] - pd[Y];

			ad[Z] = pa[Z] - pd[Z];
			bd[Z] = pb[Z] - pd[Z];
			cd[Z] = pc[Z] - pd[Z];

			bdxcdy = bd[X] * cd[Y];
			cdxbdy = cd[X] * bd[Y];

			cdxady = cd[X] * ad[Y];
			adxcdy = ad[X] * cd[Y];

			adxbdy = ad[X] * bd[Y];
			bdxady = bd[X] * ad[Y];

			det = ad[Z] * (bdxcdy - cdxbdy) + bd[Z] * (cdxady - adxcdy) + cd[Z] * (adxbdy - bdxady);

			return det;
		}

		template<typename T> bool __in_rect( T * pa, T * pb, T * pc ) {

			if (pa[X] != pb[X]) return (pa[X] <= pc[X] && pc[X] <= pb[X]) || (pa[X] >= pc[X] && pc[X] >= pb[X]);
			else return (pa[Y] <= pc[Y] && pc[Y] <= pb[Y]) || (pa[Y] >= pc[Y] && pc[Y] >= pb[Y]);

		}

		/* ========================================================================== */

		template<typename T> T __area2( T * pa, T * pb, T * pc ) {
			T ar;
			ar = __orient2d( pa, pb, pc );
			ar = abs( ar );
			return ar;
		}
		template<typename T> double __area( T * pa, T * pb, T * pc ) {
			double ar;
			ar = __area2( pa, pb, pc ) / 2.0;
			return ar;
		}

		/* ========================================================================== */

		template<typename T> bool __in_left( T * pa, T * pb, T * pc ) {
			return __orient2d( pa, pb, pc ) > 0;
		}
		template<typename T> bool __in_left_on( T * pa, T * pb, T * pc ) {
			return __orient2d( pa, pb, pc ) >= 0;
		}

		/* ========================================================================== */

		template<typename T> bool __are_collinear( T * pa, T * pb, T * pc ) {
			return __orient2d( pa, pb, pc ) == 0;
		}

		/* ========================================================================== */

		template<typename T> bool _in_right( T * pa, T * pb, T * pc ) {
			return __orient2d( pa, pb, pc ) < 0;
		}
		template<typename T> bool _in_right_on( T * pa, T * pb, T * pc ) {
			return __orient2d( pa, pb, pc ) <= 0;
		}

		/* ========================================================================== */

		template<typename T> bool __is_intersect_prop( T * pa, T * pb, T * pc, T * pd ) {
			if (__are_collinear( pa, pb, pc ) || __are_collinear( pa, pb, pd ) || __are_collinear( pc, pd, pa ) || __are_collinear( pc, pd, pb )) return FALSE;

			return __xor( __in_left(pa, pb, pc), __in_left(pa, pb, pd)) && __xor( __in_left(pc, pd, pa), __in_left(pc, pd, pb));
		}

		template<typename T> bool __is_between( T * pa, T * pb, T * pc ) {
			if (!__are_collinear( pa, pb, pc )) return FALSE;

			if (pa[X] != pb[X]) return (pa[X] <= pc[X] && pc[X] <= pb[X]) || (pa[X] >= pc[X] && pc[X] >= pb[X]);
			else return (pa[Y] <= pc[Y] && pc[Y] <= pb[Y]) || (pa[Y] >= pc[Y] && pc[Y] >= pb[Y]);
		}

		template<typename T> bool __is_intersect( T * pa, T * pb, T * pc, T * pd ) {
			if (__is_intersect_prop( pa, pb, pc, pd )) return TRUE;

			else if (__is_between( pa, pb, pc ) || __is_between( pa, pb, pd ) || __is_between( pc, pd, pa ) || __is_between( pc, pd, pb )) return TRUE;
			else return FALSE;
		}

		template<typename T> T * __cross_product_2d( T * pa, T * cp ) {
			cp[X] = pa[Y];
			cp[Y] = -pa[X];
			return cp;
		}
		template<typename T> T * __cross_product_3d( T * pa, T * pb, T * cp ) {

			cp[X] = pa[Y] * pb[Z] - pa[Z] * pb[Y];
			cp[Y] = pa[X] * pb[Z] - pa[Z] * pb[X];
			cp[Z] = pa[X] * pb[Y] - pa[Y] * pb[X];

			return cp;
		}

		template<typename T> bool __circumcircle_center( T * pa, T * pb, T *pc, double * x, double * y ) {

			T xa, ya, xb, yb, xc, yc;
			double xd, yd, xe, ye;
			double xad, yad, xae, yae;
			double det;

			xa = pa[X];
			ya = pa[Y];

			xb = pb[X];
			yb = pb[X];

			xc = pc[X];
			yc = pc[Y];

			xd = (xa + xb) / 2.;
			yd = (ya + yb) / 2.;
			xe = (xa + xc) / 2.;
			ye = (ya + yc) / 2.;

			xad = xd - xa;
			yad = yd - ya;
			xae = xe - xa;
			yae = ye - ya;
			det = xad * yae - xae * yad;

			if (__abs(det) < TOL) return false;

			*x = (yae * yad * (yd - ye) + xad * yae * xd - xae * yad * xe) / det;
			*y = -(xae * xad * (xd - xe) + yad * xae * yd - yae * xad * ye) / det;

			return true;

		}

	}
}

#undef X
#undef Y
#undef Z

#endif /* IMPL_PREDICATES_HPP */
