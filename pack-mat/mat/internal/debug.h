/*
 * debug.h
 *
 *  Created on: Mar 8, 2013
 *      Author: gustavo
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "../impl/util/msg.h"

#define __imp_extra_debug( msg, ... )	if(IMP_EXTRA_DEBUG) __notice( msg, ## __VA_ARGS__)

#define __extra_debug() if(IMP_EXTRA_DEBUG) __notice("[ %s:%d ]\n", __func__, __LINE__ )

#define __extra_debug_info( OBJ ) if(IMP_EXTRA_DEBUG) { __notice( "[ %s:%d ] \t\tINFO: ", __func__, __LINE__ ); (OBJ).info(); }

#endif /* DEBUG_H_ */
