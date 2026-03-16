#line 1 "lex.yy.cpp"

#line 3 "lex.yy.cpp"

#define  YY_INT_ALIGNED short int



#define FLEX_SCANNER
#define YY_FLEX_MAJOR_VERSION 2
#define YY_FLEX_MINOR_VERSION 6
#define YY_FLEX_SUBMINOR_VERSION 4
#if YY_FLEX_SUBMINOR_VERSION > 0
#define FLEX_BETA
#endif




#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>





#ifndef FLEXINT_H
#define FLEXINT_H



#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L


#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#endif

#include <inttypes.h>
typedef int8_t flex_int8_t;
typedef uint8_t flex_uint8_t;
typedef int16_t flex_int16_t;
typedef uint16_t flex_uint16_t;
typedef int32_t flex_int32_t;
typedef uint32_t flex_uint32_t;
#else
typedef signed char flex_int8_t;
typedef short int flex_int16_t;
typedef int flex_int32_t;
typedef unsigned char flex_uint8_t; 
typedef unsigned short int flex_uint16_t;
typedef unsigned int flex_uint32_t;


#ifndef INT8_MIN
#define INT8_MIN               (-128)
#endif
#ifndef INT16_MIN
#define INT16_MIN              (-32767-1)
#endif
#ifndef INT32_MIN
#define INT32_MIN              (-2147483647-1)
#endif
#ifndef INT8_MAX
#define INT8_MAX               (127)
#endif
#ifndef INT16_MAX
#define INT16_MAX              (32767)
#endif
#ifndef INT32_MAX
#define INT32_MAX              (2147483647)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX              (255U)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX             (65535U)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX             (4294967295U)
#endif

#ifndef SIZE_MAX
#define SIZE_MAX               (~(size_t)0)
#endif

#endif 

#endif 




#define yyconst const

#if defined(__GNUC__) && __GNUC__ >= 3
#define yynoreturn __attribute__((__noreturn__))
#else
#define yynoreturn
#endif


#define YY_NULL 0


#define YY_SC_TO_UI(c) ((YY_CHAR) (c))


#define BEGIN (yy_start) = 1 + 2 *

#define YY_START (((yy_start) - 1) / 2)
#define YYSTATE YY_START

#define YY_STATE_EOF(state) (YY_END_OF_BUFFER + state + 1)

#define YY_NEW_FILE yyrestart( yyin  )
#define YY_END_OF_BUFFER_CHAR 0


#ifndef YY_BUF_SIZE
#ifdef __ia64__

#define YY_BUF_SIZE 32768
#else
#define YY_BUF_SIZE 16384
#endif 
#endif


#define YY_STATE_BUF_SIZE   ((YY_BUF_SIZE + 2) * sizeof(yy_state_type))

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

#ifndef YY_TYPEDEF_YY_SIZE_T
#define YY_TYPEDEF_YY_SIZE_T
typedef size_t yy_size_t;
#endif

extern int yyleng;

extern FILE *yyin, *yyout;

#define EOB_ACT_CONTINUE_SCAN 0
#define EOB_ACT_END_OF_FILE 1
#define EOB_ACT_LAST_MATCH 2
    
    
    #define  YY_LESS_LINENO(n) \
            do { \
                int yyl;\
                for ( yyl = n; yyl < yyleng; ++yyl )\
                    if ( yytext[yyl] == '\n' )\
                        --yylineno;\
            }while(0)
    #define YY_LINENO_REWIND_TO(dst) \
            do {\
                const char *p;\
                for ( p = yy_cp-1; p >= (dst); --p)\
                    if ( *p == '\n' )\
                        --yylineno;\
            }while(0)
    

#define yyless(n) \
	do \
		{ \
		 \
        int yyless_macro_arg = (n); \
        YY_LESS_LINENO(yyless_macro_arg);\
		*yy_cp = (yy_hold_char); \
		YY_RESTORE_YY_MORE_OFFSET \
		(yy_c_buf_p) = yy_cp = yy_bp + yyless_macro_arg - YY_MORE_ADJ; \
		YY_DO_BEFORE_ACTION;  \
		} \
	while ( 0 )
#define unput(c) yyunput( c, (yytext_ptr)  )

#ifndef YY_STRUCT_YY_BUFFER_STATE
#define YY_STRUCT_YY_BUFFER_STATE
struct yy_buffer_state
	{
	FILE *yy_input_file;

	char *yy_ch_buf;		
	char *yy_buf_pos;		

	
	int yy_buf_size;

	
	int yy_n_chars;

	
	int yy_is_our_buffer;

	
	int yy_is_interactive;

	
	int yy_at_bol;

    int yy_bs_lineno; 
    int yy_bs_column; 

	
	int yy_fill_buffer;

	int yy_buffer_status;

#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
	
#define YY_BUFFER_EOF_PENDING 2

	};
#endif 


static size_t yy_buffer_stack_top = 0; 
static size_t yy_buffer_stack_max = 0; 
static YY_BUFFER_STATE * yy_buffer_stack = NULL; 


#define YY_CURRENT_BUFFER ( (yy_buffer_stack) \
                          ? (yy_buffer_stack)[(yy_buffer_stack_top)] \
                          : NULL)

#define YY_CURRENT_BUFFER_LVALUE (yy_buffer_stack)[(yy_buffer_stack_top)]


static char yy_hold_char;
static int yy_n_chars;		
int yyleng;


static char *yy_c_buf_p = NULL;
static int yy_init = 0;		
static int yy_start = 0;	


static int yy_did_buffer_switch_on_eof;

void yyrestart ( FILE *input_file  );
void yy_switch_to_buffer ( YY_BUFFER_STATE new_buffer  );
YY_BUFFER_STATE yy_create_buffer ( FILE *file, int size  );
void yy_delete_buffer ( YY_BUFFER_STATE b  );
void yy_flush_buffer ( YY_BUFFER_STATE b  );
void yypush_buffer_state ( YY_BUFFER_STATE new_buffer  );
void yypop_buffer_state ( void );

static void yyensure_buffer_stack ( void );
static void yy_load_buffer_state ( void );
static void yy_init_buffer ( YY_BUFFER_STATE b, FILE *file  );
#define YY_FLUSH_BUFFER yy_flush_buffer( YY_CURRENT_BUFFER )

