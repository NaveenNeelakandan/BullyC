#include <fstream>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
using namespace std;

#define TOK_IF 1001
#define TOK_ELSE 1002
#define TOK_FOR 1003
#define TOK_WHILE 1004
#define TOK_PRINT 1005
#define TOK_RETURN 1006
#define TOK_CONTINUE 1007
#define TOK_BREAK 1008
#define TOK_DBUG 1009
#define TOK_READ 1010
#define TOK_INT 1100
#define TOK_FLOAT 1101
#define TOK_STRING 1102
#define TOK_SEMICOLON 2000
#define TOK_OPENPAREN 2001
#define TOK_CLOSEPAREN 2002
#define TOK_OPENBRACKET 2003
#define TOK_CLOSEBRAKET 2004
#define TOK_OPENBRACE 2005
#define TOK_CLOSEBRACE 2006
#define TOK_COMMA 2007
#define TOK_PLUS 3000
#define TOK_MINUS 3001
#define TOK_MULTIPLY 3002
#define TOK_DIVIDE 3003
#define TOK_ASSIGN 3004
#define TOK_EQUALTO 3005
#define TOK_LESSTHAN 3006
#define TOK_GREATERTHAN 3007
#define TOK_NOTEQUALTO 3008
#define TOK_AND 3009
#define TOK_OR 3010
#define TOK_NOT 3011
#define TOK_LENGTH 3012
#define TOK_IDENTIFIER 4000
#define TOK_INTLIT 4001
#define TOK_FLOATLIT 4002
#define TOK_STRINGLIT 4003
#define TOK_EOF 0
#define TOK_UNKNOWN 6000

extern "C"
{
int yylex();

extern FILE *yyin;
extern FILE *yyout;
extern char *yytext;
extern int yyleng;
extern unsigned int lineNumber;
}

int tok;
int returnType;
string functName;

void Expr(bool& failed);
void ImpreStmt(bool& failed);

//converts yytext into an intiger
int stti()
{
    int a = 0;
    for(int i = 0; i<yyleng; i++)
        a = a*10 + (int)yytext[i] - (int)'0';
    return a;
}

//yytext to Double Converter
float sttd()
{
    int dec = 0, ans = 0;
    int i = 0, j = 0;
    for(; yytext[i]!='.' && i<yyleng; i++)
        dec = dec*10 + (int)yytext[i] - (int)'0';
    i++;
    for(; i<yyleng; i++, j++)
        ans = ans*10 + (int)yytext[i] - (int)'0';
    return ( (double)dec + ( (double)ans / ( (double)pow(10.0, j) ) ) );
}

class Variable
{
public:
	string name;
	int type;
};

class intVariable: public Variable
{
public:
	int value;
	intVariable(string, int);
};
intVariable::intVariable(string n, int v = 0)
{
	this->name = n;
	this->value = v;

	this->type = TOK_INT;
}

class floatVariable: public Variable
{
public:
	float value;
	floatVariable(string, float);
};
floatVariable::floatVariable(string n, float v = 0.0)
{
	this->name = n;
	this->value = v;

	this->type = TOK_FLOAT;
}

class fnc
{
public:
	string name;
	vector<Variable*> args;
	int firstLine;

	int returnType;

	fnc();
	fnc(string n, int l, int rt);
};
fnc::fnc()
{
	this->name = "";
	this->firstLine = 0;
	this->returnType = TOK_INT;
}
fnc::fnc(string n, int l, int rt)
{
	this->name = n;
	this->firstLine = l;
	this->returnType = returnType;
}

class Instruction
{
public:
	int operation;
	string field;

	int ifield;
	float ffield;

	char usage;		//'n' - none, 'i' - int, 'f' - float, 's' - string

	string opName;
	
