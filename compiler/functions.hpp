#ifndef FUNC_H
#define FUNC_H
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream> // для перевода int в string
#include <cstdlib>
#include <cctype>
#include <algorithm>
using namespace std;
void deleteSpaces(string * str);
/**Функции, для работы с таблицей, в которой содержатся имена объявленных переменных, и их адреса**/
void addToTable(string, int, vector<vector<string> >&); 
string addToTableConst(string, int, vector<vector<string> >&); // добавляет в таблицу числовую константу
void printTable(vector<vector<string> >);
string returnAdress(string , vector<vector<string> >); //возвращает адрес переданной переменной
void addAdress(string&, int); // добавляет в итоговый ассемблерный файл номер строки, который является адресом ячейки памяти
int varExist(string, vector<vector<string> > vect); 
string createVar(char, vector<vector<string> >&, int *, string&);
/********/
string translateConst(string); // переводит константу в ассемблер
string itos(int);
int translateExp(vector<string>&, int *, string& , vector<vector<string> >&);
//int stoi(string);
string getCommand(string);
int toRPN(string, vector<string>&); // перевести выражение в обратную польскую запись
void addSpaces(string& str); // добавляет пробелы между скобками
int isNumber(string str); // проверяет, является ли строка числом
int isSign(char c); // проверяет, является ли символ допустимой в выражении операцией
int getPrior(char c); // возвращает приоритет переданной операции

#endif
