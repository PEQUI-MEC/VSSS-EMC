/*
 * File:   math_macros.h
 * Author: gustavo
 *
 * Created on December 30, 2011, 2:41 AM
 */

#ifndef IMPL_MATH_MACROS_H
#define	IMPL_MATH_MACROS_H

// = Common comparations ===================================================

#ifndef __min
#define __min(a,b)	( (a)<(b) ? (a) : (b) )
#endif

#ifndef __max
#define __max(a,b)	( (a)>(b) ? (a) : (b) )
#endif

#ifndef __xor
#define __xor( a, b ) ( !(a) ^ !(b) )
#endif

#ifndef __abs
#define __abs(a)	( (a)< 0 ? -(a) : (a) )
#endif

#ifndef __sign
#define __sign(a)		( (a)< 0 ? -1 : 1 )
#endif

#ifndef __sqr
#define __sqr(a)   ( (a)*(a) )
#endif

#ifndef __clamp
#define __clamp(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

#ifndef imp__ceil
#define imp__ceil(a)		( ( ( (a)-(long)(a) ) != 0 )	?	( (a)>0	? (long)(a)+1 : (long)(a) )	:	( (long)(a) ) )
#endif

#ifndef imp__floor
#define imp__floor(a)	( ( ( (a)-(long)(a) ) != 0 )	?	( ( (a)<0 )	?	( (long)(a)-1 )	:	( (long)(a) ) )	:	( (long)(a) ) )
#endif

#ifndef __fix
#define __fix(a)		((a)>0)		?	imp__floor(a)		:	imp__ceil(a)
#endif

#ifndef imp__round
#define imp__round(a)	((a)>=0)	?	(long)((a)+0.5)	: (long)((a)-0.5)
#endif

#ifndef __M_PI
#define __M_PI		3.14159265358979323846	/* pi */
#endif

#define __rad2deg(r) (r)*180/M_PI
#define __deg2rad(r) (r)*M_PI/180

#endif	/* IMPL_MATH_MACROS_H */

