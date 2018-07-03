#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mySCASM.h"

int getCommand(char * command){
	if (strcmp(command, "READ") == 0)
		return 10;	
	if (strcmp(command, "WRITE") == 0)
		return 11;
	if (strcmp(command, "LOAD") == 0)
		return 20;
	if (strcmp(command, "STORE") == 0)
		return 21;
	if (strcmp(command, "ADD") == 0)
		return 30;
	if (strcmp(command, "SUB") == 0)
		return 31;
	if (strcmp(command, "DIVIDE") == 0)
		return 32;
	if (strcmp(command, "MUL") == 0)
		return 33;
	if (strcmp(command, "JUMP") == 0)
		return 40;
	if (strcmp(command, "JNEG") == 0)
		return 41;
	if (strcmp(command, "JZ") == 0)
		return 42;
	if (strcmp(command, "HALT") == 0)
		return 43;
	if (strcmp(command, "OR") == 0)
		return 53;
	if (strcmp(command, "=") == 0)
		return 60;
	return -1;
}

int main(int argv, char **args){
	FILE * fd;
	printf("%s\n", args[1]);
	fd = fopen(args[1], "r");
	if (fd == NULL){
		printf("Не удалось открыть файл.");
		return 0;
	}
	char str[1000];
	int i = 0;
	char c;
	int lines = 1;
	while((c = fgetc(fd)) != EOF){
		if (c == ';'){
			do {
				c = fgetc(fd);
			} while(c != '\n');
		}
			str[i] = c;
			i++;
		if (c == '\n')
			lines++;
	}
	fclose(fd);
	char line[20];
	int y = 0;
	extern int mem[100];
	sc_memoryInit();
	int cell, command, operand;
	do{
		str[strlen(str) - 1] = '\0';
		lines--;	
	} while(str[strlen(str) - 1] == '\n');
	for(int j = 0; j < lines; j++){
		int u = 0;
		for(int l = 0; l < 20; l++)
			line[l] = '\0';
		do{  // считывание строки
			if (y == strlen(str)){
				u++;
				break;
			}
			line[u] = str[y];
			u++;   // для перемещения внутри line
			y++;   //  для перемещения внутри  str
		} while(str[y] != '\n');
		printf("Line = %s\n", line);
		char strComm[20];
		int add = 0;
		char uy[20];
		sscanf(line,"%d %s %s", &add, strComm, uy);
		printf("%d\n%s\n%s\n", add, strComm, uy); 
		line[u] = '\0';   //   завершаю строку
		line[u + 1] = '\0';
		line[u + 2] = '\0';
		y++;   // пропускаю перенос строки с прошлой итерации
		char cellC[2]; // здесь храню номер ячейки памяти
		cellC[0] = line[0];
		cellC[1] = line[1];
		cell = atoi(cellC);
		if (cell < 0 || cell > 99){
			printf("Wrong cell value on line %d\n", j);
			exit(EXIT_FAILURE);
		}
		strcpy(line, line + 2);
		char operandC[2]; // здесь хранится операнд 
		operandC[0] = line[strlen(line) - 2];
		operandC[1] = line[strlen(line) - 1];
		operand = atoi(operandC);
		line[strlen(line) - 2] = '\0';
		command = getCommand(line);
		if (command == -1){
			printf("Wrong command on line %d\n", j);
			exit(EXIT_FAILURE);
		}
		int out, err;
		if ((operand < 0 || operand > 99) && command != 60){
			printf("Wrong operand on line %d\n", j);
			exit(EXIT_FAILURE);
		}
		if (command != 60)
			err = sc_commandEncode(command, operand, &out);
		else{
			if ((operand < 32768) && (operand > -1))
				out = operand;
			else{
				printf("Too much value on line %d\n", j);
				exit(EXIT_FAILURE);
			}	
		}
		if (err == -1){
			printf("Wrong on line %d\n", j);
			exit(EXIT_FAILURE);
		}
		mem[cell] = out; // Подготовка массива памяти
	}
	char memory[] = "RAM.b";
	int err = sc_memorySave(memory);
	return 0;
}
