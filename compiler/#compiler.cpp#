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

int main(int argc, char ** argv){
	string statements[] = {"INPUT", "GOTO", "PRINT", "REM", "LET", "IF", "END"};
	int map[100]; // здесь хранится строка программы и соответствующая ей строка в памяти
	for (unsigned int i = 0; i < 100; i++)
		map[i] = -1;
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
	int strExist = 0; // если единица, то выполняется обработка if
	while (!file.eof()){
		if (strExist == 0)
			getline(file, str);
		if (str.length() == 0)
			continue;
		if (str.find(statements[3]) != string::npos)
			continue;
		if (str.find(statements[5]) != string::npos){ // IF
			str.erase(str.begin(), str.begin() + str.find(statements[5]) + statements[5].length()); // оставляем только условие и выражение
			stringstream ss(str);
			string operands[3];
			if (strExist == 0)
				map[line] = memory;
			ss >> operands[0]; // переменная или константа условия
			ss >> operands[1]; // знак условия
			ss >> operands[2]; // переменная или константа условия
			if (operands[1] != "=" && operands[1] != ">" && operands[1] != "<"){
				cout << "Строка " << line << endl;
				cout << "Неверно составлено условие" << endl;
				exit(EXIT_FAILURE);
			} else if (operands[0].length() > 1 || operands[2].length() > 1){
				cout << "Строка " << line + 1 << endl;
				cout << "Имя переменной может состоять только из одного символа." << endl;
				exit(EXIT_FAILURE);
			}
			/* Загрузка в аккумулятор первого операнда */
			if (isNumber(operands[0])){
				addAdress(out, memory);
				out += "= " + translateConst(operands[0]) + "\n";
				memory++;
				addAdress(out, memory);
				string temp = "";
				addAdress(temp, memory - 1);
				out += "LOAD " + temp + "\n"; //загрузить первый операнд в аккумулятор
				memory++;
			} else {
				addAdress(out, memory);
				out += "LOAD " + returnAdress(operands[0], table) + "\n";
				memory++;
			}
			/* Поиск значения второго операнда */
			string adr = "";
			if (isNumber(operands[2])){
				addAdress(out, memory);
				out += "= " + translateConst(operands[2]) + "\n";
				memory++;
				addAdress(adr, memory - 1);
			} else {
				adr = returnAdress(operands[2], table);
			}
			addAdress(out, memory);
			out += "SUB " + adr + "\n";
			memory++;
			if (operands[1] == "<"){
				addAdress(out, memory);
				string temp = "";
				addAdress(temp, memory + 2);
				out += "JNEG " + temp + "\n";
				memory++;
			}else if (operands[1] == "="){
				addAdress(out, memory);
				string temp = "";
				addAdress(temp, memory + 2);
				out += "JZ " + temp + "\n";
				memory++;
			}else if (operands[1] == ">"){
				addAdress(out, memory);
				string temp = "";
				addAdress(temp, memory + 2);
				out += "JNS " + temp + "\n";
				memory++;
			}
			addAdress(out, memory);
			out += "JUMP LA" + itos(line + 1) + " \n"; // переход будет выполнен, если в аккумуляторе >= 0 
			memory++;
			str = "";
			string temp = "";
			while (ss >> temp)
				str += temp + " ";
			strExist = 1;
			//line--;
			continue;
		}else if (str.find(statements[0]) != string::npos){ //если в строке требуется ввести значение переменной (INPUT)
			str.erase(str.begin(), str.begin() + str.find(statements[0]) + statements[0].length()); // оставляем только название переменной
			deleteSpaces(&str);		
			addAdress(out, memory);
			int check = varExist(str, table);
			if (check == 0){ // выделение памяти для переменной
				addToTable(str, memory, table);
				memory++;
				out += "= 16384\n"; //окончание выделение памяти переменной
			}
			addAdress(out, memory);
			out += "READ "; //команда ввода данных с устройства ввода-вывода
			out += returnAdress(str, table) + "\n"; //ввод операнда для "READ"
			if (strExist == 0)
				map[line] = memory;
			memory++;
		}else if (str.find(statements[1]) != string::npos){  // GOTO
			str.erase(str.begin(), str.begin() + str.find(statements[1]) + statements[1].length()); // оставляем только число, указывающее на строку
			deleteSpaces(&str);	
			if (strExist == 0)
				map[line] = memory;
			addAdress(out, memory); //добавляет номер строки в ассемблерный файл
			out += "JUMP ";
			out += "LA" + str; // добавляет номер строки к которой надо перейти
			out += " \n";
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
			if (strExist == 0)
				map[line] = memory;
			memory++;
		}else if (str.find(statements[4]) != string::npos){ // LET
			str.erase(str.begin(), str.begin() + str.find(statements[4]) + statements[4].length()); // оставляем только выражение
			if (strExist == 0)
				map[line] = memory;
			int err = 0;
			vector<string> RPN;
			err = toRPN(str, RPN);
			if (err == -1){
				cout << "Строка :" << line + 1 << endl;
				exit(EXIT_FAILURE);
			}
			err = translateExp(RPN, &memory, out, table);
			if (err == -1){
				cout << "Строка :" << line + 1 << endl;
				exit(EXIT_FAILURE);
			}
		}else if (str.find(statements[6]) != string::npos){ // END
			addAdress(out, memory);
			out += "HALT 00\n";
			map[line] = memory;
			break;
			memory++;
		}
		strExist = 0;
		line++;
	}
	while (out.find("LA") != string::npos){
		size_t pos1 = out.find("LA"); 
		char temp[4];
		out.copy(temp, 3, pos1 + 2);
		int adress = stoi(temp);
		string adr = "";
		if (map[adress] == -1){
			cout << "Оператор перехода указывает на неверную строку." << endl;
			exit(EXIT_FAILURE);
		}
		addAdress(adr, map[adress]);
		out.replace(pos1, 4, adr);
	}
	ofstream fout(argv[2]);
	if (!fout){
		cout << "Не удалось создать выходной файл." << endl;
		return 0;
	}
	fout << out;
	fout.close();
	cout << "Программа успешно скомпилирована!" << endl;
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
	if (str == "Acc"){
		for (unsigned int i = 0; i < vect.size(); i++)
			if (vect[i][0] == "Acc"){
				ostringstream ss;
				ss << adr;
				vect[i][1] = ss.str();
				return;
			}
	}
	vect.push_back(vector<string>(2));
	vect[vect.size() - 1][0] = str;
	ostringstream ss;
	ss << adr;
	vect[vect.size() - 1][1] = ss.str();
}