	Instruction(int o, string on);
	Instruction(int o, string on, string f);
	Instruction(int o, string on, int f);
	Instruction(int o, string on, float f);
	void Display();
};
Instruction::Instruction(int o, string on)
{
	this->operation = o;
	this->field  = "";
	this->opName = on;

	this->usage = 'n';
}
Instruction::Instruction(int o, string on, string f)
{
	this->operation = o;
	this->field  = f;
	this->opName = on;
	
	this->usage = 's';
}
Instruction::Instruction(int o, string on, int f)
{
	this->operation = o;
	this->ifield  = f;
	this->opName = on;
	
	this->usage = 'i';
}
Instruction::Instruction(int o, string on, float f)
{
	this->operation = o;
	this->ffield  = f;
	this->opName = on;
	
	this->usage = 'f';
}
void Instruction::Display()
{
	cout<<(this->opName);
	switch(this->usage)
	{
	case 'f':
		cout<<this->ffield<<endl;
		break;
	case 'i':
		cout<<this->ifield<<endl;
		break;
	case 's':
		cout<<this->field<<endl;
		break;
	default:
		cout<<endl;
		break;
	}
}
vector<Instruction> emittedCode;

map<string, Variable*> GlobalVariableTable;
map<string, Variable*> LocalVariableTable;
map<string, fnc*> FunctionSymbolTable;

//converts yytext into a string
string strStyle()
{
    string a = "";
    for(int i = 0+1*(yytext[0]==34); i<yyleng-1*(yytext[0]==34); i++)
        a += yytext[i];
    return a;
}

//checks if a variable has already been initialized in the local symbol table
bool localVariableCheck(string name, Variable* value = NULL)
{
	bool isValid = !(LocalVariableTable.insert(map<string, Variable*>::value_type(name,value)).second);
	if(!isValid)
		LocalVariableTable.erase(name);
	return isValid;
}

//checks if a variable has already been initialized in the global symbol table
bool globalVariableCheck(string name, Variable* value = NULL)
{
	bool isValid = !(GlobalVariableTable.insert(map<string, Variable*>::value_type(name,value)).second);
	if(!isValid)
		GlobalVariableTable.erase(name);
	return isValid;
}

//checks if a variable has already been initialized in the global symbol table
bool bothVariableCheck(string name, Variable* value = NULL)
{
	bool isValid = !(LocalVariableTable.insert(map<string, Variable*>::value_type(name,value)).second);
	if(!isValid)
	{
		LocalVariableTable.erase(name);
		isValid = !(GlobalVariableTable.insert(map<string, Variable*>::value_type(name,value)).second);
		if(!isValid)
			GlobalVariableTable.erase(name);
	}
	return isValid;
}

//checks if a variable has already been initialized in the symbol table
bool functionCheck(string name, fnc* value = NULL)
{
	bool isValid = !(FunctionSymbolTable.insert(map<string, fnc*>::value_type(name,value)).second);
	if(!isValid)
		FunctionSymbolTable.erase(name);
	return isValid;
}

void throwIDorExprException(string expected = "identifier or expression")
{
	ostringstream err;
	err<<"Expected "<<expected<<", but found '"<<strStyle()<<"'";
	throw(err.str());
}

void throwUndeclaredVariableException()
{
	ostringstream err;
	err<<"Undeclared variable: "<<strStyle();
	throw(err.str());
}

