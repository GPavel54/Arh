#ifndef ADDFUNC_H
#define ADDFUNC_H
#include "myBigChars.h"
#include "mySimpleComputer.h"
#include "terminal.h"

int changePoint(int value);
void paintCell(int x, int y, int pointer, enum colors col);
void paintAcc(int y);
void paintInst(int y);
void printCellBig();
void printInterface();
void printReg();
int CU();
void printOper(int p1, int p2);
#endif
