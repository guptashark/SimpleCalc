#include <iostream> 
#include <vector>
#include <string> 
#include <sstream>
#include <map> 

using namespace std;

int fn_div(int a, int b) {
	return a / b;
}

int fn_mult(int a, int b) {
	return a * b;
}

int fn_add(int a, int b) {
	return a + b;
}

int fn_sub(int a, int b) {
	return a + b;
}

	
int main(void) {

	map<string, int (*)(int, int)> functions;
	functions["*"] = fn_mult;
	functions["/"] = fn_div;
	functions["+"] = fn_add;
	functions["-"] = fn_sub;

	cout << "Simple Calculator" << endl;
	cout << "Version 1.0" << endl;

	bool quit_signal = false;

	while(! quit_signal) {
	
		cout << "~> ";	
		string user_input;
		getline(cin, user_input);
		stringstream ss(user_input);

		vector<string> token_list;
		string token;
		while(ss >> token) token_list.push_back(token);

		for(auto i = token_list.begin(); i != token_list.end(); i++) {
			cout << *i << endl;
		}
	}

		
	return 0;

}



