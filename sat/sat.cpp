#include <iostream>
#include <fstream>
#include <string>
#include <sstream> // для перевода int в string
#include <cstdlib>
#include "../source/mySCASM.h"
using namespace std;

/*int stoi(string p){
	istringstream iss(p);
	int val;
	iss >> val;
	return val;
}*/

void deleteSpaces(string * str){
	while (str->find(' ') != string::npos){
		str->erase(str->find(' '), 1);
	}
}

int getCommand(string command){
	if (command == "READ")
		return 10;
	else if (command == "WRITE")
		return 11;
	else if (command == "LOAD")
		return 20;
	else if (command == "STORE")
		return 21;
	else if (command == "ADD")
		return 30;
	else if (command == "SUB")
		return 31;
	else if (command == "DIVIDE")
		return 32;
	else if (command == "MUL")
		return 33;
	else if (command == "JUMP")
		return 40;
	else if (command == "JNEG")
		return 41;
	else if (command == "JZ")
		return 42;
	else if (command == "HALT")
		return 43;
	else if (command == "OR")
		return 53;
	else if (command == "JNS")
		return 55;
	else if (command == "=")
		return 60;
	else
		return -1;
}

int main(int argc, char ** argv){
	if (argc != 2){
		cout << "Некорректное количество аргументов." << endl;
		exit(EXIT_FAILURE);
	}
	ifstream file(argv[1]);
	if (!file){
		cout << "Не удалось открыть файл." << endl;
		exit(EXIT_FAILURE);
	}
	string str;
	int line = 0;
	extern int mem[100];
	while (!file.eof()){
		string cell, operand, command;
		getline(file, str);
		if (str.length() == 0)
			continue;
		if (str.find(";") != string::npos)
			str.erase(str.begin() + str.find(";"), str.end());
		stringstream ss(str);
		ss >> cell;
		if (stoi(cell) > 99 || stoi(cell) < 0){
			cout << "Выход за границы памяти. Строка " << line << endl;
			exit(EXIT_FAILURE);
		}
		ss >> command;
		int command_int = getCommand(command);
		if (command_int == -1){
			cout << "Неверная команда. Строка" << line << endl;
			exit(EXIT_FAILURE);
		}
		ss >> operand;
		if (((stoi(operand) > 99 || stoi(operand) < 0) && command_int != 60) || ((stoi(operand) < 0 || stoi(operand) > 0x7fff) && command_int == 60)){
			cout << "Неверный операнд. Строка " << line << endl;
			exit(EXIT_FAILURE);
		}
		int out;
		int err = sc_commandEncode(command_int, stoi(operand), &out);
		if (err == -1){
			cout << "Не удалось закодировать команду. Строка" << line << endl;
			exit(EXIT_FAILURE);
		}
		mem[stoi(cell)] = out;
		line++;
	}
	if (!sc_memorySave("../data/RAM.b"))
		cout << "Программа успешно транслирована!" << endl;
	return 0;
}
