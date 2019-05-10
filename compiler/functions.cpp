#include "functions.hpp"

void deleteSpaces(string * str){
	while (str->find(' ') != string::npos){
		str->erase(str->find(' '), 1);
	}
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
			if (vect[i][1].length() < 2)
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

