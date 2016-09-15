/* 
 * File:   types.h
 * Author: gustavo
 *
 * Created on August 6, 2011, 1:40 AM
 */

#ifndef IMPL_TYPES_H
#define	IMPL_TYPES_H


/* ====================================================================== */
typedef char bit;
typedef char byte;
typedef signed char schar;
typedef unsigned char uchar;
typedef signed int sint;
typedef unsigned int uint;
typedef long int lint;

#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (!TRUE)
#endif

typedef char boolean;
typedef void * pointer;
typedef double real;

/* ====================================================================== */

#endif	/* IMPL_TYPES_H */


