#ifndef MYCO
#define MYCO
//Определение масок регистров
#define O 0x1   //  деление на ноль
#define P 0x2   //  переполнение
#define M 0x4   //  выход за границы памяти
#define T 0x8   //  игнорирование тактовых импульсов
#define E 0x10  //  неверная команда

struct memCell{
	int pointer;
	int x;
	int y;
};

int sc_memoryInit();
int sc_memorySet(int address, int value);
int sc_memoryGet(int address, int * value);
int sc_memorySave(const char * filename);
int sc_memoryLoad(char * filename);
int sc_regInit();
int sc_regSet(int reg, int value);
int sc_regGet(int reg, int * value);
int sc_commandEncode(int command, int operand, int * value);
int sc_commandDecode(int value, int * command, int * operand);
#endif
