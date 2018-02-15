#include <iostream> 
#include <stack> 
#include <list>
#include <vector>
#include <string> 
#include <sstream>
#include <map> 

using namespace std;

// this is dumb, 
// it's not recognizing type as a 
// base class member... wtf. 
class Data {
public:	
		// needs to be virtual
		virtual string to_str() = 0;
		virtual string getType() = 0;
	
};

class DataBool: public Data {
	private: 
		string type;
		bool val;

	public: 
		DataBool(bool val): type("bool"), val(val) {};
		string getType() {return type; };
		string to_str() {
			if(val) {
				return "#True";
			} else {
				return "#False";
			}
		}

		bool getData() {
			return val;
		}
};

class DataInteger : public Data {
	private:	
		
		string type;
		int i;

	public:
		DataInteger(int i): type("integer"), i(i) {};

		string to_str() {
			 return to_string(i); 
		};
		
		string getType() { return type; };

		int getData() {
			return i;
		}

};

class DataString : public Data {
	private:
		string type;	
		string s;
		
	public:
		DataString(string t): type("string") {
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

		string getType() { return type; };

};

class DataList : public  Data {
	private:
		string type;

		list<Data *> l;
	public:
		// take in a vector, convert to list.
		DataList(vector<Data *> d): type("list") {
			
			for(auto i = d.begin(); i != d.end(); i++) {
				l.push_back(*i);
			}
		}
		
		DataList(list<Data *> l): type("list"), l(l) {};

		string to_str() {
			string out = "(list ";
			auto iter = l.begin();
			if(l.size() != 0) {
				for(unsigned int i = 0; i < l.size() - 1; i++) {
					out += (*iter)->to_str();
					out += " ";
					iter++;
				}

				out += (*iter)->to_str();
				out += ")"; 
			} else {
				out += ")";
			}

			return out;
		}

		string getType() { return type; };
		list<Data *> getData() {
			return l;
		}

};

// since functions are first class values... 
class DataFunction : public Data {
	
	private:
		string type;
		Data *(*fn_ptr)(vector<Data *>);

	public:
		// ctor
		string getType() { return type; };

		Data *apply(vector<Data *> &args) {
			return fn_ptr(args);
		}

		// we need a ctor... lol
		DataFunction(Data *(fn_ptr)(vector<Data *>)):
			 fn_ptr(fn_ptr) {};

		string to_str() {
			return "function";
		}
};

class DataNull : public Data {
	private: 
		string type;
	public:

		DataNull(): type("null") {};
		string to_str() {
			return "";
		}
		string getType() { return type; };

		
};

map<string, Data *> defined_vars;

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
			
			Data *ret = new DataNull();
			return ret;
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
		// the first string. Lets do it! 

		Data *answer;

