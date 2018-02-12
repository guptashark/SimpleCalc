#include <iostream> 
#include <stack> 
#include <list>
#include <vector>
#include <string> 
#include <sstream>
#include <map> 

using namespace std;

class Data {
	private:
		string type;
	public:
	
		// needs to be virtual
		virtual string to_str() = 0;
	
};

map<string, Data *> defined_vars;

class DataInteger : public Data {
	private:	
		int i;

	public:
		DataInteger(int i): i(i) {};

		string to_str() {
			 return to_string(i); 
		};
};

class DataString : public Data {
	private:
		string s;

	public:
		DataString(string t) {
			unsigned int i = 1;
			while(i < t.length() - 1) {
				s.push_back(t[i]);
				i++;
			}
		}

		string to_str() {
			string ret = "\"";
			ret += s;
			ret += "\"";
			return ret;
		}
};

class DataList : public Data {
	private:
		list<Data *> l;
	public:
		// take in a vector, convert to list.
		DataList(vector<Data *> d) {
			
			for(auto i = d.begin(); i != d.end(); i++) {
				l.push_back(*i);
			}
		}

		string to_str() {
			string out = "(list ";
			auto iter = l.begin();

			for(unsigned int i = 0; i < l.size() - 1; i++) {
				out += (*iter)->to_str();
				out += " ";
				iter++;
			}

			out += (*iter)->to_str();
			out += ")"; 
			return out;
		}
};

class DataNull : public Data {
};

class ParseTreeNode {
	// lets have a pointer to the parent, 
	// so that we can traverse our tree. 
	public: 
		ParseTreeNode *parent;

		// This is to do the actual computation. 
		virtual Data *process_node() = 0;

		virtual string getLexeme() = 0;
};

// A parse tree node could just be a literal. 
class PTN_Literal: public ParseTreeNode {

	public:
	string lexeme;

	PTN_Literal(string l): lexeme(l) {};

	// potential issue - undefined identifier
	Data *process_node() {
		// could be in symbol table. 
		if(defined_vars.find(lexeme) != defined_vars.end()) {
			return defined_vars[lexeme];
		} else {
			if((lexeme[0] == '"') && (lexeme[lexeme.size() - 1] == '"')) {
				return (new DataString(lexeme));
			} 
			//cout << lexeme[0] << " " << lexeme[lexeme.size() - 1] << endl;

			// should do some checking... 
			// make sure all chars are digits
			return (new DataInteger(stoi(lexeme)));
		}
	}

	string getLexeme() { return lexeme; };
};

// Or, it could be an bracketed function call: 
class PTN_Function : public ParseTreeNode {

	public: 
	vector<ParseTreeNode *> args;

	void add_arg(ParseTreeNode *ptn) {
		args.push_back(ptn);
	}

	string getLexeme() { 
		// should throw an error... 
		return "";
	}

	Data *process_node() {

		vector<Data *> computed_args;

		// need to run a compute on all the args
		// save of course the first one, which 
		// is the function name. 
		auto i = args.begin();
		string function_name = (*i)->getLexeme();
		if(function_name == "define") {
			i++;
			// do special stuff
			string identifier = (*i)->getLexeme();
			i++;

			Data *stored_value = (*i)->process_node();
			defined_vars[identifier] = stored_value;

			return 0;
		} 

		i++;

		// this is not great... since now we've evaluated
		// before running the function itself... which 
		// doesn't work if it's an identifier that is 
		// currently going to be defined. 
		while(i != args.end()) {
			computed_args.push_back((*i)->process_node());
			i++;
		}

		// finally, we need to do the processing with 
		// the first string. Lets do just * and +. 

		Data *answer;

		if(function_name == "list") {
			answer = new DataList(computed_args);
			
		} 
		
		/*
		else if(function_name == "+") {
			//answer = 0;
			for(auto j = computed_args.begin(); j != computed_args.end(); j++) {
			//	answer += *j;
			}
		} else if(function_name == "*") {
		//	answer = 1;
			for(auto j = computed_args.begin(); j != computed_args.end(); j++) {
				answer *= *j;
			}
		} 
		*/
		return answer;
	}
};

PTN_Function *
generate_tree(vector<string> &token_list) {

	stack<PTN_Function *> nested;
	PTN_Function *start = new PTN_Function();
	nested.push(start);

	auto i = token_list.begin();

	while(i != token_list.end()) {
		if(*i == "(") {

			PTN_Function *add_me = new PTN_Function();
			nested.top()->add_arg(add_me);
			nested.push(add_me);

		} else if(*i == ")") {
			
			nested.pop();

		} else {
			PTN_Literal *add_me = new PTN_Literal(*i);
			nested.top()->add_arg(add_me);
		}

		i++;
	}
	
	return start;
}

// not really necessary at this time... 
// we can focus on implementing it later. 
void
tokenize_input(stringstream &input, vector<string> &tokens) {
	
	// all we need to do is parse the input and 
	// recognize tokens to put into the vector. 
	// mainly... brackets. 

	(void)input;
	(void)tokens;

	
}


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

		// need a more sophisticated way to put this 
		// stuff together, so that we don't need to 
		// do something like ( + 3 4 ) and can instead
		// do: (+ 3 4)
		string token;
		while(ss >> token) token_list.push_back(token);

		// process this token_list differently. 
		// For now, assume that *every* expression is bracketed, 
		// and that to print out the value of a variable, 
		// the user must produce a bracketed expr. 

		PTN_Function *command = generate_tree(token_list);
		
		Data *to_print = command->args[0]->process_node();
		//cout << command->args[0]->process_node() << endl;
		cout << to_print->to_str() << endl;
		// no error recovery - yet
	}

	return 0;
}
