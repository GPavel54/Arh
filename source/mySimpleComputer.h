#ifndef MYCO
#define MYCO
//Определение масок регистров
#define O 0x1
#define P 0x2
#define M 0x4
#define T 0x8
#define E 0x10

struct memCell{
	int pointer;
	int x;
	int y;
};

int sc_memoryInit();
int sc_memorySet(int address, int value);
int sc_memoryGet(int address, int * value);
int sc_memorySave(char * filename);
int sc_memoryLoad(char * filename);
int sc_regInit();
int sc_regSet(int reg, int value);
int sc_regGet(int reg, int * value);
int sc_commandEncode(int command, int operand, int * value);
int sc_commandDecode(int value, int * command, int * operand);
#endif
