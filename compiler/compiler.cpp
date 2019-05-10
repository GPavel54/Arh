#include "functions.hpp"
using namespace std;

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

