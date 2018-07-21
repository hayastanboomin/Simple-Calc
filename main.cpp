// Expression calc
// Using -std=c++14
//it's a simple comment from hayastanboomin

#include <iostream>
#include <string>
#include <stdexcept>
#include <stack>     
#include <algorithm> 
#include <utility>   
#include <cctype>    
#include <cmath>




using num_t = double;  // number type
using act_t = char;    // operation type
using num_stack_t = std::stack<num_t>;
using act_stack_t = std::stack<act_t>;
	
const char *error_msg = "this expression can't be evaluated";
const char UNARY_PLUS = '#';
const char UNARY_MINUS = '&';
	
num_t calc2(num_t a, num_t b, act_t act) {
	switch (act) {
		case '+': return a + b;
		case '-': return a - b;
		case '*': return a * b;
		case '/': return a / b;
		case '^': return pow(a, b);
		default: throw std::invalid_argument("Unknown operation");
	}
}
	
void make_action(num_stack_t &num_stack, act_stack_t &act_stack) 
{
	if (act_stack.empty()) 
		throw std::invalid_argument(error_msg);
	if (act_stack.top() == UNARY_MINUS || act_stack.top() == UNARY_PLUS) {
		if (act_stack.top() == UNARY_MINUS)
			num_stack.top() *= -1;
		act_stack.pop();
		return;
	}
	if (num_stack.size() < 2U) 
		throw std::invalid_argument(error_msg);
	num_t b = num_stack.top();
	num_stack.pop();
	num_t a = num_stack.top();
	num_stack.pop();
	act_t act = act_stack.top();
	act_stack.pop();
	num_stack.push(calc2(a, b, act));
}

bool isnumchar(char c) {
	return isdigit(c) || c == '.';
}

bool isactchar(char c) {
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

bool isunary(char c) {
	return c == '+' || c == '-';
}
	
int priority(act_t act) {
	switch (act) {
		case '^': return 4;
		case UNARY_PLUS: case UNARY_MINUS: return 3;
		case '*': case '/': return 2;
		case '+': case '-': return 1;
		default: return 0;
	} 
}

num_t parse_num(const std::string &snum) {
	size_t error_pos;
	num_t num = std::stod(snum, &error_pos);
	if (error_pos != snum.size())
		throw std::invalid_argument(error_msg);
	return num;
}

num_t eval(std::string s) 
{
	if (s.empty()) throw std::invalid_argument(error_msg);
	num_stack_t num_stack;
	act_stack_t act_stack;
	size_t num_pos{};
	size_t num_len{};
	bool unary{true};
	for (size_t i = 0; i < s.size(); i++) 
	{
		if (isnumchar(s[i])) {
			if (!num_len) num_pos = i;
			num_len++;
			if (i == s.size()-1U || !isnumchar(s[i+1])) {
				num_stack.push(parse_num(s.substr(num_pos, num_len)));
				num_len = 0;
			}
			unary = false;
		}
		else if (unary && isunary(s[i])) {
			if (s[i] == '-')
				act_stack.push(UNARY_MINUS);
			else
				act_stack.push(UNARY_PLUS);
		}
		else if (isactchar(s[i])) {
			while (!act_stack.empty() && priority(act_stack.top()) >= priority(s[i])) 
				make_action(num_stack, act_stack);
			act_stack.push(s[i]);
			unary = true;
		}
		else if (s[i] == '(')
			act_stack.push('('), unary = true;
		else if (s[i] == ')') {
			while (!act_stack.empty() && act_stack.top() != '(') 
				make_action(num_stack, act_stack);
			if (act_stack.empty()) 
				throw std::invalid_argument(error_msg);
			act_stack.pop();
			unary = false;
		}
		else throw std::invalid_argument(error_msg);
	}
	while (num_stack.size() > 1U)
		make_action(num_stack, act_stack);
	while (act_stack.size() && 
	      (act_stack.top() == UNARY_MINUS || act_stack.top() == UNARY_PLUS)) 
	{
		if (act_stack.top() == UNARY_MINUS)
			num_stack.top() *= -1;
		act_stack.pop();
	}
	if (!act_stack.empty() || num_stack.size() != 1U) 
		throw std::invalid_argument(error_msg);
	return num_stack.top();
}



std::string& make_lower(std::string &s) {
	for (auto &c: s) c = tolower(c);
	return s;
}

// '[', '{' -> '('
// ']', '}' -> ')'
// ','      -> '.'
void change_to_default_symb(std::string &s) {
	for (auto &c: s)
		if (c == '[' || c == '{') c = '(';
		else if (c == ']' || c == '}') c = ')';
		else if (c == ',') c = '.';
}

int main() {
	std::string s;
	std::cout << "Enter expression: ";
	while (std::getline(std::cin, s) && make_lower(s) != "exit") {
		s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
		std::cout << "Result: ";
		if (s == "about")
			std::cout << "program was written by Andrey Moskalenko";
		else
		try {
			std::cout << eval(s);
		}
		catch (std::invalid_argument &e) {
			std::cout << e.what();
		}
		std::cout << "\n\nEnter expression: ";
	}
}