// <F> -> [-] ( '('Expr')' | ID | INT_LITERAL | FLOAT_LITERAL | ID'('[<Expr>{,<Expr>}]')' )
void F(bool& succeeded)
{
	//int minus = 1, 
	int answer = 0;
	succeeded = false;
	//checking for the optional '-'
	if (tok == TOK_MINUS)
	{
		tok = yylex();

		Instruction em(1005, "Negate");
		emittedCode.push_back(em);
	}
	
	// -> [-] INT_LITERAL
	if(tok == TOK_INTLIT)
	{
		succeeded = true;
		
		Instruction em(1010, "push ", stti());
		emittedCode.push_back(em);

		tok = yylex();
	}
	
	// -> [-] FLOAT_LITERAL
	if(tok == TOK_FLOATLIT)
	{
		succeeded = true;
		
		Instruction em(1010, "push ", sttd());
		emittedCode.push_back(em);

		tok = yylex();
	}
	
	// -> [-] ID [ '(' [ <Expr> { , <Expr> } ] ')' ]
	else if(tok == TOK_IDENTIFIER)
	{
		string name  = strStyle();
		tok = yylex();
		if(tok != TOK_OPENPAREN)	//at this point, we know its not a function call
		{
			if(bothVariableCheck(name))
			{
				succeeded = true;
		
				Instruction em(1010, "push ", name);
				emittedCode.push_back(em);
			}
			else
				throwUndeclaredVariableException();
		}
		else						//at this point, we know it is a function cal
		{
			tok = yylex();
			int argNum = 0;

			if(!functionCheck(name))
			{				
				ostringstream err;
				err<<"Undeclared Function: "<<name;
				throw(err.str());
			}

			while(tok!=TOK_CLOSEPAREN)
			{
				argNum++;
				Expr(succeeded);
				if (tok == TOK_COMMA)
					tok = yylex();
				else if(tok!=TOK_CLOSEPAREN)
					throwIDorExprException(")");
			}

			if(FunctionSymbolTable[name]->args.size() != argNum)
			{
				ostringstream err;
				err<<"There is no function: "<<name<<" with "<<argNum<<" argument(s).";
				throw(err.str());
			}
		
			Instruction em(1012, "Call ", name);
			emittedCode.push_back(em);

			tok = yylex();
		}
	}
	// -> [-]'('Expr')' 
	else if(tok == TOK_OPENPAREN)
	{
		tok = yylex();
		Expr(succeeded);
		if(tok != TOK_CLOSEPAREN)
			throw(string("Unbalanced parentheses"));
		succeeded = true;
		tok = yylex();
	}
}

// <T> -> <F> {(*|/) <F>}
void T(bool& succeeded)
{
	F(succeeded);
	if(!succeeded)
		throwIDorExprException();
	
	while(tok == TOK_MULTIPLY || tok == TOK_DIVIDE)
	{
		int mindiv = tok == TOK_MULTIPLY;
		tok = yylex();
		F(succeeded);
		if(!succeeded)
			throwIDorExprException();
		
		Instruction em( (mindiv)?1003:1004 , (mindiv)?"+":"-");
		emittedCode.push_back(em);
	}
}

// <C> -> <T> {(+|-) <T>}
void C(bool& succeeded)
{
	bool addsub;
	T(succeeded);
	if(!succeeded)
		throwIDorExprException();
	
	while(tok == TOK_PLUS || tok == TOK_MINUS)
	{
		addsub = tok == TOK_PLUS;
		tok = yylex();
		T(succeeded);
		if(!succeeded)
			throwIDorExprException();
		
		Instruction em( (addsub)?1001:1002 , (addsub)?"+":"-");
		emittedCode.push_back(em);
	}
}

