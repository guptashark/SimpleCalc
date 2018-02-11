#include <iostream> 
#include <vector>
#include <string> 
#include <sstream>
#include <map> 

using namespace std;

int fn_div(int a, int b) {
	if(b == 0) {
		cout << "Error: Second argument of division cannot be 0." << endl;
		return 0;
	} else {
		return a / b;
	}
}

int fn_mult(int a, int b) {
	return a * b;
}

int fn_add(int a, int b) {
	return a + b;
}

int fn_sub(int a, int b) {
	return a - b;
}

int fn_modulo(int a, int b) {
	return a % b;
}

void show_help(map<string, int (*)(int, int)> &f) {
	cout << endl;
	cout << "Simple Calc Help" << endl;
	cout << "To quit, type in \"quit\"" << endl;
	cout << "To view the help file, type in: \"help\"" << endl;
	cout << "This calculator is a prefix expression calculator." << endl;
	cout << "	Ex: * 3 4" << endl;
	cout << "You can use any of the operations below in this syntax: " << endl;
	cout << "These operations behave the same as in C." << endl;
	cout << " 	oper arg1 arg2" << endl;
	cout << endl;

	for(auto i = f.begin(); i != f.end(); i++) {
		cout << i->first << endl;
	}
	cout << endl;
}
	
int main(void) {

	map<string, int (*)(int, int)> functions;
	functions["*"] = fn_mult;
	functions["/"] = fn_div;
	functions["+"] = fn_add;
	functions["-"] = fn_sub;
	functions["%"] = fn_modulo;

	cout << "Simple Calculator" << endl;
	cout << "Version 1.0" << endl;


	while(true) {
	
		cout << "~> ";	
		string user_input;
		getline(cin, user_input);

		if(user_input == "quit") break;
		if(user_input.empty()) {
			cout << endl;
			break;
		}
		
		stringstream ss(user_input);
		vector<string> token_list;
		string token;
		while(ss >> token) token_list.push_back(token);

		string oper = *(token_list.begin());
		if(oper == "help") {
			show_help(functions);
		} else if(functions.find(oper) == functions.end()) {
			cout << "Error: \"" << oper << "\" ";
			cout << "is not a defined function." << endl;
		} else {
			int arg1 = stoi(token_list[1]);
			int arg2 = stoi(token_list[2]);
			cout << functions[oper](arg1, arg2) << endl;
		}
	}

		
	return 0;

}



