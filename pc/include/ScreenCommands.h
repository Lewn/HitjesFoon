
#ifndef SCREEN_COMMANDS_H
#define SCREEN_COMMANDS_H


#define resetColors()   setColors(WHITE, BLACK);

#ifdef _WIN32

#include <windows.h>

#define clearScreen()   system("CLS");

#define FOREGROUND_WHITE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define FOREGROUND_BLACK 0

#define BACKGROUND_WHITE BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE
#define BACKGROUND_BLACK 0

#define STRCAT(a, b) a ## b
#define setColors(fg, bg) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), STRCAT(FOREGROUND_, fg) | STRCAT(BACKGROUND_, bg));

#else

#include <stdio.h>

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#define BLACK    0
#define RED      1
#define GREEN    2
#define YELLOW   3
#define BLUE     4
#define MAGENTA  5
#define CYAN     6
#define WHITE    7
#define DEFAULT  9

#define clearScreen  "\33[2J"
#define clear_to_bos "\33[1J"
#define clear_to_eos "\33[J"

#define clear_line   "\33[2K"
#define clear_to_bol "\33[1K"
#define clear_to_eol "\33[K"

#define visual_bell  "\33g"
#define audible_bell "\a"

#define show_cursor( v ) ((v) ? "\33\67p" : "\33\66p")

#define goto_xy( x, y ) "\33[" << y << ";" << x << "H"
#define move_to( y, x ) "\33[" << y << ";" << x << "f"

#define cursor_up(    count ) "\33[" << count << "A"
#define cursor_down(  count ) "\33[" << count << "B"
#define cursor_right( count ) "\33[" << count << "C"
#define cursor_left(  count ) "\33[" << count << "D"

#define set_scrolling_region( top, bottom ) "\33[" << top << ";" << bottom << "r"

#define scroll_up(   count ) "\33[" << count << "S"
#define scroll_down( count ) "\33[" << count << "T"

#define insert_line( count ) "\33[" << count << "L"
#define delete_line( count ) "\33[" << count << "M"

#define insert_char( count ) "\33[" << count << "@"
#define delete_char( count ) "\33[" << count << "P"

static bool is_b = FALSE;
static bool is_i = FALSE;
static bool is_u = FALSE;
static bool is_r = FALSE;
static int  fg_c = DEFAULT;
static int  bg_c = DEFAULT;

#define default_attributes "\33[0m"

// these four following are not meant to be used externally
#define set_b( b ) ((is_b = b) ? "\33[1m" : "")
#define set_i( i ) ((is_i = i) ? "\33[3m" : "")
#define set_u( u ) ((is_u = u) ? "\33[4m" : "")
#define set_r( r ) ((is_r = r) ? "\33[7m" : "")

#define setColors( fg, bg )     do { \
                                    printf("\33[3%d;4%dm", fg_c = fg, bg_c = bg); \
                                }while(0);


#define set_attributes( b, i, u, r ) set_b( b ) << set_i( i ) << set_u( u ) << set_r( r ) << set_colors( fg_c, bg_c )

#define set_bold(      b ) default_attributes << set_attributes( b, is_i, is_u, is_r )
#define set_italic(    i ) default_attributes << set_attributes( is_b, i, is_u, is_r )
#define set_underline( u ) default_attributes << set_attributes( is_b, is_i, u, is_r )
#define set_reverse(   r ) default_attributes << set_attributes( is_b, is_i, is_u, r )

#define finalize() printf(default_attributes);

#endif

#endif  // SCREEN_COMMANDS_H