// <Expr> -> ID [:= Expr | ['(' [<Expr> {, <Expr>}] ')'][ [(*|/)<T>] (+|-)<C>]{(<|>|<>|==)<C>}]
// <Expr> -> <C> {(<|>|<>|==) <C>}
void Expr(bool& succeeded)
{
	succeeded = false;
	int answer; 
	// -> ID [:= Expr | ['(' [<Expr> {, <Expr>}] ')'][ [(*|/)<T>] (+|-)<C>]{(<|>|<>|==)<C>}]
	if(tok == TOK_IDENTIFIER)
	{
		succeeded = true;
		string name = strStyle();
		tok = yylex();
		if(tok == TOK_ASSIGN)
		{
			tok = yylex();
			Expr(succeeded);

			if(!bothVariableCheck(name))
				throwUndeclaredVariableException();
			
			Instruction em(1000, "Assign ", name);
			emittedCode.push_back(em);
			return;
		}

		// ['(' [<Expr> {, <Expr>}] ')'][ [(*|/)<T>] (+|-)<C>]{(<|>|<>|==)<C>}]

		bool run = true;
		if(tok == TOK_OPENPAREN)
		{
			if(!functionCheck(name))
			{				
				ostringstream err;
				err<<"Undeclared Function: "<<name;
				throw(err.str());
			}

			tok = yylex();
			
			int argNum = 0;

			if(!functionCheck(name))
			{				
				ostringstream err;
				err<<"Undeclared Function: "<<name;
				throw(err.str());
			}
			while(tok == TOK_IDENTIFIER || tok == TOK_INTLIT || tok == TOK_OPENPAREN || tok == TOK_MINUS || tok == TOK_FLOATLIT)
			{
				argNum++;
				Expr(run);
				if(tok == TOK_COMMA)
					tok = yylex();
				else if(tok == TOK_CLOSEPAREN)
				{
					tok = yylex();
					break;
				}
				else
					throwIDorExprException(")");
			}

			if(FunctionSymbolTable[name]->args.size() != argNum)
			{
				ostringstream err;
				err<<"There is no function: "<<name<<" with "<<argNum<<" argument(s).";
				throw(err.str());
			}

			Instruction em(1012, "Call ", name);
			emittedCode.push_back(em);
		}
		else if(!bothVariableCheck(name))
			throwUndeclaredVariableException();
		else
		{
			Instruction em(1010, "Push ", name);
			emittedCode.push_back(em);
		}

		if(tok == TOK_MULTIPLY || tok == TOK_DIVIDE || tok == TOK_PLUS || tok == TOK_MINUS) //[ [ (*|/)<T> ] (+|-) <C> ]
		{
			if(tok == TOK_MULTIPLY || tok == TOK_DIVIDE)									//[ (*|/) <T> ]
			{
				if(tok == TOK_MULTIPLY)
				{
					tok = yylex();
					T(succeeded);
					if(!succeeded)
						throwIDorExprException();
					
					Instruction em(1003, "*");
					emittedCode.push_back(em);
				}
				else if(tok == TOK_DIVIDE)
				{
					tok = yylex();
					T(succeeded);
					if(!succeeded)
						throwIDorExprException();
					
					Instruction em(1004, "/");
					emittedCode.push_back(em);
				}
			}
			if(tok == TOK_PLUS)    //[+ <C>]
			{
				tok = yylex();
				C(succeeded);
				if(!succeeded)
					throwIDorExprException();

				Instruction em(1001, "+");
				emittedCode.push_back(em);
			}
			else if(tok == TOK_MINUS) //[- <C>]
			{
				tok = yylex();
				C(succeeded);
				if(!succeeded)
					throwIDorExprException();

				Instruction em(1002, "-");
				emittedCode.push_back(em);
			}
		}
		
		string symbol;
		int code;
		// {(<|>|<>|==) <C>}
		while(tok == TOK_GREATERTHAN || tok == TOK_LESSTHAN || tok == TOK_EQUALTO || tok == TOK_NOTEQUALTO)
		{
			switch(tok)
			{
			case TOK_GREATERTHAN:
				code = 1007;
				symbol = ">";
				break;
			case TOK_LESSTHAN:
				code = 1006;
				symbol = "<";
				break;
			case TOK_EQUALTO:
				code = 1008;
				symbol = "==";
				break;
			case TOK_NOTEQUALTO:
				code = 1009;
				symbol = "<>";
				break;
			}
			tok = yylex();
			C(succeeded);
			if(!succeeded)
				throwIDorExprException();

			Instruction em(code, symbol);
			emittedCode.push_back(em);
		}
	}
	else if(tok == TOK_OPENPAREN || tok == TOK_MINUS || tok == TOK_INTLIT || tok == TOK_FLOATLIT) //-> <C> {(<|>|<>|==) <C>}
	{
		succeeded = true;
		C(succeeded);
		
		string symbol;
		int code;
		// {(<|>|<>|==) <C>}
		while(tok == TOK_GREATERTHAN || tok == TOK_LESSTHAN || tok == TOK_EQUALTO || tok == TOK_NOTEQUALTO)
		{
			switch(tok)
			{
			case TOK_GREATERTHAN:
				code = 1007;
				symbol = ">";
				break;
			case TOK_LESSTHAN:
				code = 1006;
				symbol = "<";
				break;
			case TOK_EQUALTO:
				code = 1008;
				symbol = "==";
				break;
			case TOK_NOTEQUALTO:
				code = 1009;
				symbol = "<>";
				break;
			}

			tok = yylex();
			C(succeeded);

			Instruction em(code, symbol);
			emittedCode.push_back(em);
		}
	}
}

