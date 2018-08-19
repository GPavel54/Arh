#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream> // для перевода int в string
#include <cstdlib>
#include <cctype>
#include <algorithm>
using namespace std;

void deleteSpaces(string *);
/**Функции, для работы с таблицей, в которой содержатся имена объявленных переменных, и их адреса**/
void addToTable(string, int, vector<vector<string> >&); 
void printTable(vector<vector<string> >);
string returnAdress(string , vector<vector<string> >); //возвращает адрес переданной переменной
void addAdress(string&, int); // добавляет в итоговый ассемблерный файл номер строки, который является адресом ячейки памяти
int varExist(string, vector<vector<string> > vect); 
string createVar(char, vector<vector<string> >&, int *, string&);
/********/
string itos(int);
//int stoi(string);
int toRPN(string, vector<string>&); // перевести выражение в обратную польскую запись
void addSpaces(string& str); // добавляет пробелы между скобками
int isNumber(string str); // проверяет, является ли строка числом
int isSign(char c); // проверяет, является ли символ допустимой в выражении операцией
int getPrior(char c); // возвращает приоритет переданной операции

int main(int argc, char ** argv){
	string statements[] = {"INPUT", "GOTO", "PRINT", "REM", "LET"};
	int map[100] = {0}; // здесь хранится строка программы и соответствующая ей строка в памяти
	if (argc != 3){
		cout << "Некорректное количество аргументов." << endl;
		return 0;
	}
	ifstream file(argv[1]);
	if (!file){
		cout << "Не удалось открыть файл." << endl;
		return 0;
	}
	string str;
	string out = "";
	vector<vector<string> > table; //  здесь будут храниться переменные и их адреса, используемые в программе
	int memory = 0;
	int line = 0;
	while (!file.eof()){
		getline(file, str);
		if (str.length() == 0)
			continue;
		if (str.find(statements[3]) != string::npos)
			continue;
		if (str.find(statements[0]) != string::npos){ //если в строке требуется ввести значение переменной (INPUT)
			str.erase(str.begin(), str.begin() + str.find(statements[0]) + statements[0].length()); // оставляем только название переменной
			deleteSpaces(&str);		
			addAdress(out, memory);
			int check = varExist(str, table);
			if (check == 0){ // выделение памяти для переменной
				addToTable(str, memory, table);
				memory++;
				out += "= 0\n"; //окончание выделение памяти переменной
			}
			addAdress(out, memory);
			out += "READ "; //команда ввода данных с устройства ввода-вывода
			out += returnAdress(str, table) + "\n"; //ввод операнда для "READ"
			map[line] = memory;
			memory++;
		}else if (str.find(statements[1]) != string::npos){  // GOTO
			str.erase(str.begin(), str.begin() + str.find(statements[1]) + statements[1].length()); // оставляем только число, указывающее на строку
			deleteSpaces(&str);	
			addAdress(out, memory); //добавляет номер строки в ассемблерный файл
			out += "JUMP ";
			int toAdr = stoi(str);
			if (toAdr > line || toAdr < 0){
				cout << "Строка " << line << ", выход за границы памяти." << endl;
				exit(EXIT_FAILURE);
			}
			addAdress(out, map[toAdr]); //добавляет адрес строки ассемблерного файла, соответвтвующий строке входного файла
			out += '\n';
			memory++;
		}else if (str.find(statements[2]) != string::npos){ // PRINT
			str.erase(str.begin(), str.begin() + str.find(statements[2]) + statements[2].length()); // оставляем только название переменной, которую необходимо распечатать
			deleteSpaces(&str);	
			addAdress(out, memory); //добавляет номер строки в ассемблерный файл
			out += "WRITE ";
			string buf = returnAdress(str, table);
			if (buf == ""){
				cout << "Строка " << line << ", указанная переменная не объявлена." << endl;
				exit(EXIT_FAILURE);
			}
			out += buf;
			out += '\n';
			map[line] = memory;
			memory++;
		}else if (str.find(statements[4]) != string::npos){ // LET
			str.erase(str.begin(), str.begin() + str.find(statements[4]) + statements[4].length()); // оставляем только выражение
			map[line] = memory;
			int err = 0;
			vector<string> RPN;
			err = toRPN(str, RPN);
			if (err == -1){
				cout << "Строка :" << line + 1 << endl;
				exit(EXIT_FAILURE);
			}
			for (auto it = RPN.begin(); it < RPN.end(); it++)
				cout << *it;
			cout << endl;
		}
		line++;
	}
	cout << out << endl;
	return 0;
}

string itos(int p){
	ostringstream ss;
	ss << p;
	return ss.str();
}
/*
int stoi(string p){
	istringstream iss(p);
	int val;
	iss >> val;
	return val;
}*/

void addToTable(string str, int adr, vector<vector<string> >& vect){
	vect.push_back(vector<string>(2));
	vect[vect.size() - 1][0] = str;
	ostringstream ss;
	ss << adr;
	vect[vect.size() - 1][1] = ss.str();
}

