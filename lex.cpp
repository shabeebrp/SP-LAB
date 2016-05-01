#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include<fstream>
#include<algorithm>
using namespace std;
int line_no = 1;

vector<string> delims = { "{", "}", "[", "]", ",", ";", "." , "(", ")" };
vector<string> operators = { "+", "-", "*", "/"};
vector<string> relOps = { "<", ">", "!",">=","<=","!=" };
vector<string> logOps = { "&", "|" };
vector <string> assignment = { "=","+=","-=","*=","/=" };
vector<string> keyword = {
	"int", "float", "double", "char", "void",
	"while", "for", "do", "if", "return"
};
bool isDelim(string token) {
	return(find(delims.begin(), delims.end(), token) != delims.end());
}
bool isOperator(string token) {
	return(find(operators.begin(), operators.end(), token) != operators.end());
}
bool isRelOp(string token) {
	return(find(relOps.begin(), relOps.end(), token) != relOps.end());
}
bool isLogOp(string token) {
	return(find(logOps.begin(), logOps.end(), token) != logOps.end());
}
bool isKeyword(string token) {
	return(find(keyword.begin(), keyword.end(), token) != keyword.end());
}
bool isAssign(string token) {
	return(find(assignment.begin(), assignment.end(), token) != assignment.end());
}
bool isNumber(string token) {
	for (auto i = token.begin(); i != token.end(); i++)
		if (!isdigit(*i))
			return false;
	return true;
}
void print(string token) {
	if (token.size() == 0)
		return;
	if (isDelim(token))
		cout << "Delim\t:\t" << token << endl;
	else if (isOperator(token)) 
		cout << "Arithm\t:\t" << token << endl;
	else if (isRelOp(token))
		cout << "Relatop\t:\t" << token << endl;
	else if (isLogOp(token))
		cout << "Logical\t:\t" << token << endl;
	else if (isKeyword(token))
		cout << "keyword\t:\t" << token << endl;
	else if (isAssign(token))
		cout << "Assign\t:\t" << token << endl;
	else if (isNumber(token))
		cout << "Number\t:\t" << token << endl;
	else if (isalpha(token[0]) || token[0] == '_')
		cout << "ident\t:\t" << token << endl;
	else
		cout << "Invalid\t:\t" << token << endl;

}
void analyze(string token) {
	//cout << token << endl;
	string current = "";
	for (int i = 0; i < token.size(); i++)
		if (isalnum(token[i]) || token[i]=='_')
			current += string(1, token[i]);
		else {//if token[i] is not alpha numeric
			print(current);
			current = string(1, token[i]);
			if (isRelOp(string(1, token[i])) || isOperator(string(1, token[i]))) {//to check <=,>=,!=,*=,+=,/=,-=
				if (token[i + 1] == '=');
				current += string(1, token[++i]);
			}
			print(current);
			current = "";
		}
		print(current);
}

int main() {
	ifstream fin("lex.txt");
	string line;
	while (getline(fin, line)) {
		stringstream ss(line);
		string token;
		cout << "Line " << line_no << "\n-------------------\n";
		while (getline(ss, token, ' '))
			analyze(token);
		line_no++;
		cout << endl;
	}
	return 0;
}
