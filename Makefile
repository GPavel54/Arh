all: main.o terminal.o myBigChars.o mySimpleComputer.o myReadkey.o addFunctions.o computer

main.o: source/main.c
	gcc -c -o temp/main.o source/main.c
terminal.o: source/terminal.c
	gcc -c -o temp/terminal.o source/terminal.c
myBigChars.o: source/myBigChars.c
	gcc -c -o temp/myBigChars.o source/myBigChars.c
mySimpleComputer.o: source/mySimpleComputer.c
	gcc -c -o temp/mySimpleComputer.o source/mySimpleComputer.c
myReadkey.o: source/myReadkey.c
	gcc -c -o temp/myReadkey.o source/myReadkey.c
addFunctions.o: source/addFunctions.c
	gcc -c -o temp/addFunctions.o source/addFunctions.c
computer :
	gcc -Wall -o comp temp/main.o temp/terminal.o temp/myBigChars.o temp/mySimpleComputer.o  temp/myReadkey.o temp/addFunctions.o
clean:
	rm -rf *.o
withcode: 
	gcc -Wall -o comp temp/main.o temp/terminal.o temp/myBigChars.o temp/mySimpleComputer.o temp/myReadkey.o temp/addFunctions.o -g
