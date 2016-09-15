/*
 * File:   common_macros.h
 * Author: gustavo
 *
 * Created on December 30, 2011, 2:39 AM
 */

#ifndef IMPL_COMMON_MACROS_H
#define	IMPL_COMMON_MACROS_H

#include <stdlib.h>

/* = Common definitions ================================================= */

#ifndef NULL
#define NULL ( (void *)0 )
#endif

// = Others ================================================================
#define __swap( a, b, type ) { type __tmp__swap___; __tmp__swap___= (a); (a) = (b); (b) = __tmp__swap___; }

// = Common controls =======================================================
#define __exit_if_fail(expr)				{ if( !(expr) ) { exit(EXIT_FAILURE);	} }
#define __return_if_fail(expr)				{ if( !(expr) ) { return; 				} }
#define __return_val_if_fail(expr, val)		{ if( !(expr) ) { return val;			} }

template<class T> void swapp(T& v1, T& v2) {
	T tmp;
	tmp = v1;
	v1 = v2;
	v2 = tmp;
}

#endif	/* IMPL_COMMON_MACROS_H */