string returnAdress(string p1, vector<vector<string> > vect){
	for (unsigned int i = 0; i < vect.size(); i++){
		if (vect[i][0] == p1){
			string out;
			if (vect[i][0].length() < 2)
					out = '0' + vect[i][1];
			else
				out = vect[i][1];
			return out;
		}
	}
	return "";
}

void addAdress(string& out, int memory){
	ostringstream ss;
	ss << memory;
	string add;
	add = ss.str();
	if (memory < 10){
		out += '0';
		out += add;
		out += ' ';
	}else {
		out += add;
		out += ' ';
	}
}

int varExist(string p1, vector<vector<string> > vect){
	int out = 0;
	for (unsigned int i = 0; i < vect.size(); i++)
		if (vect[i][0] == p1)
			out = 1;
	return out;
}

void printTable(vector<vector<string> > table){
	cout << table.size() << endl;
	for (unsigned int i = 0; i < table.size(); i++){
		cout << table[i][0].c_str() << "  " << table[i][1].c_str() << endl;
	}
}

void deleteSpaces(string * str){
	while (str->find(' ') != string::npos){
		str->erase(str->find(' '), 1);
	}
}

string returnAdress(char p1, vector<vector<string> >& vect, int * arr){
	string ret = "";
	if (islower(p1)){
		if (arr[p1 - 'a'] == 0){
			cout << "Wrong value." << endl;
			return "";
		}
		addAdress(ret, arr[p1 - 'a']);
	}else{
		ret = p1;
		if (returnAdress(ret, vect) == "")
			return "";
		ret = returnAdress(ret, vect);
	}
	return ret;
}

int toRPN(string expression, vector<string>& out){ 
	string stack;
	string temp;
	addSpaces(expression);
	stringstream ss(expression);
	int i = 0;
	if (expression.find("=") == string::npos){
		cout << "В строке нету символа '='." << endl;
		return -1;
	}
	while(ss >> temp){
		if (i == 0){
				if (islower(temp[0]) || isNumber(temp) || temp.length() != 1){
					cout << "Выражение составлено неверно." << endl;
					return -1; 
				}
			ss >> temp;
			i++;
			continue;
		}
		if (temp == ")"){
			if (stack.find("(") == string::npos){
				cout << "Неверно расставлены скобки." << endl; // скобки расставлены неверно
				return -1;
			}
			while(stack[stack.size() - 1] != '('){
				string tm = "";
				tm.push_back(stack[stack.size() - 1]);
				out.push_back(tm);
				stack.pop_back();
			}
			stack.pop_back();
		}
		if (isNumber(temp)){  // значит это число
			out.push_back(temp);
			continue;
		}else if (temp.length() != 1 || islower(temp[0])){
			cout << "Выражение составлено неверно." << endl; // значит неправильно введено выражение
			return -1;
		} else if (!isSign(temp[0]) && temp != "(" && temp != ")"){ // значит это переменная
			out.push_back(temp);
			continue;
		}
		if (isSign(temp[0])){
			while (getPrior(stack[stack.size() - 1]) >= getPrior(temp[0])){
				string tm = "";
				tm.push_back(stack[stack.size() - 1]);
				out.push_back(tm);
				stack.pop_back(); 
			}
			stack.push_back(temp[0]);
		}
		if (temp == "(")
			stack.push_back(temp[0]);
			continue;
		
	}
	if (stack.find("(") != string::npos || stack.find(")") != string::npos){
		cout << "Неверно расставлены скобки." << endl;
		return -1;
	}
	while (stack.size() != 0){
		string tm = "";
		tm.push_back(stack[stack.size() - 1]);
		out.push_back(tm);
		stack.pop_back();
	}
	return 0;
}

string createVar(char p1, vector<vector<string> >& vect, int * memory, string& out){
	addAdress(out, *memory);
	out += "= 16384\n";
	string toTab = " ";
	toTab.resize(1);
	toTab[0] = p1;
	addToTable(toTab, *memory, vect);
	toTab = "";
	addAdress(toTab, *memory);
	(*memory)++;
	return toTab;
}

void addSpaces(string& str){
	size_t si = 0;
	while (str.find("(", si) != string::npos){
		si = str.find("(", si);
		str.replace(si, 1, "( ");
		si++;
	}
	si = 0;
	while (str.find(")", si) != string::npos){
		si = str.find(")", si);
		str.replace(si, 1, " )");
		si += 2;
	}
}

int isNumber(string str){
	unsigned int i = 0;
	while (isdigit(str[i]) && i < str.length())
		i++;
	if (i == str.length())
		return 1;
	return 0;
}

int isSign(char c){
	if ((c == '/') || (c == '*') || (c == '|') || (c == '+') || (c == '-'))
		return 1;
	return 0;
}

int getPrior(char c){
	if (c == '*' || c == '/')
		return 3;
	if (c == '+' || c == '-')
		return 2;
	if (c == '|')
		return 1;
	return 0;
}
