#ifndef BIGCHARS_H
#define BIGCHARS_H

#include "terminal.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define BOXCHAR_REC 'a'
#define BOXCHAR_BR "j"
#define BOXCHAR_BL "m"
#define BOXCHAR_TR "k"
#define BOXCHAR_TL "l"
#define BOXCHAR_VERT "x"
#define BOXCHAR_HOR "q"

int bc_printA(char *str);
int bc_box(int x1, int y1, int x2, int y2);
int bc_printbigchar(char t, int x, int y, enum colors fg, enum colors bg);
int bc_setbigcharpos(int *big, int x, int y, int value);
int bc_bigcharwrite(char * way, char * flag, int *big, int count);
int bc_bigcharread(char * way, int *big, int count);

#endif
