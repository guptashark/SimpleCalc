#include <iostream> 
#include <string> 
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
	return a  - b;
}

	
int main(void) {

	map<string, int (*)(int, int)> functions;
	functions["*"] = fn_mult;
	functions["/"] = fn_div;
	functions["+"] = fn_add;
	functions["-"] = fn_sub;

	cout << "Simple Calculator" << endl;
	cout << "Version 1.0" << endl;

	cout << functions["/"](10, 2) << endl;





}