string addToTableConst(int adr, vector<vector<string> >& vect){
	string ret = "";
	int prev = -1;
	for (unsigned int i = 0; i < vect.size(); i++){
		if (vect[i][0].length() == 2)
			prev = i;
	}
	if (prev == -1){
		ret = "AA";
		
	} else{
		ret = vect[prev][0];
		ret[1]++;
	}
	vect.push_back(vector<string>(2));
	vect[vect.size() - 1][0] = ret;
	ostringstream ss;
	ss << adr;
	vect[vect.size() - 1][1] = ss.str();
	return ret;
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
	cout << "Size = " << table.size() << endl;
	for (unsigned int i = 0; i < table.size(); i++){
		cout << table[i][0].c_str() << "  " << table[i][1].c_str() << endl;
	}
}

void deleteSpaces(string * str){
	while (str->find(' ') != string::npos){
		str->erase(str->find(' '), 1);
	}
}

string returnAdress(char p1, vector<vector<string> >& vect, int * arr = NULL){
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
	string last; // в эту переменную нужно сохранить результат
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
			last = temp;
			if (isNumber(last)){ // значит слева от = константа - выражение не верное
				cout << "Слева от = константа, выражение неверно." << endl;
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
	out.push_back(last);
	return 0;
}

int translateExp(vector<string>& RPN, int * memory, string& out, vector<vector<string> >& table){
	int ret = *memory;
	string toThis = RPN[RPN.size() - 1];
	if (returnAdress(toThis, table) == "") // значит переменная, в которую надо сохранить значение, не объявленна
		createVar(toThis[0], table, memory, out);
	RPN.pop_back();
	if (RPN.size() == 1){ // если выражение вида a = b, где b может быть константой 
		if (isNumber(RPN[0])){
			addAdress(out, *memory);
			out += "= " + translateConst(RPN[0]) + "\n";
			(*memory)++;
			addAdress(out, *memory);
			string temp = "";
			addAdress(temp, *memory - 1);
			out += "LOAD " + temp + "\n";
			(*memory)++;
			addAdress(out, *memory);
			out += "STORE " + returnAdress(toThis, table) + "\n";
			(*memory)++;
			return ret;
		} else if (isalpha(RPN[0][0]) && !islower(RPN[0][0]) && RPN[0].length() == 1){
			addAdress(out, *memory);
			string temp = returnAdress(RPN[0], table);
			if (temp == "") // значит переменная не объявлена, объявляем и инициализируем
				temp = createVar(RPN[0][0], table, memory, out); // и объявляет их
			out += "LOAD " + temp + "\n";
			(*memory)++;
			addAdress(out, *memory);
			out += "STORE " + returnAdress(toThis, table) + "\n";
			(*memory)++;
			return ret;
		}else {
			cout << "Выражение составлено неверно." << endl;
			return -1;
		}
	}
	for (auto it = RPN.begin(); it < RPN.end(); it++){ // проверяет выражение на наличие необъявленных переменных
		if (isalpha((*it)[0]) && !islower((*it)[0])){
			string adr = returnAdress(*it, table);
			if (adr == "")
				adr = createVar((*it)[0], table, memory, out); // и объявляет их
		} else if (isNumber(*it)){
			string toRPN = "";
			toRPN = addToTableConst(*memory, table);
			addAdress(out, *memory);
			string tConst = "";
			tConst = translateConst(*it);
			if (tConst == ""){
				cout << "Не удалось перевести константу." << endl;
				exit(EXIT_FAILURE);
			}
			out += "= " + tConst + "\n";
			(*memory)++;
			*it = toRPN;
		}
	}
	for (auto it = RPN.begin(); it < RPN.end(); it++){
		if (isSign((*it)[0])){ // если знак, то пропускаем
			continue;
		} else if (isalpha((*it)[0]) && !islower((*it)[0])){ // если заглавная буква, значит это переменная
			string adr;
			if (!isSign((*(it + 1))[0])) // пропускаем символы, пока не встретим знак
				continue;
			else{
				adr = returnAdress(*(it - 1), table);
				addAdress(out, *memory);
				out += "LOAD " + adr + "\n";
				(*memory)++;
				addAdress(out, *memory);
				out += getCommand(*(it + 1));
				string addr = returnAdress(*it, table);
				int tm = stoi(addr);
				addr = "";
				addAdress(addr, tm);
				out += addr + "\n";
				(*memory)++;
				int i = 2;
				if (it + i >= RPN.end()){
					addAdress(out, *memory);
					out += "STORE " + returnAdress(toThis, table) + "\n";
					(*memory)++;
					break;
				}
				while (it + i < RPN.end() && isSign((*(it + i))[0])){
					string acc = "";
					if (*(it + i) == "/" || *(it + i) == "-"){ // значит необходимо поменять значение операнда и аккумулятора
						addAdress(out, *memory);
						out += "STORE "; // сохранить значение в пустой ячейке памяти
						addAdress(out, *memory);
						addAdress(acc, *memory);
						out += "\n";
						(*memory)++;
						addAdress(out, *memory);
						out += "LOAD " + returnAdress(*(it - i), table) + "\n"; // загрузить переменную, над которой будет выполняться операция в аккумулятор
						(*memory)++;
						addAdress(out, *memory);
						out += getCommand(*(it + i)) + acc + "\n"; // выполнить операцию
						(*memory)++;
						acc = "";
						i++;
						continue;
					}
					addAdress(out, *memory);
					out += getCommand(*(it + i)) + returnAdress(*(it - i), table) + "\n";
					(*memory)++;
					i++;
				}
				if (it + i >= RPN.end()){
					addAdress(out, *memory);
					out += "STORE " + returnAdress(toThis, table) + "\n";
					(*memory)++;
					break;
				}
				it = RPN.erase(it - i + 1, it + i);
				RPN.insert(it, "Acc");
				addAdress(out, *memory);
				(*memory)++;
				string tmp = "";
				addToTable("Acc", *memory, table);
				addAdress(tmp, *memory);
				out += "STORE " + tmp + "\n"; // сохранить значение аккумулятора в ячейке памяти
				addAdress(out, *memory);
				out += "= 16384\n";
				(*memory)++; // пропустить ячейку памяти, в которой будет храниться значение аккумулятора
			}
		}
	}
	return ret;
}

string translateConst(string p1){
	if ((stoi(p1) > 8191) || (stoi(p1) < -8191)){
		printf("Слишком большое значение.\n");
		return "";
	}
	int val = stoi(p1);
	if (val >= 0){
		val |= 0x4000;  //установка бита "не команда"
	}else{
		val = -val;
		val = ~val;
		val++;
		val |= 0x4000;
		val &= 32767;
	}
	return itos(val);
}

string getCommand(string c){
	if (c == "+")
		return "ADD ";
	if (c == "-")
		return "SUB ";
	if (c == "*")
		return "MUL ";
	if (c == "/")
		return "DIVIDE ";
	if (c == "|")
		return "OR ";
	return "";
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
	//cout << "Создал переменную - " << p1 << endl;
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
	if (str.length() == 1 && str[0] == '-')
		return 0;
	while ((isdigit(str[i]) || str[i] == '-') && i < str.length())
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
