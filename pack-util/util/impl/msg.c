/*
 * msg.c
 *
 *  Created on: Jul 31, 2013
 *      Author: gtl
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include "msg.h"

/**
 * =============================================================================
 * =                               Defines                                     =
 * =============================================================================
 **/


/**
 * =============================================================================
 * =                      _msg functions implementation                        =
 * =============================================================================
 **/

void msg_set(FILE * f, int attr, int fg, int bg) {
//	char command[13];
	/* Command is the control command to the terminal */
//	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	fprintf(STD_OUTPUT, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
}

void msg_reset(FILE * f) {
	msg_set(f, MSG_ATTR_RESET, DEFAULT_FOREGROUND, DEFAULT_BACKGROUND);
}

void msg_print(FILE * f, const char * str, ...) {
	va_list vl;
	va_start(vl, str);
	vfprintf(f, str, vl);
	va_end(vl);
	fflush(f);
}

void msg_notice(const char * str, ...) {
	va_list vl;
	va_start(vl, str);

	msg_reset( NOTICE_OUT);
	msg_set( NOTICE_OUT, MSG_ATTR_BRIGHT, NOTICE_COLOR, DEFAULT_BACKGROUND);
	fprintf(NOTICE_OUT, MSG_NOTICE_MESSAGE);
	msg_reset( NOTICE_OUT);

	vfprintf(NOTICE_OUT, str, vl);
	va_end(vl);

	fflush(NOTICE_OUT);
}

void msg_warning(const char * str, ...) {
	va_list vl;
	va_start(vl, str);

	msg_reset( WARNING_OUT);
	msg_set( WARNING_OUT, MSG_ATTR_BRIGHT, WARNING_COLOR, DEFAULT_BACKGROUND);
	fprintf(WARNING_OUT, MSG_WARNING_MESSAGE);
	msg_reset( WARNING_OUT);

	vfprintf(WARNING_OUT, str, vl);
	va_end(vl);

	fflush(WARNING_OUT);
}

void msg_error(const char * str, ...) {
	va_list vl;
	va_start(vl, str);

	msg_reset( ERROR_OUT);
	msg_set( ERROR_OUT, MSG_ATTR_BRIGHT, ERROR_COLOR, DEFAULT_BACKGROUND);
	fprintf(ERROR_OUT, MSG_ERROR_MESSAGE);
	msg_reset( ERROR_OUT);

	vfprintf(ERROR_OUT, str, vl);
	va_end(vl);

	fflush(ERROR_OUT);
}

void msg_print_text(const char *template_arg, ...) {
	va_list ap;
	va_start(ap, template_arg);
	vfprintf(STD_OUTPUT, template_arg, ap);
	va_end(ap);
}

/**
 * =============================================================================
 * =                  Auxiliary functions implementation                       =
 * =============================================================================
 **/
void errno_print(const char * s) {
	msg_error("%s error %d, %s\n", s, errno, strerror(errno));
}

void errno_exit(const char * s) {
	msg_error("%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}