YY_BUFFER_STATE yy_scan_buffer ( char *base, yy_size_t size  );
YY_BUFFER_STATE yy_scan_string ( const char *yy_str  );
YY_BUFFER_STATE yy_scan_bytes ( const char *bytes, int len  );

void *yyalloc ( yy_size_t  );
void *yyrealloc ( void *, yy_size_t  );
void yyfree ( void *  );

#define yy_new_buffer yy_create_buffer
#define yy_set_interactive(is_interactive) \
	{ \
	if ( ! YY_CURRENT_BUFFER ){ \
        yyensure_buffer_stack (); \
		YY_CURRENT_BUFFER_LVALUE =    \
            yy_create_buffer( yyin, YY_BUF_SIZE ); \
	} \
	YY_CURRENT_BUFFER_LVALUE->yy_is_interactive = is_interactive; \
	}
#define yy_set_bol(at_bol) \
	{ \
	if ( ! YY_CURRENT_BUFFER ){\
        yyensure_buffer_stack (); \
		YY_CURRENT_BUFFER_LVALUE =    \
            yy_create_buffer( yyin, YY_BUF_SIZE ); \
	} \
	YY_CURRENT_BUFFER_LVALUE->yy_at_bol = at_bol; \
	}
#define YY_AT_BOL() (YY_CURRENT_BUFFER_LVALUE->yy_at_bol)



#define yywrap() (1)
#define YY_SKIP_YYWRAP
typedef flex_uint8_t YY_CHAR;

FILE *yyin = NULL, *yyout = NULL;

typedef int yy_state_type;

extern int yylineno;
int yylineno = 1;

extern char *yytext;
#ifdef yytext_ptr
#undef yytext_ptr
#endif
#define yytext_ptr yytext

static yy_state_type yy_get_previous_state ( void );
static yy_state_type yy_try_NUL_trans ( yy_state_type current_state  );
static int yy_get_next_buffer ( void );
static void yynoreturn yy_fatal_error ( const char* msg  );


#define YY_DO_BEFORE_ACTION \
	(yytext_ptr) = yy_bp; \
	yyleng = (int) (yy_cp - yy_bp); \
	(yy_hold_char) = *yy_cp; \
	*yy_cp = '\0'; \
	(yy_c_buf_p) = yy_cp;
#define YY_NUM_RULES 49
#define YY_END_OF_BUFFER 50

struct yy_trans_info
	{
	flex_int32_t yy_verify;
	flex_int32_t yy_nxt;
	};
static const flex_int16_t yy_accept[98] =
    {   0,
        0,    0,   50,   48,   46,   47,   33,   48,   29,   48,
       34,   35,   27,   25,   41,   26,   48,   28,   43,   40,
       31,   30,   32,   45,   38,   39,   45,   45,   45,   45,
       45,   45,   45,   45,   36,   48,   37,   46,   20,    0,
       44,    0,   23,   17,   13,   15,   14,   16,   42,    0,
        1,   18,   42,   43,   21,   19,   22,   45,   45,   45,
       45,   45,    8,   45,   45,   45,   45,   45,   24,    0,
        0,    1,   42,   45,   45,   45,   11,    3,   45,   45,
       45,   45,    2,    5,    9,   45,   45,   45,    6,   45,
        4,   45,   45,   10,   12,    7,    0

    } ;

static const YY_CHAR yy_ec[256] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    2,    3,
        1,    1,    2,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    2,    4,    5,    1,    1,    6,    7,    1,    8,
        9,   10,   11,   12,   13,   14,   15,   16,   16,   16,
       16,   16,   16,   16,   16,   16,   16,    1,   17,   18,
       19,   20,    1,    1,   21,   21,   21,   21,   21,   21,
       21,   21,   21,   21,   21,   21,   21,   21,   21,   21,
       21,   21,   21,   21,   21,   21,   21,   21,   21,   21,
       22,   23,   24,    1,   21,    1,   25,   21,   26,   27,

       28,   29,   21,   30,   31,   21,   21,   32,   21,   33,
       34,   21,   21,   35,   36,   37,   38,   39,   40,   21,
       21,   21,   41,   42,   43,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,

        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1
    } ;

static const YY_CHAR yy_meta[44] =
    {   0,
        1,    1,    2,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    3,    1,    1,    1,    1,
        3,    1,    1,    1,    3,    3,    3,    3,    3,    3,
        3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
        1,    1,    1
    } ;

static const flex_int16_t yy_base[102] =
    {   0,
        0,    0,  118,  119,  115,  119,   97,   39,  119,  108,
      119,  119,   95,   34,  119,   33,   97,   39,   34,  119,
       93,   92,   91,    0,  119,  119,   79,   76,   23,   18,
       79,   78,   71,   74,  119,   61,  119,  100,  119,   51,
      119,   98,  119,  119,  119,  119,  119,  119,   84,   89,
        0,  119,   82,   45,  119,  119,  119,    0,   72,   60,
       61,   59,    0,   56,   55,   58,   59,   58,  119,   78,
       50,    0,   71,   51,   57,   59,    0,    0,   44,   37,
       45,   39,  119,    0,    0,   33,   34,   34,    0,   39,
        0,   33,   29,    0,    0,    0,  119,   74,   60,   77,

       80
    } ;

static const flex_int16_t yy_def[102] =
    {   0,
       97,    1,   97,   97,   97,   97,   97,   98,   97,   97,
       97,   97,   97,   97,   97,   97,   97,   97,   97,   97,
       97,   97,   97,   99,   97,   97,   99,   99,   99,   99,
       99,   99,   99,   99,   97,   97,   97,   97,   97,   98,
       97,   98,   97,   97,   97,   97,   97,   97,   97,  100,
      101,   97,   97,   97,   97,   97,   97,   99,   99,   99,
       99,   99,   99,   99,   99,   99,   99,   99,   97,  100,
      100,  101,   97,   99,   99,   99,   99,   99,   99,   99,
       99,   99,   97,   99,   99,   99,   99,   99,   99,   99,
       99,   99,   99,   99,   99,   99,    0,   97,   97,   97,

       97
    } ;