// <Expr_Stmt> -> (<AExpr> | <NExpr> | <MExpr>);
void ExprStmt(bool& failed)
{
	if(tok == TOK_SEMICOLON)
	{
		tok = yylex();
		return;
	}
	Expr(failed);

	if(!failed)
		return;

	if (tok!=TOK_SEMICOLON)
	{
		ostringstream err;
		err<<"Expected ; but found "<<strStyle();
		throw(err.str());
	}
	Instruction em( 1017 , "Clear Stack");
	emittedCode.push_back(em);
	tok = yylex();
}

//  <DeclStmt> -> int ID [:= INT_LITERAL] {, ID [:= INT_LITERAL]}
void DeclStmt(bool& failed, char table = 'l')
{
	string name;
	int value;
	float fvalue;
	bool enterLoop = false;
	if(tok != TOK_INT && tok != TOK_FLOAT)
	{
		failed = false;
		return;
	}
	returnType = tok;

	tok = yylex();
	if(tok != TOK_IDENTIFIER)
		throwIDorExprException("identifier");

	name = strStyle();
	tok = yylex();
	if (tok == TOK_OPENPAREN)
	{
		failed = false;
		functName = name;
		return;
	}
	enterLoop = true;

	while((tok == TOK_IDENTIFIER)||enterLoop)
	{
		if (!enterLoop)
		{
			name = strStyle();
			tok = yylex();
		}
		enterLoop = false;
		value = 0;
		if(tok == TOK_ASSIGN)
		{
			tok = yylex();
			if(returnType == TOK_INT && tok != TOK_INTLIT)
				throwIDorExprException("int literal");
			if(returnType == TOK_FLOAT && tok != TOK_FLOATLIT)
				throwIDorExprException("float literal");
			value = stti();
			fvalue = sttd();
			tok = yylex();
		}
		if( (table =='l')&&(localVariableCheck(name)) || (table == 'g')&&(globalVariableCheck(name)) )
			throw(string("Variable already declared"));

		if(table == 'l')
			LocalVariableTable[name] = NULL;
		else
		{
			Variable* v;
			if(returnType == TOK_INT)
				v = new intVariable(name, value);
			else if(returnType == TOK_FLOAT)
				v = new floatVariable(name, fvalue);

			GlobalVariableTable[name] = v;
		}

		if(tok == TOK_COMMA)
		{
			tok = yylex();
			if(tok != TOK_IDENTIFIER)
				throwIDorExprException("identifier");
		}
		else if(tok != TOK_SEMICOLON)
			throwIDorExprException(";");
	}
	if(tok == TOK_SEMICOLON)
		tok = yylex();
}

//  <PrintStmt> -> print [<PrintItem> {, <PrintItem>}];
void PrintStmt(bool& failed)
{
	tok = yylex();
	if(tok== TOK_SEMICOLON)
	{
		Instruction em( 1014 , "Print", "");
		emittedCode.push_back(em);
	}
	while(tok != TOK_SEMICOLON)
	{
		if(tok == TOK_COMMA)
		{
			tok = yylex();
		}

		if(tok == TOK_IDENTIFIER || tok == TOK_INTLIT || tok == TOK_OPENPAREN || tok == TOK_MINUS)
		{
			Expr(failed);
			Instruction em(1014, "Print");
			emittedCode.push_back(em);
		}
		else if(tok == TOK_STRINGLIT)
		{
			
			Instruction em( 1014 , "Print ", strStyle());
			emittedCode.push_back(em);
			tok = yylex();
		}
		else
			throwIDorExprException("string literal or identifier");

		if(tok != TOK_SEMICOLON && tok != TOK_COMMA)
			throwIDorExprException(";");
	}
	tok = yylex();
}

