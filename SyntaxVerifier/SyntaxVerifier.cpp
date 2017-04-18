/********************************************************************************************
*									SyntaxVerifier.cpp 										*
*																							*
*	DESCRIPTION: Verifies correct syntax in a document instance structure.					*
*																							*
*																							*
*	AUTHORS: Aaron Millikin	and Ben Heinrichs						START DATE: 4/18/2017	*
*********************************************************************************************/

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <string>
#include <map>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <stack>
using namespace std;
ifstream inFile;
ofstream outFile;
stack<string> domainStack;

// Trim both ends of string
static inline string &trim(string &s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(),
		not1(ptr_fun<int, int>(isspace))));
	s.erase(find_if(s.rbegin(), s.rend(),
		not1(ptr_fun<int, int>(isspace))).base(), s.end());
	return s;
}

// Evaluates current expression using set domain.
bool evaluateSet(string s) {
	int parenthesisCnt = 0, bracketCnt = 0, expCnt = 0, prevExpPos = -1, curExpPos = 0;

	// Determines number of '=' in expression
	expCnt = count(s.begin(), s.end(), '=') + count(s.begin(), s.end(), ';');
	if (expCnt > 0) {
		curExpPos = s.find_first_of('=');
	}

	// Evaluates each subexpression within a line, separated by '=' or ';'
	for (int i = 0; i <= expCnt; i++) {
		for (int j = prevExpPos + 1; j < curExpPos; j++) {

			// First character must be  '(' or '{'
			if (j == prevExpPos + 1) {
				if (s[j] == '(') {
					parenthesisCnt++;
				}
				else if (s[j] == '{') {
					bracketCnt++;
				}
				else {
					return false;
				}
			}
			else {
				if (isalnum(s[j])) {
					if (s[j - 1] != '{' || s[j - 1] != ',' || (bracketCnt == 0 && s[j - 1] == ',')) return false;
				}
				else if (s[j] == '(') {
					if (s[j - 1] != '+' || s[j - 1] != '*') return false;
					parenthesisCnt++;
				}
				else if (s[j] == ')') {
					if (parenthesisCnt == 0 || s[j - 1] != '}')	return false;
					parenthesisCnt--;
				}
				else if (s[j] == '{') {
					if (s[j - 1] != '+' || s[j - 1] != '*' || s[j - 1] != '(') return false;
					bracketCnt++;
				}
				else if (s[j] == '}') {
					if (bracketCnt == 0 || !isalnum(s[j - 1])) return false;
					bracketCnt--;
				}
				else if (s[j] == ',') {
					if (!isalnum(s[j - 1])) return false;
				}
				else if (s[j] == '+' || s[j] == '*') {
					if (s[j - 1] != ')' || s[j - 1] != '}')	return false;
				}
				else {
					return false;
				}
			}

			// If any unclosed parentheses or bracket or did not end with ')' or '}', invalid
			if (parenthesisCnt != 0 || bracketCnt != 0 || s[j] != ')' || s[j] != '}') return false;
		}

		// Checks for position of next '='
		prevExpPos = curExpPos;
		curExpPos = s.find_first_of('=', prevExpPos+1);
		if (curExpPos == -1) curExpPos = s.find_first_of(";", prevExpPos + 1);
		if (curExpPos == -1) curExpPos = s.length();
	}

	// If expression has not returned false yet, it is valid.
	return true;
}

