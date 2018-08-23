#include "addFunctions.h"
#include <stdint.h>
#include "mySimpleComputer.h"
#include <stdlib.h>

extern struct memCell current;
extern int accumulator; 
extern uint16_t instructionCounter;
extern uint8_t flags;	
extern int needIncrement;

void printInterface(){
	char keys[7][12] = {"load", "save", "run", "step", "reset", "accumulator", "instrCount"};
	char dkeys[6] = "lsrti";
	/// Этот блок печатает рамку и внутри нее состояние памяти
	bc_box(1, 1, 62, 12);
	mt_gotoXY(5, 1);
	printf(" Memory ");
	mt_gotoXY(2, 2);
	int y = 0;
	int tmp;
	for (int i = 0; i < 10; i++){
		for (int j = 0; j < 10; j++){		
			sc_memoryGet(y, &tmp);
			if ((tmp & 16384) == 0)
				printf("+%04x ", tmp);
			else if ((tmp & 8192) == 0){
				printf(" %04x ", tmp & 0x3fff);
			}else{
				tmp = ~tmp & 0x3fff; 
				tmp++;
				printf("-%04x ", tmp);
			}
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
	printReg();	
	mt_gotoXY(1, 23);
}


void printCellBig(){
	bc_box(1, 13, 42, 22);
	int buf;
	sc_memoryGet(current.pointer, &buf);
	if ((buf & 16384) == 0)
		bc_printbigchar('P', 2, 14, cl_green, cl_default);
	else if ((buf & 8192) == 0) {
		for (int i = 0; i < 8; i++){
			mt_gotoXY(2, 14 + i);
			printf("        ");
		}
		buf &= 0x3fff;
	}else{
		buf = ~buf & 0x3fff; 
		buf++;
		bc_printbigchar('M', 2, 14, cl_green, cl_default);
	}
	char ints[4];
	sprintf(ints, "%04x", buf);
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
	if ((value & 16384) == 0)
		printf("+%04x ", value);
	else if ((value & 8192) == 0){
		printf(" %04x ", value & 0x3fff);
	} else {
		int vall = ~value & 0x3fff; 
		vall++;
		printf("-%04x ", vall);
	}
	mt_setbgcolor(cl_default);
	printCellBig();
	if (col == cl_red){
		int notAcom = 0;
		int com = 0, opr = 0;		
		notAcom = sc_commandDecode(value, &com, &opr);
		if (notAcom == 0)
			printOper(com, opr);
		else
			printOper(-1, -1);
	}
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
	if ((accumulator & 16384) == 0)
		printf("     +%04x     ", accumulator);
	else if ((accumulator & 8192) == 0){
		printf("     %04x     ", accumulator & 0x3fff);
	} else {
		int vall = ~accumulator & 0x3fff; 
		vall++;
		printf("    -%04x     ", vall);
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
	char out[6];
	for (int i = 0; i < 6; i++)
		out[i] = ' ';
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

int ALU (int command, int operand){
	if (command == 30){  //ADD
		int val = 0;
		int err = sc_memoryGet(operand, &val);
		if (err != 0)
			return -1;
		if ((val & 16384 != 0) && (accumulator & 16384 != 0)){
			accumulator &= 16383;
			val &= 16383;
			accumulator += val;
			ISOVERFLOW
			accumulator &= 0x3fff;	
			accumulator |= 16384;	
			paintAcc(0);
			return 0;
		}
		accumulator += val;
		ISOVERFLOW
		accumulator &= 0x7fff;	
		paintAcc(0);
		return 0;
	}
	if (command == 31){  //SUB
		int val = 0;
		int err = sc_memoryGet(operand, &val);
		if (err != 0)
			return -1;
		if ((val & 0x4000) != 0 && (accumulator & 0x4000) != 0){		
			if ((val & 0x2000) != 0){
				accumulator += (~val & 0x3fff) + 1;
				ISOVERFLOW
				accumulator &= 0x3fff;
				accumulator |= 16384;					
				paintAcc(0);
				return 0;
			} else if ((val & 0x2000) == 0){
				val &= 0x3fff;
				accumulator &= 0x3fff;
				accumulator -= val;
				ISOVERFLOW
				accumulator |= 16384; // включить бит не команда					
				paintAcc(0);
				return 0;
			}
		}
		accumulator += (~val & 0x7fff) + 1;
		ISOVERFLOW		
		paintAcc(0);
		return 0;
	}
	if (command == 32){  //DIVIDE
		int val = 0;
		int err = sc_memoryGet(operand, &val);
		if (err != 0)
			return -1;
		if ((val & 0x4000) != 0 && (accumulator & 0x4000) != 0){ //проверка, если оба значения - целочисленные типы
			if ((val & 0x2000) == 0 && (accumulator & 0x2000) == 0){
				val &= 0x3fff;  // выключить первый бит(не команда)
				accumulator &= 0x3fff; // выключить первый бит(не команда)
				accumulator /= val;
				ISOVERFLOW
				accumulator &= 0x7fff; // удалить лишние биты, если таковые появились
				accumulator |= 0x4000; // вернуть бит, обозначающий не команда					
				paintAcc(0);
				return 0;
			} else if ((val & 0x2000) != 0 && (accumulator & 0x2000) == 0){
				val = ~val;
				val++;
				val &= 0x3fff;
				accumulator &= 0x3fff;
				accumulator /= val;
				ISOVERFLOW				
				accumulator = ~accumulator; // установить, что число отрицательное
				accumulator++;
				accumulator &= 0x7fff;
				accumulator |= 0x6000; // вернуть бит, обозначающий не команда						
				paintAcc(0);
				return 0;
			} else if ((val & 0x2000) == 0 && (accumulator & 0x2000) != 0){
				accumulator = ~accumulator;
				accumulator++;
				accumulator &= 0x3fff;
				val &= 0x3fff;
				accumulator /= val;
				accumulator = ~accumulator; // установить, что число отрицательное
				accumulator++;
				ISOVERFLOW
				accumulator &= 0x7fff;
				accumulator |= 0x6000; // вернуть бит, обозначающий не команда						
				paintAcc(0);
				return 0;
			} else if ((val & 0x2000) != 0 && (accumulator & 0x2000) != 0){
				accumulator = ~accumulator;
				accumulator++;
				accumulator &= 0x3fff;
				val = ~val;
				val++;
				val &= 0x3fff;
				accumulator /= val;
				ISOVERFLOW
				accumulator &= 0x7fff;
				accumulator |= 0x4000;
				paintAcc(0);
				return 0;
			}
		}
		accumulator /= val;
		ISOVERFLOW
		accumulator &= 0x7fff;						
		paintAcc(0);
		return 0;	
	}
	if (command == 33){   //MUL
		int val = 0;
		int err = sc_memoryGet(operand, &val);
		if (err != 0)
			return -1;
		if ((val & 0x4000) != 0 && (accumulator & 0x4000) != 0){ //проверка, если оба значения - целочисленные типы
			if ((val & 0x2000) == 0 && (accumulator & 0x2000) == 0){
				val &= 0x3fff;  // выключить первый бит(не команда)
				accumulator &= 0x3fff; // выключить первый бит(не команда)
				accumulator *= val;				
				ISOVERFLOW
				accumulator &= 0x7fff; // удалить лишние биты, если таковые появились
				accumulator |= 0x4000; // вернуть бит, обозначающий не команда					
				paintAcc(0);
				return 0;
			} else if ((val & 0x2000) != 0 && (accumulator & 0x2000) == 0){
				val = ~val;
				val++;
				val &= 0x3fff;
				accumulator &= 0x3fff;
				accumulator *= val;
				accumulator = ~accumulator; // установить, что число отрицательное
				accumulator++;
				ISOVERFLOW
				accumulator &= 0x7fff;
				accumulator |= 0x6000; // вернуть бит, обозначающий не команда						
				paintAcc(0);
				return 0;
			} else if ((val & 0x2000) == 0 && (accumulator & 0x2000) != 0){
				accumulator = ~accumulator;
				accumulator++;
				accumulator &= 0x3fff;
				val &= 0x3fff;
				accumulator *= val;				
				accumulator = ~accumulator; // установить, что число отрицательное
				accumulator++;
				ISOVERFLOW
				accumulator &= 0x7fff;
				accumulator |= 0x6000; // вернуть бит, обозначающий не команда						
				paintAcc(0);
				return 0;
			} else if ((val & 0x2000) != 0 && (accumulator & 0x2000) != 0){
				accumulator = ~accumulator;
				accumulator++;
				accumulator &= 0x3fff;
				val = ~val;
				val++;
				val &= 0x3fff;
				accumulator *= val;
				ISOVERFLOW
				accumulator &= 0x7fff;
				accumulator |= 0x4000;
				paintAcc(0);
				return 0;
			}
		}
		accumulator *= val;
		ISOVERFLOW
		accumulator &= 0x7fff;						
		paintAcc(0);	
		return 0;
	}
	if (command == 53){
		int val = 0;
		int err = sc_memoryGet(operand, &val);
		if (err == 0)
		accumulator |= val;						
		paintAcc(0);
		return 0;	
	}
	return -1;
}

int CU(){
	int value, error = 0;
	error = sc_memoryGet(current.pointer, &value);
	if (error == -1){
		sc_regSet(M, 1);
		return -1;
	}
	int command, operand;
	if ((value & 16384) != 0){	
		return 0;
	}
	error = sc_commandDecode(value, &command, &operand);
	if (error != 0){
		sc_regSet(E, 1);
		return -1;
	}
	if ((command >= 30 && command <= 33) || command == 53)
		return ALU(command, operand);
	else{
		if (command == 10){
			mt_gotoXY(1, 23);
			int val = 0;
			scanf("%d", &val);
			mt_gotoXY(1, 23);
			printf("                             \n         ");
			scanf("%*[^\n]");
			fflush(stdin);
			if (val <= 0x7fff && val >=0){
				int er = 0;				
				er = sc_memorySet(operand, val);
				if (er != 0)
					return -1;				
				paintCell(operand % 10, operand / 10, operand, cl_default);
				return 0;
			} else{
				sc_regSet(P,1);
				return -1;	
			}
		}
		if (command == 11){
			int val = 0, er = 0;		
			er = sc_memoryGet(operand, &val);
			if (er != 0)
				return -1;
			mt_gotoXY(1, 23);
			if ((val & 16384) == 0)
				printf("Value = +%x", val);	
			else if ((val & 8192) == 0)
				printf("Value = %x", val & 0x3fff);
			else{
				val = ~val & 0x3fff; 
				val++;
				printf("Value = -%x", val & 0x3fff);
			}
			return 0;
		}
		if (command == 20){
			int val = 0, er = 0;		
			er = sc_memoryGet(operand, &val);
			if (er != 0)
				return -1;
			accumulator = val;					
			paintAcc(0);
			return 0;
		}
		if (command == 21){
			int er = sc_memorySet(operand, accumulator);
			if (er == 0)
				paintCell(operand % 10, operand / 10, operand, cl_default);
			return er;
		}
		if (command == 40){
			if (operand < 100 && operand >=0){
				paintCell(current.x, current.y, current.pointer, cl_default);
				needIncrement = 1;
				current.pointer = operand;
				current.x = operand % 10;
				current.y = operand / 10;
				instructionCounter = operand;
				paintCell(current.x, current.y, current.pointer, cl_red);
				return 0;
			}else{
				sc_regSet(M, 1);
				return -1;			
			}
		}
		if (command == 41){
			if ((accumulator & 24576) != 0){
				if (operand > 99 || operand < 0){
					sc_regSet(M ,1);
					return -1;				
				}
				paintCell(current.x, current.y, current.pointer, cl_default);
				needIncrement = 1;
				current.pointer = operand;
				current.x = operand % 10;
				current.y = operand / 10;
				instructionCounter = operand;
				paintCell(current.x, current.y, current.pointer, cl_red);
			}
			return 0;
		}
		if (command == 42){
			if (accumulator == 16384){
				if (operand > 99 || operand < 0){
					sc_regSet(M ,1);
					return -1;				
				}
				paintCell(current.x, current.y, current.pointer, cl_default);
				needIncrement = 1;
				current.pointer = operand;
				current.x = operand % 10;
				current.y = operand / 10;
				instructionCounter = operand;
				paintCell(current.x, current.y, current.pointer, cl_red);
			}
			return 0;
		}
		if (command == 43){ 
			needIncrement = 2;
			return 0;
		}
		if (command == 55){ // JNS
			if ((accumulator & 24576) == 16384){
				if (operand > 99 || operand < 0){
					sc_regSet(M ,1);
					return -1;				
				}
				paintCell(current.x, current.y, current.pointer, cl_default);
				needIncrement = 1;
				current.pointer = operand;
				current.x = operand % 10;
				current.y = operand / 10;
				instructionCounter = operand;
				paintCell(current.x, current.y, current.pointer, cl_red);
			}
			return 0;
		}
	}
}

void printOper(int p1, int p2){
	mt_gotoXY(67, 11);
	if (p1 == -1)
		printf("        ");
	else 
		printf("+%2d:%2d ", p1, p2);
}


