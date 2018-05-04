#ifndef TERMINAL_H
#define TERMINAL_H

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

enum colors {
	cl_black,
	cl_red,
	cl_green,
	cl_brown,
	cl_blue,
	cl_magenta,
	cl_cyan,
	cl_light_blue,
	cl_default
};

void mt_clrscr ();								
int mt_gotoXY (int x, int y);					
int mt_getscreensize (int *rows, int *cols);	
int mt_setfgcolor (enum colors color);			
int mt_setbgcolor (enum colors color);	

#endif
