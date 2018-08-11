#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream> // для перевода int в string
#include <cstdlib>
using namespace std;
void addToTable(string, int, vector<vector<string> >&); 
void deleteSpaces(string *);
void printTable(vector<vector<string> >);
string returnAdress(string , vector<vector<string> >); //возвращает адрес переданной переменной
void addAdress(string&, int); // добавляет в итоговый ассемблерный файл номер строки, который является адресом ячейки памяти 
string itos(int);
int stoi(string);

int main(int argc, char ** argv){
	string statements[] = {"INPUT", "GOTO", "PRINT"};
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
		if (str.find(statements[0]) != string::npos){ //если в строке требуется ввести значение переменной (INPUT)
			str.erase(str.begin(), str.begin() + str.find(statements[0]) + statements[0].length()); // оставляем только название переменной
			deleteSpaces(&str);		
			addAdress(out, memory);
			addToTable(str, memory, table);
			memory++;
			out += "= 0\n"; //окончание выделение памяти переменной
			addAdress(out, memory);
			out += "READ "; //команда ввода данных с устройства ввода-вывода
			out += returnAdress(str, table) + "\n"; //ввод операнда для "READ"
			map[line] = memory;
			memory++;
		}else if (str.find(statements[1]) != string::npos){
			str.erase(str.begin(), str.begin() + str.find(statements[1]) + statements[1].length()); // оставляем только число, указывающее на строку
			deleteSpaces(&str);	
			addAdress(out, memory);
			out += "JUMP ";
			int toAdr = stoi(str);
			if (toAdr > line || toAdr < 0)
				exit(EXIT_FAILURE);
			addAdress(out, map[toAdr]);
			out += '\n';
			memory++;
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

int stoi(string p){
	istringstream iss(p);
	int val;
	iss >> val;
	return val;
}

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
