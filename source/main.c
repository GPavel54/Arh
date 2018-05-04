#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include "myReadkey.h"
#include "addFunctions.h"
#include "mySimpleComputer.h"
#include "myBigChars.h"
#include "terminal.h"

uint8_t flags;
uint16_t instructionCounter;
int accumulator;
int inInstr = 0;   // Проверка, находится ли курсор в счетчике команд
struct memCell current;

void signalhandler(int signo){
	if (instructionCounter < 99)
		instructionCounter += 1;
	if (inInstr != 1)
		paintInst(0);
	else
		paintInst(1);
}

int main(){
	sc_memoryInit();
	mt_clrscr();
	printInterface();
	paintCell(current.x, current.y, current.pointer, cl_red);	
	int a;
	int val;
	int inAcc = 0;    // Проверка, находится ли курсор в аккумуляторе
	rk_mytermsave();
	int tact = 0;
	//sc_regSet(T, 1);  //игнорирование тактовых импульсов
	sc_regGet(T, &tact);
	printReg();
	if (tact != 1){
		static struct itimerval nval;
		signal (SIGALRM, signalhandler);
		nval.it_interval.tv_sec = 2;
		nval.it_interval.tv_usec = 0;
		nval.it_value.tv_sec = 2;
		nval.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &nval, NULL);	
	}
	while (a != KEY_i){
		rk_readkey(&a);
		sc_regGet(T, &tact);
		if (tact == 1)
			setitimer(ITIMER_REAL, 0, NULL);
		switch (a){
			case KEY_right:
				if (current.x != 9)
					changePoint(1);
				else{
					paintCell(current.x, current.y, current.pointer, cl_default);
					paintAcc(1);
					inAcc = 1;
				}				
				break;
			case KEY_down:
				if (inAcc != 1 && inInstr != 1)
					changePoint(10);
				if (inAcc == 1){
					paintInst(1);
					paintAcc(0);
					inAcc = 0;
					inInstr = 1;		
				}
				break;	
			case KEY_left:
				if (inAcc == 1){
					inAcc = 0;
					paintAcc(0);
					paintCell(current.x, current.y, current.pointer, cl_red);
				} else if (inInstr == 1){
					inInstr = 0;
					paintInst(0);
					paintCell(current.x, current.y, current.pointer, cl_red);
				} else
					changePoint(-1);
				break;
			case KEY_up:
				if (inAcc != 1 && inInstr != 1)
					changePoint(-10);
				if (inInstr == 1){
					paintInst(0);
					paintAcc(1);
					inAcc = 1;
					inInstr = 0;		
				}
				break;
			case KEY_another:
					mt_gotoXY(1, 23);
				if (inAcc == 1){
					scanf("%d", &val);
					if (val < 10000 && val > -10000){
						accumulator = val;
						paintAcc(1);
					}
					mt_gotoXY(1, 23);
					printf("                                   \n         ");
				} else if (inInstr == 1){
					scanf("%d", &val);
					if (val < 100 && val > -1){
						instructionCounter = val;
						paintInst(1);
					}
					mt_gotoXY(1, 23);
					printf("                                   \n         ");
				}else {
					printf("Input value for %d:\n", current.pointer);
					scanf("%d", &val);
					if (val < 10000 && val > -10000){
						sc_memorySet(current.pointer, val);
						paintCell(current.x, current.y, current.pointer, cl_red);
						mt_gotoXY(1, 23);
						printf("                        \n         ");
					}
				}
				break;
			case KEY_s:
				sc_memorySave("data/RAM.b");	
				break;
			case KEY_l:
				sc_memoryLoad("data/RAM.b");
				printInterface();
				paintCell(current.x, current.y, current.pointer, cl_red);
				break;
			case KEY_q:    // временно
				setitimer(ITIMER_REAL, 0, NULL);
				break;
			case KEY_f5:
				printf("Input value for accumulator:\n");
					scanf("%d", &val);
					if (val < 10000 && val > -10000){	
						accumulator = val;
						paintAcc(0);
					}
				mt_gotoXY(1, 23);
				printf("                             \n         ");
				break;
			case KEY_f6:
				printf("Input value for instructionCounter:\n");
				scanf("%d", &val);
				if (val < 100 && val > -1){
						instructionCounter = val;
						paintInst(0);
					}
				mt_gotoXY(1, 23);
				printf("                                      \n         ");      
				break;	
		}	
	}
	mt_gotoXY(1, 23);
	return 0;
}
