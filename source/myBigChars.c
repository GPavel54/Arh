#include "myBigChars.h"

int bc_printA(char *str)
{
	printf("\E(0%s\E(B", str);
	return 0;
}

int bc_box(int x1, int y1, int x2, int y2)
{
	int tmp;
	int maxx, maxy;
	int i;
	
	if (x1 > x2)
	{
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2)
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	mt_getscreensize(&maxy, &maxx);
	if ((x1 < 0) || (y1 < 0) || (x2 > maxx) || (y2 > maxy) || (x2 - x1 < 2) || (y2 - y1 < 2)){
		printf("max x = %d, max y = %d\n", maxx, maxy);
		return -1;
	}
	mt_gotoXY(x1, y1);
	bc_printA(BOXCHAR_TL);
	for (i = x1 + 1; i < x2; i++)
		bc_printA(BOXCHAR_HOR);
	bc_printA(BOXCHAR_TR);
	for (i = y1 +1; i < y2; i++)
	{
		mt_gotoXY(x1, i);
		bc_printA(BOXCHAR_VERT);
		mt_gotoXY(x2, i);
		//printf(" \E(B");
		bc_printA(BOXCHAR_VERT);
	}
	mt_gotoXY(x1, y2);
	bc_printA(BOXCHAR_BL);
	for (i = x1 + 1; i < x2; i++)
		bc_printA(BOXCHAR_HOR);
	bc_printA(BOXCHAR_BR);
	return 0;
}

int bc_printbigchar(char t, int x, int y, enum colors fg, enum colors bg)
{
	int maxx, maxy;
	int pos, bit;
	int i, j;
	char row[9];
	int big[2];
	char way[12] = "data/int";
	t = toupper(t);
	if (((t >= '0') && (t <= '9')) || ((t >= 'A') && (t <= 'F')) || t == 'P' || t == 'M'){
		way[8] = t;
		way[9] = '.';
		way[10] = 'b';
		way[11] = '\0';
	}
	else
		return -1;
	bc_bigcharread(way, big, 2);	
	
	
	mt_getscreensize(&maxy, &maxx);
	if ((x < 0) || (y < 0) || (x + 8 > maxx) || (y + 8 > maxy))
		return -1;
	row[8] = '\0';
	mt_setfgcolor(fg);
	mt_setbgcolor(bg);
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			pos = i >> 2;
			bit = (big[pos] >> ((i % 4) * 8 + j)) & 1;
			if (bit == 0)
				row[j] = ' ';
			else
				row[j] = BOXCHAR_REC;
		}
		mt_gotoXY(x, y + i);
		bc_printA(row);
	}
	mt_setfgcolor(cl_default);
	mt_setbgcolor(cl_default);
	return 0;
}

int bc_setbigcharpos(int *big, int x, int y, int value)
{
	int pos;

	if ((x < 0) || (y < 0)  || (x > 7) || (y > 7) || (value < 0) || (value > 1))
		return -1;
	if (y <3)
		pos = 0;
	else
		pos = 1;
	y = y % 4;
	if (value == 0)
		big[pos] &= ~(1 << (8 * y + x));
	else
		big[pos] |= 1 << (8 * y + x);
	
	return 0;
}

int bc_bigcharwrite(char * way, char * flag, int *big, int count)
{
	FILE *fp;
	fp = fopen(way, flag);
	if (fp == NULL)
		return -1;
	fwrite(big, sizeof(int), count, fp);
	fclose(fp);	
	return 0;
}

int bc_bigcharread(char * way, int *big, int count)
{
	FILE * fp;
	fp = fopen(way, "rb");
	if (fp == NULL)
		return -1;
	fread(big, sizeof(int), count, fp);	
	fclose(fp);	
	return 0;
}