// Evaluates current expression using set domain.
bool evaluateAlg(string s) {
	int parenthesisCnt = 0, expCnt = 0, prevExpPos = -1, curExpPos = 0;

	// Determines number of '=' in expression
	expCnt = count(s.begin(), s.end(), '=') + count(s.begin(), s.end(), ';');
	if (expCnt > 0) {
		curExpPos = s.find_first_of('=');
	}

	// Evaluates each subexpression within a line, separated by '=' or ';'
	for (int i = 0; i <= expCnt; i++) {
		for (int j = prevExpPos + 1; j < curExpPos; j++) {

			// First character must be a digit, '(', or '-' followed by a digit
			if (j == prevExpPos + 1) {
				if (!isdigit(s[j]) || s[j] != '(' || (s[j] != '-' && isdigit(s[j + 1]))) return false;
				if (s[j] = '(')	parenthesisCnt++;
			}
			else {
				if (isdigit(s[j])) {
					if (isdigit(s[j - 1]) || s[j - 1] == ')') return false;
				}
				else if (s[j] == '(') {
					if (isdigit(s[j - 1]) || s[j - 1] == ')') return false;
					parenthesisCnt++;
				}
				else if (s[j] == ')') {
					if (parenthesisCnt == 0 || !isdigit(s[j-1]) || s[j-1] != ')') return false;
					parenthesisCnt--;
				}
				else if (s[j] == '-') {
					if (!isdigit(s[j - 1]) || s[j - 1] != '(' || s[j-1] != ')');
				}
				else if (s[j] == '+' || s[j] == '*' || s[j] == '^') {
					if (!isdigit(s[j - 1]) || s[j - 1] != ')') return false;
				}
				else {
					return false;
				}
			}

			// If any unclosed parentheses, invalid
			if (parenthesisCnt != 0) return false;
		}

		// Checks for position of next '='
		prevExpPos = curExpPos;
		curExpPos = s.find_first_of('=', prevExpPos + 1);
		if (curExpPos == -1) curExpPos = s.find_first_of(";", prevExpPos + 1);
		if (curExpPos == -1) curExpPos = s.length();
	}

	// If expression has not returned false yet, it is valid.
	return true;
}

// Evaluates current expression using set domain.
bool evaluateBool(string s) {
	int parenthesisCnt = 0, expCnt = 0, prevExpPos = -1, curExpPos = 0;

	// Determines number of '=' in expression
	expCnt = count(s.begin(), s.end(), '=') + count(s.begin(), s.end(), ';');
	if (expCnt > 0) {
		curExpPos = s.find_first_of('=');
	}

	// Evaluates each subexpression within a line, separated by '=' or ';'
	for (int i = 0; i <= expCnt; i++) {
		for (int j = prevExpPos + 1; j < curExpPos; j++) {

			// First character must be a digit, '(', or '-' followed by a digit
			if (j == prevExpPos + 1) {
				if (s[j] != '0' || s[j] != '1' || s[j] != '(') return false;
				if (s[j] = '(')	parenthesisCnt++;
			}
			else {
				if (s[j] == '0' || s[j] == '1') {
					if (s[j - 1] == '0' || s[j - 1] == '1' || s[j - 1] == ')') return false;
				}
				else if (s[j] == '(') {
					if (s[j-1] != '(' || s[j - 1] != '+' || s[j - 1] != '*') return false;
					parenthesisCnt++;
				}
				else if (s[j] == ')') {
					if (parenthesisCnt == 0 || s[j - 1] != '0' || s[j-1] != '1' || s[j - 1] != ')') return false;
					parenthesisCnt--;
				}
				else if (s[j] == '+' || s[j] == '*') {
					if (!isdigit(s[j - 1]) || s[j - 1] != ')') return false;
				}
				else {
					return false;
				}
			}

			// If any unclosed parentheses, invalid
			if (parenthesisCnt != 0) return false;
		}

		// Checks for position of next '='
		prevExpPos = curExpPos;
		curExpPos = s.find_first_of('=', prevExpPos + 1);
		if (curExpPos == -1) curExpPos = s.find_first_of(";", prevExpPos + 1);
		if (curExpPos == -1) curExpPos = s.length();
	}

	// If expression has not returned false yet, it is valid.
	return true;
}

// Evaluates current expression using set domain.
bool evaluateStr(string s) {
	int parenthesisCnt = 0, expCnt = 0, prevExpPos = -1, curExpPos = 0;

	// Determines number of '=' in expression
	expCnt = count(s.begin(), s.end(), '=') + count(s.begin(), s.end(), ';');
	if (expCnt > 0) {
		curExpPos = s.find_first_of('=');
	}

	// Evaluates each subexpression within a line, separated by '=' or ';'
	for (int i = 0; i <= expCnt; i++) {
		for (int j = prevExpPos + 1; j < curExpPos; j++) {

			// First character must be a digit, '(', or '-' followed by a digit
			if (j == prevExpPos + 1) {
				if (!isdigit(s[j]) || s[j] != '(') return false;
				if (s[j] = '(')	parenthesisCnt++;
			}
			else {
				if (isdigit(s[j])) {
					if (isdigit(s[j]) || s[j - 1] == ')') return false;
				}
				else if (s[j] == '(') {
					if (s[j - 1] != '(' || s[j - 1] != '+' || s[j - 1] != '*') return false;
					parenthesisCnt++;
				}
				else if (s[j] == ')') {
					if (parenthesisCnt == 0 || !isdigit(s[j]) || s[j - 1] != ')') return false;
					parenthesisCnt--;
				}
				else if (s[j] == '+' || s[j] == '*') {
					if (!isdigit(s[j - 1]) || s[j - 1] != ')') return false;
				}
				else {
					return false;
				}
			}

			// If any unclosed parentheses, invalid
			if (parenthesisCnt != 0) return false;
		}

		// Checks for position of next '='
		prevExpPos = curExpPos;
		curExpPos = s.find_first_of('=', prevExpPos + 1);
		if (curExpPos == -1) curExpPos = s.find_first_of(";", prevExpPos + 1);
		if (curExpPos == -1) curExpPos = s.length();
	}

	// If expression has not returned false yet, it is valid.
	return true;
}

