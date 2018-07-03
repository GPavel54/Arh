#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mySCASM.h"

int getCommand(char *comm)
{
	if (strcmp(comm,"READ") == 0)
		return 10;
	if (strcmp(comm,"WRITE") == 0)
		return 11;

	if (strcmp(comm,"LOAD") == 0)
		return 20;
	if (strcmp(comm,"STORE") == 0)
		return 21;

	if (strcmp(comm,"ADD") == 0)
		return 30;
	if (strcmp(comm,"SUB") == 0)
		return 31;
	if (strcmp(comm,"DIVIDE") == 0)
		return 32;
	if (strcmp(comm,"MUL") == 0)
		return 33;

	if (strcmp(comm,"JUMP") == 0)
		return 40;
	if (strcmp(comm,"JNEG") == 0)
		return 41;
	if (strcmp(comm,"JZ") == 0)
		return 42;
	if (strcmp(comm,"HALT") == 0)
		return 43;

	if (strcmp(comm,"OR") == 0)
		return 53;
	if (strcmp(comm, "=") == 0)
		return 90;
	return -1;
}

int main(int argc, char** argv)
{
	char line[100], ch;
	char strComm[10], operand_str[10];
	int value, command, operand, address;
   
	FILE *file;

	if ( argc != 3) {
		printf("Шаблон: ./sat file.sa file.o\n ");
		return -1;

	} else if ( ( file = fopen (argv[1], "rb") ) <= 0) {
		printf("Не удалось открыть %s.\n", argv[1]);
		return -1;
	}

	sc_memoryInit();
	
	do {
		fgets(line, sizeof(line), file);
		if (sscanf(line,"%d %s %s", &address, strComm, &operand_str) < 3) {
			printf("Ошибка трансляции.<\n");
			return -1;
		}
		command = getCommand(strComm);
		if (command == 90){
			if (sscanf(operand_str, "%d", &operand) != 1 ||
			sc_memorySet(address, operand) == 1){
				printf("Ошибка трансляции.\n");
				return -1;
			}
			continue;
		}
		if (command != -1) {
			if ( sscanf(operand_str, "%d", &operand) != 1 
			|| sc_commandEncode(command, operand, &value) == -1 
			|| sc_memorySet (address, value) == -1) {
				printf("Ошибка трансляции.\n");
				return -1;
			}
		} else {
			printf("Ошибка трансляции.\n");
			return -1;
		}  

   	} while(!feof(file));
	
	fclose(file);
	if (sc_memorySave(argv[2]) == -1) {
		printf("Ошибка создания файла %s\n", argv[2]);
		return -1;
	}

	printf("Ок.\n");
	return 0;
}
