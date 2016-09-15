/* 
 * File:   mem.hpp
 * Author: gustavo
 *
 * Created on August 28, 2012, 2:16 AM
 */

#ifndef IMPL_MEM_H
#define	IMPL_MEM_H

#include <stdlib.h>
#include <stddef.h>

void mem_copy_function(char * dest, char * src, int nbytes);
void mem_clear_function(char * dest, int nbytes);

#define mem_alloc_size(size) (void *) calloc( (size_t)1, (size) )
#define mem_alloc(type) (type *) calloc( (size_t)1, sizeof(type) )
#define mem_allocn(n, type) (type *) calloc( (size_t)(n), sizeof(type) )
#define mem_free(p) { if(p) {free(p); p = 0;} }
#define mem_copy( src, dest, n ) ( mem_copy_function( (char*) src, (char*) dest, n*sizeof(*src) ) )
#define mem_clear( src, n) ( mem_clear_function( (char*) src, n*sizeof(*src) ) )
#define mem_clear_var( src, n) ( mem_clear_function( (char*) &src, n*sizeof(src) ) )

#define mem2d_idx( stride, x, y )			( (long) ( (x) + (y)*(stride) ) )
#define mem2d_val( data, stride, x, y )		( data[ mem2d_idx(stride, x, y) ] )
#define mem2d_pointer( data, stride, x, y)	&( mem2d_val(data, stride, x, y) )

#endif	/* IMPL_MEM_H */