static const flex_int16_t yy_nxt[163] =
    {   0,
        4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
       14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
       24,   25,    4,   26,   24,   27,   24,   28,   29,   24,
       30,   24,   24,   24,   31,   24,   32,   24,   33,   34,
       35,   36,   37,   41,   45,   47,   63,   53,   50,   54,
       64,   48,   46,   51,   61,   41,   62,   52,   53,   71,
       54,   42,   58,   96,   83,   95,   94,   93,   92,   91,
       90,   89,   88,   42,   40,   40,   40,   70,   70,   70,
       72,   87,   72,   86,   85,   84,   73,   71,   82,   81,
       80,   79,   78,   77,   76,   75,   74,   73,   71,   49,

       97,   38,   69,   68,   67,   66,   65,   60,   59,   57,
       56,   55,   49,   44,   43,   39,   38,   97,    3,   97,
       97,   97,   97,   97,   97,   97,   97,   97,   97,   97,
       97,   97,   97,   97,   97,   97,   97,   97,   97,   97,
       97,   97,   97,   97,   97,   97,   97,   97,   97,   97,
       97,   97,   97,   97,   97,   97,   97,   97,   97,   97,
       97,   97
    } ;

static const flex_int16_t yy_chk[163] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    8,   14,   16,   30,   19,   18,   19,
       30,   16,   14,   18,   29,   40,   29,   18,   54,   71,
       54,    8,   99,   93,   71,   92,   90,   88,   87,   86,
       82,   81,   80,   40,   98,   98,   98,  100,  100,  100,
      101,   79,  101,   76,   75,   74,   73,   70,   68,   67,
       66,   65,   64,   62,   61,   60,   59,   53,   50,   49,

       42,   38,   36,   34,   33,   32,   31,   28,   27,   23,
       22,   21,   17,   13,   10,    7,    5,    3,   97,   97,
       97,   97,   97,   97,   97,   97,   97,   97,   97,   97,
       97,   97,   97,   97,   97,   97,   97,   97,   97,   97,
       97,   97,   97,   97,   97,   97,   97,   97,   97,   97,
       97,   97,   97,   97,   97,   97,   97,   97,   97,   97,
       97,   97
    } ;


static const flex_int32_t yy_rule_can_match_eol[50] =
    {   0,
0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 1, 0, 0,     };

static yy_state_type yy_last_accepting_state;
static char *yy_last_accepting_cpos;

extern int yy_flex_debug;
int yy_flex_debug = 0;


#define REJECT reject_used_but_not_detected
#define yymore() yymore_used_but_not_detected
#define YY_MORE_ADJ 0
#define YY_RESTORE_YY_MORE_OFFSET
char *yytext;
#line 1 "lexer.l"
#line 2 "lexer.l"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include "parser.tab.hpp"

void report_lexical_error(const char* text, int line);
#line 549 "lex.yy.cpp"

#line 551 "lex.yy.cpp"

#define INITIAL 0

#ifndef YY_NO_UNISTD_H

#include <unistd.h>
#endif

#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE void *
#endif

static int yy_init_globals ( void );



int yylex_destroy ( void );

int yyget_debug ( void );

void yyset_debug ( int debug_flag  );

YY_EXTRA_TYPE yyget_extra ( void );

void yyset_extra ( YY_EXTRA_TYPE user_defined  );

FILE *yyget_in ( void );

void yyset_in  ( FILE * _in_str  );

FILE *yyget_out ( void );

void yyset_out  ( FILE * _out_str  );

			int yyget_leng ( void );

char *yyget_text ( void );

int yyget_lineno ( void );

void yyset_lineno ( int _line_number  );



#ifndef YY_SKIP_YYWRAP
#ifdef __cplusplus
extern "C" int yywrap ( void );
#else
extern int yywrap ( void );
#endif
#endif

#ifndef YY_NO_UNPUT
    
    static void yyunput ( int c, char *buf_ptr  );
    
#endif

#ifndef yytext_ptr
static void yy_flex_strncpy ( char *, const char *, int );
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen ( const char * );
#endif

#ifndef YY_NO_INPUT
#ifdef __cplusplus
static int yyinput ( void );
#else
static int input ( void );
#endif

#endif


#ifndef YY_READ_BUF_SIZE
#ifdef __ia64__

#define YY_READ_BUF_SIZE 16384
#else
#define YY_READ_BUF_SIZE 8192
#endif 
#endif


#ifndef ECHO

#define ECHO do { if (fwrite( yytext, (size_t) yyleng, 1, yyout )) {} } while (0)
#endif


#ifndef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( YY_CURRENT_BUFFER_LVALUE->yy_is_interactive ) \
		{ \
		int c = '*'; \
		int n; \
		for ( n = 0; n < max_size && \
			     (c = getc( yyin )) != EOF && c != '\n'; ++n ) \
			buf[n] = (char) c; \
		if ( c == '\n' ) \
			buf[n++] = (char) c; \
		if ( c == EOF && ferror( yyin ) ) \
			YY_FATAL_ERROR( "input in flex scanner failed" ); \
		result = n; \
		} \
	else \
		{ \
		errno=0; \
		while ( (result = (int) fread(buf, 1, (yy_size_t) max_size, yyin)) == 0 && ferror(yyin)) \
			{ \
			if( errno != EINTR) \
				{ \
				YY_FATAL_ERROR( "input in flex scanner failed" ); \
				break; \
				} \
			errno=0; \
			clearerr(yyin); \
			} \
		}\
\

#endif


#ifndef yyterminate
#define yyterminate() return YY_NULL
#endif


#ifndef YY_START_STACK_INCR
#define YY_START_STACK_INCR 25
#endif


#ifndef YY_FATAL_ERROR
#define YY_FATAL_ERROR(msg) yy_fatal_error( msg )
#endif




#ifndef YY_DECL
#define YY_DECL_IS_OURS 1

extern int yylex (void);

#define YY_DECL int yylex (void)
#endif 


#ifndef YY_USER_ACTION
#define YY_USER_ACTION
#endif


#ifndef YY_BREAK
#define YY_BREAK break;
#endif

#define YY_RULE_SETUP \
	YY_USER_ACTION


