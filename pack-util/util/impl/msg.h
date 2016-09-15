/*
 * msg.h
 *
 *  Created on: Jul 30, 2013
 *      Author: gtl
 */

#ifndef MSG_H_
#define MSG_H_

#include <stdio.h>
#include <stdlib.h>

#define STD_OUTPUT	stdout
#define NOTICE_OUT stdout
#define WARNING_OUT stdout
#define ERROR_OUT stdout
#define ASSERT_OUT stdout

#define MSG_ERROR_MESSAGE	"    [ERROR]: "
#define MSG_WARNING_MESSAGE	"  [WARNING]: "
#define MSG_NOTICE_MESSAGE	"     [NOTE]: "
#define MSG_ASSERT_MESSAGE	"[ASSERTION]: "

/** Reset the text attributes */
#define MSG_ATTR_RESET		0
/** Enable the bright text attributes */
#define MSG_ATTR_BRIGHT		1
/** Enable the dim text attributes */
#define MSG_ATTR_DIM		2
/** Enable the blink text attributes */
#define MSG_ATTR_BLINK		3
/** Enable the underline text attributes */
#define MSG_ATTR_UNDERLINE	4
/** Enable the reverse color text attributes */
#define MSG_ATTR_REVERSE	7
/** Enable the hidden text attributes */
#define MSG_ATTR_HIDDEN		8

/** Message color text*/
#define MSG_COLOR_BLACK 	0
#define MSG_COLOR_RED       1
#define MSG_COLOR_GREEN		2
#define MSG_COLOR_YELLOW	3
#define MSG_COLOR_BLUE		4
#define MSG_COLOR_MAGENTA	5
#define MSG_COLOR_CYAN		6
#define MSG_COLOR_GRAY		7
#define MSG_COLOR_WHITE		8  // as background, WHITE means default bg
/** Default message color text */
#define DEFAULT_FOREGROUND	8
#define DEFAULT_BACKGROUND	8

#define NOTICE_COLOR    MSG_COLOR_GREEN
#define WARNING_COLOR   MSG_COLOR_YELLOW
#define ERROR_COLOR     MSG_COLOR_RED
#define ASSERT_COLOR    MSG_COLOR_YELLOW

#ifdef __cplusplus
extern "C" {
#endif

	void msg_print(FILE * f, const char * template_arg, ...);
	void msg_notice(const char * template_arg, ...);
	void msg_warning(const char * template_arg, ...);
	void msg_error(const char * template_arg, ...);

	/**
	 * @function void msg_set_attributes(int attr, int fg, int bg);
	 * @param attr - text attribute
	 * @param fg - foreground color
	 * @param bg - background color
	 */
//void msg_set_fg(FILE * f, int fg);
//void msg_set_bg(FILE * f, int fg);
//void msg_set_attr(FILE * f, int fg);
	void msg_set(FILE * f, int attr, int fg, int bg);

//void msg_reset_fg(FILE * f);
//void msg_reset_bg(FILE * f);
//void msg_reset_attr(FILE * f);
	void msg_reset(FILE * f);

	void errno_print(const char * s);
	void errno_exit(const char * s);

#ifdef __cplusplus
}
#endif

#define __print_code_info( f ) msg_print( f, "\t-- Code info ----------------\n\t|Archive: %s\n\t|Line: %d\n\t|Function: %s\n\n", __FILE__, __LINE__, __func__ )

#define __print_notice_label	msg_set( NOTICE_OUT, MSG_ATTR_BRIGHT, NOTICE_COLOR, DEFAULT_BACKGROUND); fprintf( NOTICE_OUT, MSG_NOTICE_MESSAGE); msg_reset(NOTICE_OUT)
#define __print_warning_label	msg_set( WARNING_OUT, MSG_ATTR_BRIGHT, WARNING_COLOR, DEFAULT_BACKGROUND); fprintf( WARNING_OUT, MSG_WARNING_MESSAGE); msg_reset(WARNING_OUT)
#define __print_error_label		msg_set( ERROR_OUT, MSG_ATTR_BRIGHT, ERROR_COLOR, DEFAULT_BACKGROUND); fprintf( ERROR_OUT, MSG_ERROR_MESSAGE); msg_reset(ERROR_OUT)
#define __print_assert_label	msg_set( ASSERT_OUT, MSG_ATTR_BRIGHT, ASSERT_COLOR, DEFAULT_BACKGROUND); fprintf( ASSERT_OUT, MSG_ASSERT_MESSAGE); msg_reset(ASSERT_OUT)

#define __notice( msg, ... )	__print_notice_label; msg_print( NOTICE_OUT, msg, ## __VA_ARGS__); __print_code_info( NOTICE_OUT )
#define __warning( msg, ... )	__print_warning_label; msg_print( WARNING_OUT, msg, ## __VA_ARGS__); __print_code_info( WARNING_OUT )
#define __error( msg, ... )		__print_error_label; msg_print( ERROR_OUT, msg, ## __VA_ARGS__); __print_code_info( ERROR_OUT )

#define __assert( expr ) \
	if( !(expr) ) { \
		__print_assert_label; \
		msg_print( ASSERT_OUT,  "\t[ (%s) is false ]\n", #expr ); \
		__print_code_info(ASSERT_OUT); \
		exit(1);\
	}

#define __assert_msg( expr, msg, ... ) \
	if( !(expr) ) { \
		__print_assert_label; \
		msg_print( ASSERT_OUT, "\t[ (%s) is false ]\n", #expr ); \
		msg_print( ASSERT_OUT, "\n\t" msg, ## __VA_ARGS__ ); \
		msg_print( ASSERT_OUT, "\n\n" ); \
		__print_code_info(ASSERT_OUT); \
		exit(1);\
	}

#define __warning_if_fail(expr) { \
	if(expr) { } \
	else { \
		__warning( "_warning_if_fail(%s)==0 present in function: %s. [ %s:%d ]\n", \
					#expr, __func__, __FILE__, __LINE__ ); \
	} \
}

#endif /* MSG_H_ */