		if(function_name == "list") {
			answer = new DataList(computed_args);
			
		} else {
			Data *current_val = defined_vars[function_name];
			DataFunction *current = dynamic_cast<DataFunction *>(current_val);

			answer = current->apply(computed_args);
		}


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


// integer mult, mapped with *
Data *fn_mult(vector<Data *> args) {

	int answer = 1;
	for(auto i = args.begin(); i != args.end(); i++) {
	
		// we ensure that all args are ints
		if((*i)->getType() != "integer") {
			cout << "BAD type! Expected Ints!" << endl;
		}

		DataInteger *current = dynamic_cast<DataInteger *>(*i);
		
		answer *= current->getData();
		
	}

	DataInteger *ret = new DataInteger(answer);
	return ret;
}

Data *fn_add(vector<Data *> args) {

	int answer = 0;
	for(auto i = args.begin(); i != args.end(); i++) {
	
		// we ensure that all args are ints
		if((*i)->getType() != "integer") {
			cout << "BAD type! Expected Ints!" << endl;
		}

		DataInteger *current = dynamic_cast<DataInteger *>(*i);
		
		answer += current->getData();
		
	}

	DataInteger *ret = new DataInteger(answer);
	return ret;
}

Data *fn_sub(vector<Data *> args) {
	int answer = 0;
	auto i = args.begin();
	
	DataInteger *current = dynamic_cast<DataInteger *>(*i);
	answer = current->getData();

	i++;

	current = dynamic_cast<DataInteger *>(*i);
	answer = answer - current->getData();

	DataInteger *ret = new DataInteger(answer);
	return ret;
}

Data *fn_div(vector<Data *> args) {
	int answer = 0;
	auto i = args.begin();
	
	DataInteger *current = dynamic_cast<DataInteger *>(*i);
	answer = current->getData();

	i++;

	current = dynamic_cast<DataInteger *>(*i);
	answer = answer / current->getData();
		
	DataInteger *ret = new DataInteger(answer);
	return ret;
}

Data *fn_remainder(vector<Data *> args) {
	int answer = 0;
	auto i = args.begin();
	
	DataInteger *current = dynamic_cast<DataInteger *>(*i);
	answer = current->getData();

	i++;

	current = dynamic_cast<DataInteger *>(*i);
	answer = answer % current->getData();

	DataInteger *ret = new DataInteger(answer);
	return ret;
}

Data *fn_sqr(vector<Data *> args) {
	int answer = 0;
	auto i = args.begin();

	DataInteger *current = dynamic_cast<DataInteger *>(*i);
	answer = current->getData() * current->getData();

	return new DataInteger(answer);
}

Data *fn_even_q(vector<Data *> args) {
	auto i = args.begin();
	
	DataInteger *c = dynamic_cast<DataInteger *>(*i);
	if(c->getData() % 2 == 0) {
		return new DataBool(true);
	} else {
		return new DataBool(false);
	}
}


// note, this may be wrong, since 
// we may run into issues such as... 
Data *fn_cons(vector<Data *> args) {

	// assume types are correct
	auto i = args.begin();
	Data *to_add = *i;
	i++;
	DataList *current = dynamic_cast<DataList *>(*i);
	list<Data *> l = current->getData();
	l.push_front(to_add);
	DataList *ret = new DataList(l);
	return ret;
}

Data *fn_rest(vector<Data *> args) {
	
	auto i = args.begin();
	DataList *current = dynamic_cast<DataList *>(*i);
	list<Data *> l = current->getData();
	l.pop_front();
	DataList *ret = new DataList(l);
	return ret;
}

Data *fn_list_length(vector<Data *> args) {
	auto i = args.begin();
	DataList *current = dynamic_cast<DataList *>(*i);
	int length = current->getData().size();
	return new DataInteger(length);
}

// TODO
// Do we need a deep copy...  I mean... 
// starting to feel dicey
Data *fn_list_reverse(vector<Data *> args) {
	auto i = args.begin();

	DataList *source_ptr = dynamic_cast<DataList *>(*i);
	list<Data *> source = source_ptr->getData();	

	list<Data *> output;

	for(auto j = source.begin(); j != source.end(); j++) {
		Data *current = *j;	
		output.push_front(current);
	}
	
	return new DataList(output);
}


	

// Greater than or equal to	
Data *fn_geq(vector<Data *> args) {
	auto i = args.begin();
	DataInteger *first = dynamic_cast<DataInteger *>(*i);
	i++;
	DataInteger *second = dynamic_cast<DataInteger *>(*i);
	int f = first->getData();
	int s = second->getData();

	bool val;
	if(f >= s) {
		val = true;
	} else {
		val = false;
	}

	DataBool *ret = new DataBool(val);	
	return ret;
}

Data *fn_leq(vector<Data *> args) {
	auto i = args.begin();
	DataInteger *first = dynamic_cast<DataInteger *>(*i);
	i++;
	DataInteger *second = dynamic_cast<DataInteger *>(*i);
	int f = first->getData();
	int s = second->getData();

	bool val;
	if(f <= s) {
		val = true;
	} else {
		val = false;
	}

	DataBool *ret = new DataBool(val);	
	return ret;
}



Data *fn_gt(vector<Data *> args) {
	auto i = args.begin();
	DataInteger *first = dynamic_cast<DataInteger *>(*i);
	i++;
	DataInteger *second = dynamic_cast<DataInteger *>(*i);
	int f = first->getData();
	int s = second->getData();

	bool val;
	if(f > s) {
		val = true;
	} else {
		val = false;
	}

	DataBool *ret = new DataBool(val);	
	return ret;
}

Data *fn_lt(vector<Data *> args) {
	auto i = args.begin();
	DataInteger *first = dynamic_cast<DataInteger *>(*i);
	i++;
	DataInteger *second = dynamic_cast<DataInteger *>(*i);
	int f = first->getData();
	int s = second->getData();

	bool val;
	if(f < s) {
		val = true;
	} else {
		val = false;
	}

	DataBool *ret = new DataBool(val);	
	return ret;
}

Data *fn_build_list(vector<Data *> args) {
	auto i = args.begin();
	DataInteger *num_elements = dynamic_cast<DataInteger *>(*i);	

	i++;
	DataFunction *f = dynamic_cast<DataFunction *>(*i);

	list<Data *> l;

	for(int k = 0; k < num_elements->getData(); k++) {
		vector<Data *> f_args;
		f_args.push_back(new DataInteger(k));
		l.push_back(f->apply(f_args));
	}

	return new DataList(l);
}

Data *fn_filter(vector<Data *> args) {
	auto i = args.begin();
	DataFunction *f = dynamic_cast<DataFunction *>(*i);
	i++;

	DataList *source_ptr = dynamic_cast<DataList *>(*i);
	list<Data *> source = source_ptr->getData();	

	list<Data *> output;

	for(auto j = source.begin(); j != source.end(); j++) {
		Data *current = *j;	
		vector<Data *> f_args = {current};
		DataBool *in_list = dynamic_cast<DataBool *>(f->apply(f_args));
		if(in_list->getData()) {
			output.push_back(*j);
		}
	}
	
	return new DataList(output);
}



// foldr is a really weird one I don't yet want to do. 
// or mayme am unsure of how to do

Data *fn_map(vector<Data *> args) {
	auto i = args.begin();
	DataFunction *f = dynamic_cast<DataFunction *>(*i);
	i++;

	DataList *source_ptr = dynamic_cast<DataList *>(*i);
	list<Data *> source = source_ptr->getData();	

	list<Data *> output;

	for(auto j = source.begin(); j != source.end(); j++) {
		Data *current = *j;	
		vector<Data *> f_args = {current};
		Data *image = f->apply(f_args);
		output.push_back(image);
	}
	
	return new DataList(output);
}

Data *fn_list_empty(vector<Data *> args) {
	auto i = args.begin();
	DataList *l_ptr = dynamic_cast<DataList *>(*i);

	unsigned int len = l_ptr->getData().size();

	if(len == 0) {
		return new DataBool(true);
	} else {
		return new DataBool(false);
	}
}

Data *predicate_string(vector<Data *> args) {
	auto i = args.begin();

	if((*i)->getType() == "string") {
		return new DataBool(true);
	} else {
		return new DataBool(false);
	}
}

Data *predicate_list(vector<Data *> args) {
	auto i = args.begin();

	if((*i)->getType() == "list") {
		return new DataBool(true);
	} else {
		return new DataBool(false);
	}
}

Data *predicate_bool(vector<Data *> args) {
	auto i = args.begin();

	if((*i)->getType() == "bool") {
		return new DataBool(true);
	} else {
		return new DataBool(false);
	}
}


Data *fn_not(vector<Data *> args) {
	auto i= args.begin();

	DataBool *src = dynamic_cast<DataBool *>(*i);
	if(src->getData()) {
		return new DataBool(false);
	} else {
		return new DataBool(true);
	}
}

Data *fn_and(vector<Data *> args) {

	DataBool *ret;
	auto i = args.begin();
	
	while(i != args.end()) {
		DataBool *current = dynamic_cast<DataBool *>(*i);
		if(current->getData() == false) {
			ret = new DataBool(false);
			return ret;
		}
		i++;
	}

	ret = new DataBool(true);
	return ret;
}



Data *fn_or(vector<Data *> args) {

	DataBool *ret;
	auto i = args.begin();
	
	while(i != args.end()) {
		DataBool *current = dynamic_cast<DataBool *>(*i);
		if(current->getData() == true) {
			ret = new DataBool(true);
			return ret;
		}
		i++;
	}

	ret = new DataBool(false);
	return ret;
}


Data *fn_if(vector<Data *> args) {
	auto i = args.begin();
	DataBool *condition = dynamic_cast<DataBool *>(*i);
	i++;
	Data *first = *i;
	i++;
	Data *second = *i;

	
	if(condition->getData()) {
		return first;
	} else {
		return second;
	}
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
	/////////////////////////
	defined_vars["*"] = new DataFunction(fn_mult);
	defined_vars["+"] = new DataFunction(fn_add);
	defined_vars["-"] = new DataFunction(fn_sub);
	defined_vars["/"] = new DataFunction(fn_div);
	defined_vars["remainder"] = new DataFunction(fn_remainder);

	defined_vars["even?"] = new DataFunction(fn_even_q);
	defined_vars["string?"] = new DataFunction(predicate_string);
	defined_vars["list?"] = new DataFunction(predicate_list);
	defined_vars["bool?"] = new DataFunction(predicate_bool);

	defined_vars["sqr"] = new DataFunction(fn_sqr);
	
	defined_vars["cons"] = new DataFunction(fn_cons);
	defined_vars["rest"] = new DataFunction(fn_rest);
	defined_vars["length"] = new DataFunction(fn_list_length);
	defined_vars["empty?"] = new DataFunction(fn_list_empty);
	defined_vars["reverse"] = new DataFunction(fn_list_reverse);
	defined_vars["build_list"] = new DataFunction(fn_build_list);
	defined_vars["filter"] = new DataFunction(fn_filter);
	defined_vars["map"] = new DataFunction(fn_map);

	defined_vars[">="] = new DataFunction(fn_geq);	
	defined_vars["<="] = new DataFunction(fn_leq);
	defined_vars[">"] = new DataFunction(fn_gt);
	defined_vars["<"] = new DataFunction(fn_lt);

	defined_vars["not"] = new DataFunction(fn_not);
	defined_vars["and"] = new DataFunction(fn_and);
	defined_vars["or"] = new DataFunction(fn_or);


	defined_vars["if"] = new DataFunction(fn_if);

	
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
		cout << to_print->getType() << endl;
		// no error recovery - yet
	}

	return 0;
}