YY_DECL
{
	yy_state_type yy_current_state;
	char *yy_cp, *yy_bp;
	int yy_act;
    
	if ( !(yy_init) )
		{
		(yy_init) = 1;

#ifdef YY_USER_INIT
		YY_USER_INIT;
#endif

		if ( ! (yy_start) )
			(yy_start) = 1;	

		if ( ! yyin )
			yyin = stdin;

		if ( ! yyout )
			yyout = stdout;

		if ( ! YY_CURRENT_BUFFER ) {
			yyensure_buffer_stack ();
			YY_CURRENT_BUFFER_LVALUE =
				yy_create_buffer( yyin, YY_BUF_SIZE );
		}

		yy_load_buffer_state(  );
		}

	{
#line 26 "lexer.l"


#line 29 "lexer.l"
 

 
#line 775 "lex.yy.cpp"

	while ( 1 )		
		{
		yy_cp = (yy_c_buf_p);

		
		*yy_cp = (yy_hold_char);

		
		yy_bp = yy_cp;

		yy_current_state = (yy_start);
yy_match:
		do
			{
			YY_CHAR yy_c = yy_ec[YY_SC_TO_UI(*yy_cp)] ;
			if ( yy_accept[yy_current_state] )
				{
				(yy_last_accepting_state) = yy_current_state;
				(yy_last_accepting_cpos) = yy_cp;
				}
			while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
				{
				yy_current_state = (int) yy_def[yy_current_state];
				if ( yy_current_state >= 98 )
					yy_c = yy_meta[yy_c];
				}
			yy_current_state = yy_nxt[yy_base[yy_current_state] + yy_c];
			++yy_cp;
			}
		while ( yy_base[yy_current_state] != 119 );

yy_find_action:
		yy_act = yy_accept[yy_current_state];
		if ( yy_act == 0 )
			{ 
			yy_cp = (yy_last_accepting_cpos);
			yy_current_state = (yy_last_accepting_state);
			yy_act = yy_accept[yy_current_state];
			}

		YY_DO_BEFORE_ACTION;

		if ( yy_act != YY_END_OF_BUFFER && yy_rule_can_match_eol[yy_act] )
			{
			int yyl;
			for ( yyl = 0; yyl < yyleng; ++yyl )
				if ( yytext[yyl] == '\n' )
					
    yylineno++;
;
			}

do_action:	

		switch ( yy_act )
	{ 
			case 0: 
			
			*yy_cp = (yy_hold_char);
			yy_cp = (yy_last_accepting_cpos);
			yy_current_state = (yy_last_accepting_state);
			goto yy_find_action;

case 1:
YY_RULE_SETUP
#line 32 "lexer.l"
{  }
	YY_BREAK
case 2:

YY_RULE_SETUP
#line 33 "lexer.l"
{  }
	YY_BREAK

case 3:
YY_RULE_SETUP
#line 36 "lexer.l"
{ return TOKEN_INT;    }
	YY_BREAK
case 4:
YY_RULE_SETUP
#line 37 "lexer.l"
{ return TOKEN_FLOAT;  }
	YY_BREAK
case 5:
YY_RULE_SETUP
#line 38 "lexer.l"
{ return TOKEN_CHAR;   }
	YY_BREAK
case 6:
YY_RULE_SETUP
#line 39 "lexer.l"
{ return TOKEN_VOID;   }
	YY_BREAK
case 7:
YY_RULE_SETUP
#line 40 "lexer.l"
{ return TOKEN_TENSOR; }
	YY_BREAK
case 8:
YY_RULE_SETUP
#line 41 "lexer.l"
{ return TOKEN_IF;     }
	YY_BREAK
case 9:
YY_RULE_SETUP
#line 42 "lexer.l"
{ return TOKEN_ELSE;   }
	YY_BREAK
case 10:
YY_RULE_SETUP
#line 43 "lexer.l"
{ return TOKEN_WHILE;  }
	YY_BREAK
case 11:
YY_RULE_SETUP
#line 44 "lexer.l"
{ return TOKEN_FOR;    }
	YY_BREAK
case 12:
YY_RULE_SETUP
#line 45 "lexer.l"
{ return TOKEN_RETURN; }
	YY_BREAK

case 13:
YY_RULE_SETUP
#line 48 "lexer.l"
{ return TOKEN_INCREMENT;    }
	YY_BREAK
case 14:
YY_RULE_SETUP
#line 49 "lexer.l"
{ return TOKEN_DECREMENT;    }
	YY_BREAK
case 15:
YY_RULE_SETUP
#line 50 "lexer.l"
{ return TOKEN_PLUS_ASSIGN;  }
	YY_BREAK
case 16:
YY_RULE_SETUP
#line 51 "lexer.l"
{ return TOKEN_MINUS_ASSIGN; }
	YY_BREAK
case 17:
YY_RULE_SETUP
#line 52 "lexer.l"
{ return TOKEN_STAR_ASSIGN;  }
	YY_BREAK
case 18:
YY_RULE_SETUP
#line 53 "lexer.l"
{ return TOKEN_SLASH_ASSIGN; }
	YY_BREAK
case 19:
YY_RULE_SETUP
#line 54 "lexer.l"
{ return TOKEN_EQ;           }
	YY_BREAK
case 20:
YY_RULE_SETUP
#line 55 "lexer.l"
{ return TOKEN_NEQ;          }
	YY_BREAK
case 21:
YY_RULE_SETUP
#line 56 "lexer.l"
{ return TOKEN_LEQ;          }
	YY_BREAK
case 22:
YY_RULE_SETUP
#line 57 "lexer.l"
{ return TOKEN_GEQ;          }
	YY_BREAK
case 23:
YY_RULE_SETUP
#line 58 "lexer.l"
{ return TOKEN_AND;          }
	YY_BREAK
case 24:
YY_RULE_SETUP
#line 59 "lexer.l"
{ return TOKEN_OR;           }
	YY_BREAK
case 25:
YY_RULE_SETUP
#line 60 "lexer.l"
{ return TOKEN_PLUS;         }
	YY_BREAK
case 26:
YY_RULE_SETUP
#line 61 "lexer.l"
{ return TOKEN_MINUS;        }
	YY_BREAK
case 27:
YY_RULE_SETUP
#line 62 "lexer.l"
{ return TOKEN_STAR;         }
	YY_BREAK
case 28:
YY_RULE_SETUP
#line 63 "lexer.l"
{ return TOKEN_SLASH;        }
	YY_BREAK
case 29:
YY_RULE_SETUP
#line 64 "lexer.l"
{ return TOKEN_PERCENT;      }
	YY_BREAK
case 30:
YY_RULE_SETUP
#line 65 "lexer.l"
{ return TOKEN_ASSIGN;       }
	YY_BREAK
case 31:
YY_RULE_SETUP
#line 66 "lexer.l"
{ return TOKEN_LT;           }
	YY_BREAK
case 32:
YY_RULE_SETUP
#line 67 "lexer.l"
{ return TOKEN_GT;           }
	YY_BREAK
case 33:
YY_RULE_SETUP
#line 68 "lexer.l"
{ return TOKEN_NOT;          }
	YY_BREAK

case 34:
YY_RULE_SETUP
#line 71 "lexer.l"
{ return TOKEN_LPAREN;    }
	YY_BREAK
case 35:
YY_RULE_SETUP
#line 72 "lexer.l"
{ return TOKEN_RPAREN;    }
	YY_BREAK
case 36:
YY_RULE_SETUP
#line 73 "lexer.l"
{ return TOKEN_LBRACE;    }
	YY_BREAK
case 37:
YY_RULE_SETUP
#line 74 "lexer.l"
{ return TOKEN_RBRACE;    }
	YY_BREAK
case 38:
YY_RULE_SETUP
#line 75 "lexer.l"
{ return TOKEN_LBRACKET;  }
	YY_BREAK
case 39:
YY_RULE_SETUP
#line 76 "lexer.l"
{ return TOKEN_RBRACKET;  }
	YY_BREAK
case 40:
YY_RULE_SETUP
#line 77 "lexer.l"
{ return TOKEN_SEMICOLON; }
	YY_BREAK
case 41:
YY_RULE_SETUP
#line 78 "lexer.l"
{ return TOKEN_COMMA;     }
	YY_BREAK

case 42:
YY_RULE_SETUP
#line 81 "lexer.l"
{
    yylval.fval = atof(yytext);
    return TOKEN_FLOAT_LITERAL;
}
	YY_BREAK
case 43:
YY_RULE_SETUP
#line 86 "lexer.l"
{
    yylval.ival = atoi(yytext);
    return TOKEN_INT_LITERAL;
}
	YY_BREAK
case 44:

YY_RULE_SETUP
#line 91 "lexer.l"
{
    yylval.sval = strdup(yytext);
    return TOKEN_STRING_LITERAL;
}
	YY_BREAK

case 45:
YY_RULE_SETUP
#line 97 "lexer.l"
{
    yylval.sval = strdup(yytext);
    return TOKEN_IDENTIFIER;
}
	YY_BREAK

case 46:
YY_RULE_SETUP
#line 103 "lexer.l"
{  }
	YY_BREAK
case 47:

YY_RULE_SETUP
#line 104 "lexer.l"
{  }
	YY_BREAK

case 48:
YY_RULE_SETUP
#line 107 "lexer.l"
{
    report_lexical_error(yytext, yylineno);
}
	YY_BREAK
case 49:
YY_RULE_SETUP
#line 111 "lexer.l"
ECHO;
	YY_BREAK
#line 1111 "lex.yy.cpp"
case YY_STATE_EOF(INITIAL):
	yyterminate();

	case YY_END_OF_BUFFER:
		{
		
		int yy_amount_of_matched_text = (int) (yy_cp - (yytext_ptr)) - 1;

		
		*yy_cp = (yy_hold_char);
		YY_RESTORE_YY_MORE_OFFSET

		if ( YY_CURRENT_BUFFER_LVALUE->yy_buffer_status == YY_BUFFER_NEW )
			{
			
			(yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_n_chars;
			YY_CURRENT_BUFFER_LVALUE->yy_input_file = yyin;
			YY_CURRENT_BUFFER_LVALUE->yy_buffer_status = YY_BUFFER_NORMAL;
			}

		
		if ( (yy_c_buf_p) <= &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] )
			{ 
			yy_state_type yy_next_state;

			(yy_c_buf_p) = (yytext_ptr) + yy_amount_of_matched_text;

			yy_current_state = yy_get_previous_state(  );

			

			yy_next_state = yy_try_NUL_trans( yy_current_state );

			yy_bp = (yytext_ptr) + YY_MORE_ADJ;

			if ( yy_next_state )
				{
				
				yy_cp = ++(yy_c_buf_p);
				yy_current_state = yy_next_state;
				goto yy_match;
				}

			else
				{
				yy_cp = (yy_c_buf_p);
				goto yy_find_action;
				}
			}

		else switch ( yy_get_next_buffer(  ) )
			{
			case EOB_ACT_END_OF_FILE:
				{
				(yy_did_buffer_switch_on_eof) = 0;

				if ( yywrap(  ) )
					{
					
					(yy_c_buf_p) = (yytext_ptr) + YY_MORE_ADJ;

					yy_act = YY_STATE_EOF(YY_START);
					goto do_action;
					}

				else
					{
					if ( ! (yy_did_buffer_switch_on_eof) )
						YY_NEW_FILE;
					}
				break;
				}

			case EOB_ACT_CONTINUE_SCAN:
				(yy_c_buf_p) =
					(yytext_ptr) + yy_amount_of_matched_text;

				yy_current_state = yy_get_previous_state(  );

				yy_cp = (yy_c_buf_p);
				yy_bp = (yytext_ptr) + YY_MORE_ADJ;
				goto yy_match;

			case EOB_ACT_LAST_MATCH:
				(yy_c_buf_p) =
				&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)];

				yy_current_state = yy_get_previous_state(  );

				yy_cp = (yy_c_buf_p);
				yy_bp = (yytext_ptr) + YY_MORE_ADJ;
				goto yy_find_action;
			}
		break;
		}

	default:
		YY_FATAL_ERROR(
			"fatal flex scanner internal error--no action found" );
	} 
		} 
	} 
} 