//  <ReadStmt> -> read [ID];
void ReadStmt(bool& succeeded)
{
	succeeded = false;
	tok = yylex();
	if(tok == TOK_SEMICOLON)
	{
		Instruction em( 1015 , "Read ");
		emittedCode.push_back(em);
		succeeded = true;
		tok = yylex();
		return;
	}
	if(tok == TOK_IDENTIFIER)
	{
		if(bothVariableCheck(strStyle()))
		{
			Instruction em( 1015 , "Read ", strStyle());
			emittedCode.push_back(em);
		}
		else
			throwUndeclaredVariableException();

		if((tok = yylex())==TOK_SEMICOLON)
		{
			succeeded = true;
			tok = yylex();
			return;
		}
	}
	throwIDorExprException(";");
}

// <ReturnStmt> -> return <Expr>;
void ReturnStmt(bool& succeeded)
{
	if (tok != TOK_RETURN)
		throwIDorExprException("return");

	tok = yylex();
	Expr(succeeded);
	if(tok != TOK_SEMICOLON)
		throwIDorExprException(";");
	tok = yylex();

	Instruction em(1012, "Return");
	emittedCode.push_back(em);
}

// <Block> -> <ImprStmt> | '{' {<ImprStmt>} '}'
void Block(bool& succeeded)
{
	if (tok != TOK_OPENBRACE)
	{
		ImpreStmt(succeeded);

		if(succeeded)
			return;
	}
	else
	{
		bool run = true;
		tok = yylex();
		while (run)
			ImpreStmt(run);
		if(tok==TOK_CLOSEBRACE)
		{
			tok = yylex();
			return;
		}
	}
	ostringstream err;
	err<<"encountered unexpected token "<<strStyle();
	throw(err.str());
}

// <IfStmt> -> if '('<expr>')' <block> [else <block>]
void IfStmt(bool& succeeded)
{
	tok = yylex();
	
	if(tok!=TOK_OPENPAREN)
		throwIDorExprException("(");
	tok = yylex();
	Expr(succeeded);

	if(tok!=TOK_CLOSEPAREN)
		throwIDorExprException(")");

	int ifPos = emittedCode.size();

	Instruction em(1016, "If ", 0);
	emittedCode.push_back(em);

	tok = yylex();

	Block(succeeded);

	emittedCode.at(ifPos).ifield = emittedCode.size();

	if(tok == TOK_ELSE)
	{	
		Instruction em(1011, "Goto ", (int)(emittedCode.size()) );
		emittedCode.push_back(em);

		emittedCode.at(ifPos).ifield = emittedCode.size();
		tok = yylex();
		Block(succeeded);
	}
}

// <WhileStmt> -> while '(' <expr> ')' <block>
void WhileStmt(bool& succeeded)
{
	tok = yylex();

	int whilePos = emittedCode.size();

	if(tok != TOK_OPENPAREN)
		throwIDorExprException("(");
	tok = yylex();
	Expr(succeeded);

	if(tok!=TOK_CLOSEPAREN)
		throwIDorExprException(")");
	tok = yylex();
	
	int ifPos = emittedCode.size();
	Instruction em(1016, "If ", 0);
	emittedCode.push_back(em);

	Block(succeeded);

	Instruction om(1011, "Goto ", whilePos );
	emittedCode.push_back(om);

	emittedCode.at(ifPos).ifield = emittedCode.size();
}

//  <ImprStmt> -> (<PrintStmt> | <ReadStmt> | <ExprStmt>)
void ImpreStmt(bool& failed)
{
	failed = false;
	if(tok == TOK_PRINT)
	{
		failed = true;
		PrintStmt(failed);
	}
	else if(tok == TOK_READ)
	{
		failed = true;
		ReadStmt(failed);
	}
	else if(tok == TOK_IDENTIFIER || tok == TOK_INTLIT || tok == TOK_OPENPAREN || tok == TOK_MINUS || tok == TOK_FLOATLIT || tok == TOK_SEMICOLON)
	{
		failed = true;
		ExprStmt(failed);
	}
	else if(tok == TOK_IF)
	{
		failed = true;
		IfStmt(failed);
	}
	else if(tok == TOK_WHILE)
	{
		failed = true;
		WhileStmt(failed);
	}
	else
	{
		return;
	}
}

