#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream> // для перевода int в string
#include <cstdlib>
#include <cctype>
#include <algorithm>
#define ITER vector<char>::iterator
using namespace std;
void deleteSpaces(string *);
/**Функции, для работы с таблицей, в которой содержатся имена объявленных переменных, и их адреса**/
void addToTable(string, int, vector<vector<string> >&); 
void printTable(vector<vector<string> >);
string returnAdress(string , vector<vector<string> >); //возвращает адрес переданной переменной
void addAdress(string&, int); // добавляет в итоговый ассемблерный файл номер строки, который является адресом ячейки памяти
int varExist(string, vector<vector<string> > vect); 
/********/
int translateExpression(string, int *, vector<vector<string> >&, string&); // перевести выражение 
int countParan(vector<char>);
string itos(int);
int stoi(string);

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
			err = translateExpression(str, &memory, table, out);
			if (err == -1){
				cout << "Строка " << line + 1 << ", ошибка в выражении." << endl;
				exit(EXIT_FAILURE);
			}
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

ITER isParan(ITER it1, ITER it2){ // поиск скобки, соответствующей закрывающей
	return find(it1, it2, '(');
}

int translateExpression(string str, int * memory, vector<vector<string> >& vect, string& out){
	/* Блок считывания выражения в массив символов без пробелов */
	stringstream ss(str);
	string temp;
	vector<char> tokens;
	while (ss >> temp){
		for (unsigned int i = 0; i < temp.length(); i++)
			tokens.push_back(temp[i]);
	}
	/* ******* */
	/* Проверка на правильное количество переменных и операций */
	int size = countParan(tokens);
	if (size == -1)
		return -1;
	/* ******* */
	
	int addMap[26] = {0}; //здесь хранятся переменные для промежуточных значений выражения
	string addExp[26] = {""}; // здесь хранятся выражения, результат которых должен быть записан во временную переменную
	ITER it = tokens.begin(), it1;
	ITER rem;
	ITER fst = it;  // чтобы отследить, изменился ли адрес скобки, при вызове следующий функции
	while ((it = find(tokens.begin(), tokens.end(), '(')) != tokens.end()){
		rem = it;
		it1 = find(it, tokens.end(), ')');
		if (it1 == tokens.end())
			return -1;  // Не верно расположены скобки
		while ((it = isParan(it + 1, it1)) != it1)
			fst = it; // Поиск первой операции в скобках
		/* копирования выражения в скобках */
		char toDel[it1 - fst + sizeof(char)]; 
		int y = 0;
		it = fst;
		for (; fst <= it1; fst++){
			if (*fst != '(' && *fst != ')'){
				toDel[y] = *fst;
				y++;
			}
		}
		toDel[y] = '\0';
		/* ****** */
		/* Обновление таблицы временных переменных */
		for (int pt1 = 0; pt1 < 26; pt1++){
			if (addMap[pt1] == 0){
				addMap[pt1] = 1;
				addExp[pt1] = toDel;
				*it = 'a' + pt1;
				break;
			}
		}
		/* ***** */
		tokens.erase(it + 1, it1 + 1);
		cout << addMap[0] << "  -  " << addExp[0] << endl;
		break;
	}
	for (ITER i1 = tokens.begin(); i1 != tokens.end(); i1++)
		cout << *i1;
	cout << endl;
	exit(EXIT_SUCCESS);
	return 0;	
}

int countParan(vector<char> vect){
	int p1= 0, p2 = 0, p3 = 0, p4 = 0;
	for (ITER it = vect.begin(); it != vect.end(); it++){
		if (*it == '(')
			p1++;
		if (*it == ')')
			p2++;
		if (*it == '/')
			p3++;
		if (*it == '*')
			p3++;
		if (*it == '+')
			p3++;
		if (*it == '-')
			p3++;
		if (isalpha(*it))
			p4++;
		if (*it == '=')
			p3++;
	}
	int ret = p3;
	if ((p1 != p2) || (p4 != p3 + 1))
		ret = -1;
	return ret;
}