static int yy_get_next_buffer (void)
{
    	char *dest = YY_CURRENT_BUFFER_LVALUE->yy_ch_buf;
	char *source = (yytext_ptr);
	int number_to_move, i;
	int ret_val;

	if ( (yy_c_buf_p) > &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars) + 1] )
		YY_FATAL_ERROR(
		"fatal flex scanner internal error--end of buffer missed" );

	if ( YY_CURRENT_BUFFER_LVALUE->yy_fill_buffer == 0 )
		{ 
		if ( (yy_c_buf_p) - (yytext_ptr) - YY_MORE_ADJ == 1 )
			{
			
			return EOB_ACT_END_OF_FILE;
			}

		else
			{
			
			return EOB_ACT_LAST_MATCH;
			}
		}

	

	
	number_to_move = (int) ((yy_c_buf_p) - (yytext_ptr) - 1);

	for ( i = 0; i < number_to_move; ++i )
		*(dest++) = *(source++);

	if ( YY_CURRENT_BUFFER_LVALUE->yy_buffer_status == YY_BUFFER_EOF_PENDING )
		
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars) = 0;

	else
		{
			int num_to_read =
			YY_CURRENT_BUFFER_LVALUE->yy_buf_size - number_to_move - 1;

		while ( num_to_read <= 0 )
			{ 

			
			YY_BUFFER_STATE b = YY_CURRENT_BUFFER_LVALUE;

			int yy_c_buf_p_offset =
				(int) ((yy_c_buf_p) - b->yy_ch_buf);

			if ( b->yy_is_our_buffer )
				{
				int new_size = b->yy_buf_size * 2;

				if ( new_size <= 0 )
					b->yy_buf_size += b->yy_buf_size / 8;
				else
					b->yy_buf_size *= 2;

				b->yy_ch_buf = (char *)
					
					yyrealloc( (void *) b->yy_ch_buf,
							 (yy_size_t) (b->yy_buf_size + 2)  );
				}
			else
				
				b->yy_ch_buf = NULL;

			if ( ! b->yy_ch_buf )
				YY_FATAL_ERROR(
				"fatal error - scanner input buffer overflow" );

			(yy_c_buf_p) = &b->yy_ch_buf[yy_c_buf_p_offset];

			num_to_read = YY_CURRENT_BUFFER_LVALUE->yy_buf_size -
						number_to_move - 1;

			}

		if ( num_to_read > YY_READ_BUF_SIZE )
			num_to_read = YY_READ_BUF_SIZE;

		
		YY_INPUT( (&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[number_to_move]),
			(yy_n_chars), num_to_read );

		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
		}

	if ( (yy_n_chars) == 0 )
		{
		if ( number_to_move == YY_MORE_ADJ )
			{
			ret_val = EOB_ACT_END_OF_FILE;
			yyrestart( yyin  );
			}

		else
			{
			ret_val = EOB_ACT_LAST_MATCH;
			YY_CURRENT_BUFFER_LVALUE->yy_buffer_status =
				YY_BUFFER_EOF_PENDING;
			}
		}

	else
		ret_val = EOB_ACT_CONTINUE_SCAN;

	if (((yy_n_chars) + number_to_move) > YY_CURRENT_BUFFER_LVALUE->yy_buf_size) {
		
		int new_size = (yy_n_chars) + number_to_move + ((yy_n_chars) >> 1);
		YY_CURRENT_BUFFER_LVALUE->yy_ch_buf = (char *) yyrealloc(
			(void *) YY_CURRENT_BUFFER_LVALUE->yy_ch_buf, (yy_size_t) new_size  );
		if ( ! YY_CURRENT_BUFFER_LVALUE->yy_ch_buf )
			YY_FATAL_ERROR( "out of dynamic memory in yy_get_next_buffer()" );
		
		YY_CURRENT_BUFFER_LVALUE->yy_buf_size = (int) (new_size - 2);
	}

	(yy_n_chars) += number_to_move;
	YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] = YY_END_OF_BUFFER_CHAR;
	YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars) + 1] = YY_END_OF_BUFFER_CHAR;

	(yytext_ptr) = &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[0];

	return ret_val;
}



    static yy_state_type yy_get_previous_state (void)
{
	yy_state_type yy_current_state;
	char *yy_cp;
    
	yy_current_state = (yy_start);

	for ( yy_cp = (yytext_ptr) + YY_MORE_ADJ; yy_cp < (yy_c_buf_p); ++yy_cp )
		{
		YY_CHAR yy_c = (*yy_cp ? yy_ec[YY_SC_TO_UI(*yy_cp)] : 1);
		if ( yy_accept[yy_current_state] )
			{
			(yy_last_accepting_state) = yy_current_state;
			(yy_last_accepting_cpos) = yy_cp;
			}
		while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
			{
			yy_current_state = (int) yy_def[yy_current_state];
			if ( yy_current_state >= 98 )
				yy_c = yy_meta[yy_c];
			}
		yy_current_state = yy_nxt[yy_base[yy_current_state] + yy_c];
		}

	return yy_current_state;
}


    static yy_state_type yy_try_NUL_trans  (yy_state_type yy_current_state )
{
	int yy_is_jam;
    	char *yy_cp = (yy_c_buf_p);

	YY_CHAR yy_c = 1;
	if ( yy_accept[yy_current_state] )
		{
		(yy_last_accepting_state) = yy_current_state;
		(yy_last_accepting_cpos) = yy_cp;
		}
	while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
		{
		yy_current_state = (int) yy_def[yy_current_state];
		if ( yy_current_state >= 98 )
			yy_c = yy_meta[yy_c];
		}
	yy_current_state = yy_nxt[yy_base[yy_current_state] + yy_c];
	yy_is_jam = (yy_current_state == 97);

		return yy_is_jam ? 0 : yy_current_state;
}

