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
struct memCell current;

void signalhandler(int signo){
	if (instructionCounter < 99)
		instructionCounter += 1;
	paintInst(0);
}

void sghandler(int signo){
	sc_regSet(T, 1);
	printReg();
	sc_memoryLoad("data/RAM0.b");
	printInterface();
	current.x = 0; 
	current.y = 0;
	current.pointer = 0;
	paintCell(current.x, current.y, current.pointer, cl_red);
}

int main(){
	sc_memoryInit();
	mt_clrscr();
	int tact = 0;
	sc_regSet(T, 1);  //игнорирование тактовых импульсов
	sc_regGet(T, &tact);
	printInterface();
	paintCell(current.x, current.y, current.pointer, cl_red);
	signal(SIGUSR1, sghandler);//Установка обработчика для SIGUSR1
	int a;
	int val;
	int inAcc = 0;    // Проверка, находится ли курсор в аккумуляторе
	rk_mytermsave();
	while (1){
		paintInst(0);
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
				if (inAcc != 1)
					changePoint(10);
				break;	
			case KEY_left:
				if (inAcc == 1){
					inAcc = 0;
					paintAcc(0);
					paintCell(current.x, current.y, current.pointer, cl_red);
				} else
					changePoint(-1);
				break;
			case KEY_up:
				if (inAcc != 1)
					changePoint(-10);
				break;
			case KEY_another:
					mt_gotoXY(1, 23);
				if (inAcc == 1){
					scanf("%d", &val);
					if (val < 32768 && val > 0){
						accumulator = val;
						paintAcc(1);
					}
					mt_gotoXY(1, 23);
					printf("                                   \n         ");
				} else {
					printf("Input value for %d:\n", current.pointer);
					scanf("%d", &val);
					if (val < 32768 && val > 0){
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
			case KEY_r:
				sc_regSet(T, 0);
				printReg();
				static struct itimerval nval;
				signal (SIGALRM, signalhandler);
				nval.it_interval.tv_sec = 2;
				nval.it_interval.tv_usec = 0;
				nval.it_value.tv_sec = 2;
				nval.it_value.tv_usec = 0;
				setitimer(ITIMER_REAL, &nval, NULL);
			case KEY_t:
				break;	
			case KEY_f5:
				printf("Input value for accumulator:\n");
					scanf("%d", &val);
					if (val < 32768 && val > 0){	
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
			case KEY_i:
				raise(SIGUSR1);
				break;
		}	
	}
	mt_gotoXY(1, 23);
	return 0;
}
