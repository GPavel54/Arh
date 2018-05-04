#include "addFunctions.h"
#include <stdint.h>
#include "mySimpleComputer.h"
#include <stdlib.h>
#include <math.h>

extern struct memCell current;
extern int accumulator; 
extern uint16_t instructionCounter;
extern uint8_t flags;	

void printInterface(){
	char keys[7][12] = {"load", "save", "run", "step", "reset", "accumulator", "instrCount"};
	char dkeys[6] = "lsrti";
	/// Этот блок печатает рамку и внутри нее состояние памяти
	bc_box(1, 1, 62, 12);
	mt_gotoXY(5, 1);
	printf(" Memory ");
	mt_gotoXY(2, 2);
	int y = 0;
	int * tmp;
	tmp = (int *)malloc(sizeof(int *));
	for (int i = 0; i < 10; i++){
		for (int j = 0; j < 10; j++){		
			sc_memoryGet(y, tmp);
			if (*tmp * -1 > 0)
				printf("-%04d ", abs(*tmp));
			else
				printf("+%04d ", *tmp);
			y++;
		}
		mt_gotoXY(2 ,2 + i + 1);
	}
	//Конец блока памяти
	//Блок с аккумулятором, регистр флагов, счетчик команд
	bc_box(63, 1, 79, 3);
	mt_gotoXY(65, 1);
	printf(" accumulator ");
	paintAcc(0);
	bc_box(63, 4, 79, 6);
	mt_gotoXY(64, 4);
	printf(" instrCounter ");
	paintInst(0);
	bc_box(63, 7, 79, 9);
	mt_gotoXY(67, 7);
	printf(" Flags ");
	printReg();
	bc_box(63, 10, 79, 12);
	mt_gotoXY(66, 10);
	printf(" Operation ");
	//
	//Блок с большими символами
	printCellBig();
	//
	//Блок с назначением клавиш
	bc_box(43, 13, 79, 22);
	mt_gotoXY(45, 13);
	printf(" Keys: ");
	for (int i = 0; i < 7; i++){
		char buf[3] = "F5";
		if (i == 6){	
			buf[0] = 'F';		
			buf[1] = '6';		
			buf[2] = '\0';
		}
		mt_gotoXY(44, 14 + i);
		if (i < 5)
			printf("%c   - %s", dkeys[i], keys[i]);
		else 	
			printf("%s  - %s", buf, keys[i]);
	}
	//
	mt_gotoXY(1, 23);
}


void printCellBig(){
	bc_box(1, 13, 42, 22);
	int buf;
	sc_memoryGet(current.pointer, &buf);
	if (buf >= 0)
		bc_printbigchar('P', 2, 14, cl_green, cl_default);
	else {
		bc_printbigchar('M', 2, 14, cl_green, cl_default);
		buf *= -1;
	}	
	char ints[4];
	sprintf(ints, "%04d", buf);
	int x = 10;	
	for (int i = 0; i < 4; i++){
		bc_printbigchar(ints[i], x, 14, cl_green, cl_default);
		x += 8;	
	}
}

void paintCell(int x, int y, int pointer, enum colors col){		
	mt_gotoXY(x*6 + 2, y + 2);
	mt_setbgcolor(col);
	int value;
	sc_memoryGet(pointer, &value);
	if (value > -1)
		printf("+%04d ", value);
	else {
		value *= -1;
		printf("-%04d ", value);
	}
	mt_setbgcolor(cl_default);
	printCellBig();
}
int changePoint(int value){
	struct memCell prev = current;	
	if (current.pointer + value < 100 && current.pointer + value >= 0){
		current.pointer += value;		
	}
	int toY;
	toY = current.pointer / 10;
	value = current.pointer % 10;
	current.x = value;
	current.y = toY;
	paintCell(prev.x, prev.y, prev.pointer, cl_default);
	paintCell(current.x, current.y, current.pointer, cl_red);
	instructionCounter = current.pointer;
	paintInst(0);
}
void paintAcc(int y){
	mt_gotoXY(64, 2);
	if (y == 0)
		mt_setbgcolor(cl_default);	
	else
		mt_setbgcolor(cl_red);
	if (accumulator > -1)
		printf("     +%04d     ", accumulator);
	else {		
		accumulator *= -1;
		printf("     -%04d     ", accumulator);
		accumulator *= -1;
	}
	mt_setbgcolor(cl_default);
}
void paintInst(int y){
	mt_gotoXY(64, 5);
	if (y == 0)
		mt_setbgcolor(cl_default);	
	else
		mt_setbgcolor(cl_red);
	printf("     %04d     ", instructionCounter);
	mt_setbgcolor(cl_default);
}

void printReg(){
	char out[6] = "";
	int y = 0;
	sc_regGet(O, &y);
	if (y == 1)
		out[0] = 'M';
	sc_regGet(P, &y);
	if (y == 1)
		out[1] = 'P';
	sc_regGet(M, &y);
	if (y == 1)
		out[2] = 'M';
	sc_regGet(T, &y);
	if (y == 1)
		out[3] = 'T';
	sc_regGet(E, &y);
	if (y == 1)
		out[4] = 'E';
	out[5] = '\0';
	mt_gotoXY(67, 8);
	printf("%s", out);
	mt_gotoXY(1, 23);
}