#ifndef YY_NO_UNPUT

    static void yyunput (int c, char * yy_bp )
{
	char *yy_cp;
    
    yy_cp = (yy_c_buf_p);

	
	*yy_cp = (yy_hold_char);

	if ( yy_cp < YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + 2 )
		{ 
		
		int number_to_move = (yy_n_chars) + 2;
		char *dest = &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[
					YY_CURRENT_BUFFER_LVALUE->yy_buf_size + 2];
		char *source =
				&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[number_to_move];

		while ( source > YY_CURRENT_BUFFER_LVALUE->yy_ch_buf )
			*--dest = *--source;

		yy_cp += (int) (dest - source);
		yy_bp += (int) (dest - source);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars =
			(yy_n_chars) = (int) YY_CURRENT_BUFFER_LVALUE->yy_buf_size;

		if ( yy_cp < YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + 2 )
			YY_FATAL_ERROR( "flex scanner push-back overflow" );
		}

	*--yy_cp = (char) c;

    if ( c == '\n' ){
        --yylineno;
    }

	(yytext_ptr) = yy_bp;
	(yy_hold_char) = *yy_cp;
	(yy_c_buf_p) = yy_cp;
}

#endif

#ifndef YY_NO_INPUT
#ifdef __cplusplus
    static int yyinput (void)
