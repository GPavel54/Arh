all: main.o terminal.o myBigChars.o mySimpleComputer.o myReadkey.o addFunctions.o computer

main.o: source/main.c
	mkdir -p build
	gcc -c -o build/main.o source/main.c
terminal.o: source/terminal.c
	gcc -c -o build/terminal.o source/terminal.c
myBigChars.o: source/myBigChars.c
	gcc -c -o build/myBigChars.o source/myBigChars.c
mySimpleComputer.o: source/mySimpleComputer.c
	gcc -c -o build/mySimpleComputer.o source/mySimpleComputer.c
myReadkey.o: source/myReadkey.c
	gcc -c -o build/myReadkey.o source/myReadkey.c
addFunctions.o: source/addFunctions.c
	gcc -c -o build/addFunctions.o source/addFunctions.c
computer:
	gcc -Wall -o comp build/main.o build/terminal.o build/myBigChars.o build/mySimpleComputer.o  build/myReadkey.o build/addFunctions.o
compiler: compiler/functions.cpp compiler/compiler.cpp
	g++ -Wall -std=c++11 -o scompiler compiler/functions.cpp compiler/compiler.cpp
sat: sat/sat.cpp
	g++ -Wall -std=c++11 -o satr sat/sat.cpp source/mySCASM.c
clean:
	rm -rf build/*.o
withcode: 
	gcc -Wall -o comp build/main.o build/terminal.o build/myBigChars.o build/mySimpleComputer.o build/myReadkey.o build/addFunctions.o -g
