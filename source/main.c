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
int stop = 0;
int needIncrement = 0;

void signalhandler(int signo){
	sc_regSet(T, 0);
	printReg();	
	int error = CU();
	if (error != 0){
		printReg();
		stop = 0;
		return;		
	}
	paintAcc(0);
	if (needIncrement == 0)
		if (instructionCounter < 99){
			changePoint(1);
		}
		else{
			stop = 0;
			sc_regSet(M, 1);
			return;
		}
}

void sghandler(int signo){
	sc_regSet(T, 1);
	sc_regSet(P, 0);
	sc_regSet(O, 0);
	sc_regSet(E, 0);
	sc_regSet(M, 0);
	printReg();
	sc_memoryLoad("data/RAM0.b");
	printInterface();
	current.x = 0; 
	current.y = 0;
	current.pointer = 0;
	instructionCounter = 0;
	paintInst(0);
	paintCell(current.x, current.y, current.pointer, cl_red);
	mt_gotoXY(1, 23);
	printf("                        \n         ");
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
	rk_mytermsave();
	while (1){
		paintInst(0);
		rk_readkey(&a);
		sc_regGet(T, &tact);
		if (tact == 1)
			setitimer(ITIMER_REAL, 0, NULL);
		switch (a){
			case KEY_right:
				changePoint(1);				
				break;
			case KEY_down:
				changePoint(10);
				break;	
			case KEY_left:
				changePoint(-1);
				break;
			case KEY_up:
				changePoint(-10);
				break;
			case KEY_another:
				mt_gotoXY(1, 23);
				printf("Input value for %d:\n", current.pointer);
				int wInp = scanf("%d", &val);
				if (val < 32768 && val > -32768 && wInp != 0){
					sc_memorySet(current.pointer, val);
					paintCell(current.x, current.y, current.pointer, cl_red);
				}
				scanf("%*[^\n]");
				fflush(stdin);
				mt_gotoXY(1, 23);
				printf("                        \n         ");
				val = 0;
				wInp = 0;
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
				static struct itimerval nval;  // установка таймера
				signal (SIGALRM, signalhandler);
				nval.it_interval.tv_sec = 2;
				nval.it_interval.tv_usec = 0;
				nval.it_value.tv_sec = 1;
				nval.it_value.tv_usec = 0;
				setitimer(ITIMER_REAL, &nval, NULL); //конец установки таймера
				stop = 1;				
				while (stop == 1)
					continue;
				setitimer(ITIMER_REAL, 0, NULL);
				sc_regSet(T, 1);
				printReg();		
			case KEY_t: ;
				int e;
				e = CU();
				if (e != 0)
					printReg();
				if (needIncrement == 0)
					changePoint(1);
				needIncrement = 0;
				break;	
			case KEY_f5:
				printf("Input value for accumulator:\n");
					wInp = scanf("%d", &val);
					if (val < 32768 && val > -32768 && wInp != 0){	
						accumulator = val;
						paintAcc(0);
					}
				scanf("%*[^\n]");
				fflush(stdin);
				wInp = 0;
				mt_gotoXY(1, 23);
				printf("                             \n         ");
				break;
			case KEY_f6:
				printf("Input value for instructionCounter:\n");
				wInp = scanf("%d", &val);
				if (val < 100 && val > -1 && wInp != 0){
						instructionCounter = val;
						paintInst(0);
						paintCell(current.x, current.y, current.pointer, cl_default);
						current.x = val % 10;
						current.y = val / 10;
						current.pointer = val;
						paintCell(current.x, current.y, current.pointer, cl_red);
					}
				scanf("%*[^\n]");
				fflush(stdin);
				wInp = 0;
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
