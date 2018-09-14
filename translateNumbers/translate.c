#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv){
	if (argc != 2){
		printf("Неправильное количество аргументов.\n");
		exit(EXIT_FAILURE);
	}
	int num = atoi(argv[1]);
	if (num >= 0)
		num = num | 16384;
	else{
		num = num * -1;
		num = (~num & 0x3fff) + 1;
		num = num | 16384;
	} 
	printf("%d\n", num);
	return 0;
}