// Verify's a line's syntax
string verify(string s) {
	bool valid = false;

	// Checks if domain start/end, else evaluates expression at top of stack
	if (s.substr(0, 1) == "<" && s.substr(s.length()-1, 1) == ">") {
		s = s.substr(1, s.length() - 2);
		if (s == " sets " || s == " algebra " || s == " boolean " || s == " strings ") {
			domainStack.push(trim(s));
			return "";
		}
		else if (s == "/") {
			domainStack.pop();
			return "";
		}
		else {
			return "Error: Invalid Domain!";
		}
	}
	else {

		// Makes sure there is a valid domain on the stack to evaluate at
		if (domainStack.size() == 0) {
			return "Error: Expression provided without a domain.";
		}

		// Removes all whitespace from expression
		s.erase(remove_if(s.begin(), s.end(), isspace), s.end());

		// Evaluates the expression at domain on top of stack.
		if (domainStack.top() == "sets") {
			valid = evaluateSet(s);
		}
		else if (domainStack.top() == "algebra") {
			valid = evaluateAlg(s);
		}
		else if (domainStack.top() == "boolean") {
			valid = evaluateBool(s);
		}
		else if (domainStack.top() == "strings") {
			valid = evaluateStr(s);
		}

		if (valid) {
			return "The following expression is valid in the " + domainStack.top() + " domain: ";
		}
		else {
			return "The following expression is invalid in the " + domainStack.top() + " domain: ";
		}
	}
}

void prompt()
{
	cout << "Welcome to Aaron and Ben's Expression Syntax Verifier! Analyzes files and outputs ." << endl;
	cout << "Accepted input: SyntaxVerifier <infile> <outfile>" << endl;
}
int main(int argc, char* argv[]) {
	clock_t startTime = clock();
	double secondsElapsed;
	vector<string> memory;
	string line;

	// Verifies correct number of parameters provided
	if (argc != 2) {
		cout << "Incorrect number of arguments supplied." << endl;
		prompt();
		EXIT_FAILURE;
	}

	// Opens in file, checks if error.
	inFile.open(argv[1], ios::in);
	if (!inFile) {
		cout << "Can't open input file " << argv[2] << endl;
		prompt();
		EXIT_FAILURE;
	}

	// Opens out file, checks if error.
	outFile.open(argv[2], ios::out);
	if (!outFile) {
		cout << "Can't open output file " << argv[3] << endl;
		prompt();
		EXIT_FAILURE;
	}

	// Reads file into string vector
	while (getline(inFile, line)) {
		memory.push_back(trim(line));
	}

	// Iterates through each line and verifies it.
	// Returns blank for domain start/end lines, else returns valid/invalid evaluation
	// Will return an error if unable to perform evaluation
	for (auto i = memory.cbegin(); i != memory.cend() ; ++i) {
		line = verify(*i);
		if (line != "") {
			// Checks if error received during verification, 
			// otherwise writes evaluation to out file
			if (line.substr(0, 5) == "Error") {
				cout << line << endl;
				EXIT_FAILURE;
			}
			else {
				line = line + *i;
				outFile << line << endl;
			}
		}
	}

	secondsElapsed = (clock() - startTime) / CLOCKS_PER_SEC;
	cout << fixed << setprecision(3);
	cout << "Runtime: " << secondsElapsed << endl;

	EXIT_SUCCESS;
}