#else
    static int input  (void)
#endif

{
	int c;
    
	*(yy_c_buf_p) = (yy_hold_char);

	if ( *(yy_c_buf_p) == YY_END_OF_BUFFER_CHAR )
		{
		
		if ( (yy_c_buf_p) < &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] )
			
			*(yy_c_buf_p) = '\0';

		else
			{ 
			int offset = (int) ((yy_c_buf_p) - (yytext_ptr));
			++(yy_c_buf_p);

			switch ( yy_get_next_buffer(  ) )
				{
				case EOB_ACT_LAST_MATCH:
					

					
					yyrestart( yyin );

					

				case EOB_ACT_END_OF_FILE:
					{
					if ( yywrap(  ) )
						return 0;

					if ( ! (yy_did_buffer_switch_on_eof) )
						YY_NEW_FILE;
#ifdef __cplusplus
					return yyinput();
#else
					return input();
#endif
					}

				case EOB_ACT_CONTINUE_SCAN:
					(yy_c_buf_p) = (yytext_ptr) + offset;
					break;
				}
			}
		}

	c = *(unsigned char *) (yy_c_buf_p);	
	*(yy_c_buf_p) = '\0';	
	(yy_hold_char) = *++(yy_c_buf_p);

	if ( c == '\n' )
		
    yylineno++;
;

	return c;
}
#endif	


    void yyrestart  (FILE * input_file )
{
    
	if ( ! YY_CURRENT_BUFFER ){
        yyensure_buffer_stack ();
		YY_CURRENT_BUFFER_LVALUE =
            yy_create_buffer( yyin, YY_BUF_SIZE );
	}

	yy_init_buffer( YY_CURRENT_BUFFER, input_file );
	yy_load_buffer_state(  );
}


    void yy_switch_to_buffer  (YY_BUFFER_STATE  new_buffer )
{
    
	
	yyensure_buffer_stack ();
	if ( YY_CURRENT_BUFFER == new_buffer )
		return;

	if ( YY_CURRENT_BUFFER )
		{
		
		*(yy_c_buf_p) = (yy_hold_char);
		YY_CURRENT_BUFFER_LVALUE->yy_buf_pos = (yy_c_buf_p);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
		}

	YY_CURRENT_BUFFER_LVALUE = new_buffer;
	yy_load_buffer_state(  );

	
	(yy_did_buffer_switch_on_eof) = 1;
}

