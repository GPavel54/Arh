#include "mySCASM.h"
#include "terminal.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define N 100

int mem[100];
uint8_t flags;
int sc_memoryInit(){
	for (int i = 0; i < N; i++){
		mem[i] = 0;
	}
	/*extern struct memCell current;
	current.pointer = 0;
	current.x = 0;
	current.y = 0;*/
	return 0;
}

int sc_memorySet(int address, int value){
	if (address > 99 || address < 0){
		sc_regSet(M, 1);
		return -1;
	}
	mem[address] = value;
	return 0;
}

int sc_memoryGet(int address, int * value){
	if (address > 99 || address < 0){
		sc_regSet(M, 1);
		return -1;
	}
	*value = mem[address];
	return 0;
}

int sc_memorySave(char * filename){
	FILE * output = NULL;
	output = fopen(filename, "wb");
	if (output == NULL)
		return -1;
	fwrite(mem, sizeof(int), N, output);
	fclose(output);
	return 0;
}

int sc_memoryLoad(char * filename){
	FILE * input = NULL;
	input = fopen(filename, "rb");
	if (input == NULL)
		return -1;
	fread(mem, sizeof(int), N, input);
	fclose(input);
	return 0;
}

int sc_regInit(){
	flags = 0x0;
	return 0;
}

int sc_regSet(int reg, int value){
	flags &= ~reg;
	if (value == 1)
		flags |= reg;
	return 0;
}
int sc_regGet(int reg, int * value){
	int ch = flags;
	ch &= reg;
	if (ch == 0)
		*value = 0;
	else
		*value = 1;
	return 0;
}

int sc_commandEncode(int command, int operand, int * value){
	int ret = 1;	
	if (command == 10 || command == 53 || command == 55 || command == 11 || command == 20 || command == 21 || (command < 34 && command > 29) || (command < 44 && command > 39) )
		ret = 0;
	if (ret != 0)
		return -1;
	if (operand > 127 || operand < 0)
		return -1;
	*value = 0;	
	*value += command;
	*value <<= 7;
	*value += operand;
	return 0;
}

int sc_commandDecode(int value, int * command, int * operand){
	unsigned int u = value;
	if ((value & 16384) != 0)
		return -1;
	*operand = u & 0x7f;
	u = value >> 7;
	int com = 0;
	com = u & 0x7f;
	if (com == 10 || com == 53 || com == 11 || com == 20 || com == 21 || (com < 34 && com > 29) || (com < 44 && com > 39) )
		*command = com;
	else
		return -1;
	//printOper(*command, *operand);
	return 0;
}