// <Function> -> int|float ID ([ int|float ID{, int|float ID} ]) '{' { <decl_stmt> }{ <impr_stmt> }<return_stmt> '}'
void Function(bool& succeeded, bool& midParse)
{

	succeeded = false;
	string name;
	int rtype, paramtype; 
	if(midParse)
	{
		name = functName;
		rtype = returnType;
		midParse = false;
	}
	else
	{
		if(tok == TOK_INT || tok == TOK_FLOAT)
			rtype = tok;
		else
			return;

		tok = yylex();

		if(tok != TOK_IDENTIFIER)
			throwIDorExprException("Invalid function name");
		else
			name = strStyle();

		tok = yylex();
		if(tok != TOK_OPENPAREN)
			throwIDorExprException("(");
	}
	tok = yylex();

	if(functionCheck(name))
		throw(string("Previously declared function"));

	FunctionSymbolTable[name] = new fnc(name, emittedCode.size(), rtype);

	while(tok != TOK_CLOSEPAREN)
	{
		if (tok != TOK_INT && tok != TOK_FLOAT)
			throwIDorExprException("'int' or 'float'");
		//add support for actually remembering the parameter data types here
		
		paramtype = tok;

		tok = yylex();

		if (tok != TOK_IDENTIFIER)
			throwIDorExprException("ID");
		//add code here for actually remembering the parameter name (local to function name)
		
		if(paramtype == TOK_INT)
			FunctionSymbolTable[name]->args.push_back(new intVariable(strStyle()));
		else
			FunctionSymbolTable[name]->args.push_back(new floatVariable(strStyle()));

		tok = yylex();

		if(tok == TOK_COMMA)
			tok = yylex();
		else if(tok != TOK_CLOSEPAREN)
			throwIDorExprException(")");
	}
	tok = yylex();

	if (tok != TOK_OPENBRACE)
		throwIDorExprException("{");

	tok = yylex();
	bool run = true;
	while (run)
		DeclStmt(run);
	run = true;
	while (run)
		ImpreStmt(run);
	run = true;
	ReturnStmt(run);

	if (tok != TOK_CLOSEBRACE)
		throwIDorExprException("}");

	tok = yylex();

	LocalVariableTable.clear();

	succeeded = true;
}

//	<Program> -> {<DeclStmt>}{ImperStmt}
void Program(bool& failed)
{
	bool run = true;
	while (run)
		DeclStmt(run, 'g');
	run = true;
	bool fstart = (tok == TOK_OPENPAREN);
	while (run)
		Function(run, fstart );
	if(tok!=TOK_EOF)
	{
		ostringstream err;
		err<<"encountered unexpected token "<<strStyle();
		throw(err.str());
	}
	failed = run;
}

int main(int argc, char** argv) {
    int iToK = 0;
    ofstream write;
    if(argc>1)
        yyin = fopen(argv[1], "rt");
    else
    {
        cout<<"enter CTRL+D to end transmission."<<endl;
        yyin = stdin;
    }
    if(argc==3)
        write.open(argv[2]);
		
	bool failed = true;
	tok = yylex();
	
	try{
		Program(failed);
		cout<<"Run successful!"<<endl;

		for(int i = 0; i<emittedCode.size(); i++)
		{
			cout<<i<<". ";
			emittedCode.at(i).Display();
		}
	}
	catch (string error){
		emittedCode.clear();
		cerr<<endl<<argv[1]<<" (line "<<lineNumber<<") Syntax Error: "<<error<<endl;
	}
	cout<<"Press any key to terminate"<<endl;
	getch();
    
    return 0;
}