static void yy_load_buffer_state  (void)
{
    	(yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_n_chars;
	(yytext_ptr) = (yy_c_buf_p) = YY_CURRENT_BUFFER_LVALUE->yy_buf_pos;
	yyin = YY_CURRENT_BUFFER_LVALUE->yy_input_file;
	(yy_hold_char) = *(yy_c_buf_p);
}


    YY_BUFFER_STATE yy_create_buffer  (FILE * file, int  size )
{
	YY_BUFFER_STATE b;
    
	b = (YY_BUFFER_STATE) yyalloc( sizeof( struct yy_buffer_state )  );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

	b->yy_buf_size = size;

	
	b->yy_ch_buf = (char *) yyalloc( (yy_size_t) (b->yy_buf_size + 2)  );
	if ( ! b->yy_ch_buf )
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

	b->yy_is_our_buffer = 1;

	yy_init_buffer( b, file );

	return b;
}


    void yy_delete_buffer (YY_BUFFER_STATE  b )
{
    
	if ( ! b )
		return;

	if ( b == YY_CURRENT_BUFFER ) 
		YY_CURRENT_BUFFER_LVALUE = (YY_BUFFER_STATE) 0;

	if ( b->yy_is_our_buffer )
		yyfree( (void *) b->yy_ch_buf  );

	yyfree( (void *) b  );
}


    static void yy_init_buffer  (YY_BUFFER_STATE  b, FILE * file )

{
	int oerrno = errno;
    
	yy_flush_buffer( b );

	b->yy_input_file = file;
	b->yy_fill_buffer = 1;

    
    if (b != YY_CURRENT_BUFFER){
        b->yy_bs_lineno = 1;
        b->yy_bs_column = 0;
    }

        b->yy_is_interactive = file ? (isatty( fileno(file) ) > 0) : 0;
    
	errno = oerrno;
}


    void yy_flush_buffer (YY_BUFFER_STATE  b )
{
    	if ( ! b )
		return;

	b->yy_n_chars = 0;

	
	b->yy_ch_buf[0] = YY_END_OF_BUFFER_CHAR;
	b->yy_ch_buf[1] = YY_END_OF_BUFFER_CHAR;

	b->yy_buf_pos = &b->yy_ch_buf[0];

	b->yy_at_bol = 1;
	b->yy_buffer_status = YY_BUFFER_NEW;

	if ( b == YY_CURRENT_BUFFER )
		yy_load_buffer_state(  );
}


void yypush_buffer_state (YY_BUFFER_STATE new_buffer )
{
    	if (new_buffer == NULL)
		return;

	yyensure_buffer_stack();

	
	if ( YY_CURRENT_BUFFER )
		{
		
		*(yy_c_buf_p) = (yy_hold_char);
		YY_CURRENT_BUFFER_LVALUE->yy_buf_pos = (yy_c_buf_p);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
		}

	
	if (YY_CURRENT_BUFFER)
		(yy_buffer_stack_top)++;
	YY_CURRENT_BUFFER_LVALUE = new_buffer;

	
	yy_load_buffer_state(  );
	(yy_did_buffer_switch_on_eof) = 1;
}


void yypop_buffer_state (void)
{
    	if (!YY_CURRENT_BUFFER)
		return;

	yy_delete_buffer(YY_CURRENT_BUFFER );
	YY_CURRENT_BUFFER_LVALUE = NULL;
	if ((yy_buffer_stack_top) > 0)
		--(yy_buffer_stack_top);

	if (YY_CURRENT_BUFFER) {
		yy_load_buffer_state(  );
		(yy_did_buffer_switch_on_eof) = 1;
	}
}


static void yyensure_buffer_stack (void)
{
	yy_size_t num_to_alloc;
    
	if (!(yy_buffer_stack)) {

		
      num_to_alloc = 1; 
		(yy_buffer_stack) = (struct yy_buffer_state**)yyalloc
								(num_to_alloc * sizeof(struct yy_buffer_state*)
								);
		if ( ! (yy_buffer_stack) )
			YY_FATAL_ERROR( "out of dynamic memory in yyensure_buffer_stack()" );

		memset((yy_buffer_stack), 0, num_to_alloc * sizeof(struct yy_buffer_state*));

		(yy_buffer_stack_max) = num_to_alloc;
		(yy_buffer_stack_top) = 0;
		return;
	}

	if ((yy_buffer_stack_top) >= ((yy_buffer_stack_max)) - 1){

		
		yy_size_t grow_size = 8 ;

		num_to_alloc = (yy_buffer_stack_max) + grow_size;
		(yy_buffer_stack) = (struct yy_buffer_state**)yyrealloc
								((yy_buffer_stack),
								num_to_alloc * sizeof(struct yy_buffer_state*)
								);
		if ( ! (yy_buffer_stack) )
			YY_FATAL_ERROR( "out of dynamic memory in yyensure_buffer_stack()" );

		
		memset((yy_buffer_stack) + (yy_buffer_stack_max), 0, grow_size * sizeof(struct yy_buffer_state*));
		(yy_buffer_stack_max) = num_to_alloc;
	}
}


YY_BUFFER_STATE yy_scan_buffer  (char * base, yy_size_t  size )
{
	YY_BUFFER_STATE b;
    
	if ( size < 2 ||
	     base[size-2] != YY_END_OF_BUFFER_CHAR ||
	     base[size-1] != YY_END_OF_BUFFER_CHAR )
		
		return NULL;

	b = (YY_BUFFER_STATE) yyalloc( sizeof( struct yy_buffer_state )  );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_buffer()" );

	b->yy_buf_size = (int) (size - 2);	
	b->yy_buf_pos = b->yy_ch_buf = base;
	b->yy_is_our_buffer = 0;
	b->yy_input_file = NULL;
	b->yy_n_chars = b->yy_buf_size;
	b->yy_is_interactive = 0;
	b->yy_at_bol = 1;
	b->yy_fill_buffer = 0;
	b->yy_buffer_status = YY_BUFFER_NEW;

	yy_switch_to_buffer( b  );

	return b;
}


YY_BUFFER_STATE yy_scan_string (const char * yystr )
{
    
	return yy_scan_bytes( yystr, (int) strlen(yystr) );
}


YY_BUFFER_STATE yy_scan_bytes  (const char * yybytes, int  _yybytes_len )
{
	YY_BUFFER_STATE b;
	char *buf;
	yy_size_t n;
	int i;
    
	
	n = (yy_size_t) (_yybytes_len + 2);
	buf = (char *) yyalloc( n  );
	if ( ! buf )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_bytes()" );

	for ( i = 0; i < _yybytes_len; ++i )
		buf[i] = yybytes[i];

	buf[_yybytes_len] = buf[_yybytes_len+1] = YY_END_OF_BUFFER_CHAR;

	b = yy_scan_buffer( buf, n );
	if ( ! b )
		YY_FATAL_ERROR( "bad buffer in yy_scan_bytes()" );

	
	b->yy_is_our_buffer = 1;

	return b;
}

#ifndef YY_EXIT_FAILURE
#define YY_EXIT_FAILURE 2
#endif

static void yynoreturn yy_fatal_error (const char* msg )
{
			fprintf( stderr, "%s\n", msg );
	exit( YY_EXIT_FAILURE );
}



#undef yyless
#define yyless(n) \
	do \
		{ \
		 \
        int yyless_macro_arg = (n); \
        YY_LESS_LINENO(yyless_macro_arg);\
		yytext[yyleng] = (yy_hold_char); \
		(yy_c_buf_p) = yytext + yyless_macro_arg; \
		(yy_hold_char) = *(yy_c_buf_p); \
		*(yy_c_buf_p) = '\0'; \
		yyleng = yyless_macro_arg; \
		} \
	while ( 0 )




int yyget_lineno  (void)
{
    
    return yylineno;
}


FILE *yyget_in  (void)
{
        return yyin;
}


FILE *yyget_out  (void)
{
        return yyout;
}


int yyget_leng  (void)
{
        return yyleng;
}



char *yyget_text  (void)
{
        return yytext;
}


void yyset_lineno (int  _line_number )
{
    
    yylineno = _line_number;
}


void yyset_in (FILE *  _in_str )
{
        yyin = _in_str ;
}

void yyset_out (FILE *  _out_str )
{
        yyout = _out_str ;
}

int yyget_debug  (void)
{
        return yy_flex_debug;
}

void yyset_debug (int  _bdebug )
{
        yy_flex_debug = _bdebug ;
}

static int yy_init_globals (void)
{
        

    
    yylineno =  1;
    
    (yy_buffer_stack) = NULL;
    (yy_buffer_stack_top) = 0;
    (yy_buffer_stack_max) = 0;
    (yy_c_buf_p) = NULL;
    (yy_init) = 0;
    (yy_start) = 0;


#ifdef YY_STDINIT
    yyin = stdin;
    yyout = stdout;
#else
    yyin = NULL;
    yyout = NULL;
#endif

    
    return 0;
}


int yylex_destroy  (void)
{
    
    
	while(YY_CURRENT_BUFFER){
		yy_delete_buffer( YY_CURRENT_BUFFER  );
		YY_CURRENT_BUFFER_LVALUE = NULL;
		yypop_buffer_state();
	}

	
	yyfree((yy_buffer_stack) );
	(yy_buffer_stack) = NULL;

    
    yy_init_globals( );

    return 0;
}



#ifndef yytext_ptr
static void yy_flex_strncpy (char* s1, const char * s2, int n )
{
		
	int i;
	for ( i = 0; i < n; ++i )
		s1[i] = s2[i];
}
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen (const char * s )
{
	int n;
	for ( n = 0; s[n]; ++n )
		;

	return n;
}
#endif

void *yyalloc (yy_size_t  size )
{
			return malloc(size);
}

void *yyrealloc  (void * ptr, yy_size_t  size )
{
		
	
	return realloc(ptr, size);
}

void yyfree (void * ptr )
{
			free( (char *) ptr );	
}

#define YYTABLES_NAME "yytables"

#line 111 "lexer.l"




void report_lexical_error(const char* text, int line) {
    std::cerr << "Lexical Error (line " << line
              << "): unrecognized character '" << text << "'" << std::endl;
}

