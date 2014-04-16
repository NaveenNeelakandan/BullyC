#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

#include "lexer.h"              // token definitions


extern "C"                      // make the identifiers in lex.yy.c file accessible to C++ 
{
extern char     *yytext;
extern FILE     *yyin;
extern FILE     *yyout;
extern int      yyleng;

extern unsigned int     myLine;
extern unsigned long    myPos;      // For while loop processing

int yylex(void);
void yyrestart(FILE *);             // For while loop processing
}

typedef map<string, float> CSymbolMap;      // STL container type for managing variables (strings) and thier values (floats).


// Implementation GOLDEN rules
//      1. Production function names should be the same as the production names
//      2. A production function returns a value only if the produciton should return a value
//      3. Before calling a production function, make sure the current token is in the First set of that production
//      4. After taking action on a terminal/token, read past the token; production functions must read past all applicable tokens before returning.


// Production function declarations (prototypes)
void P(bool fExecute = true);
void S(bool fExecute = true);
void O(bool fExecute = true);
void G(bool fExecute = true);
void A(bool fExecute = true);
void W(bool fExecute = true);
void C(bool fExecute = true);
float E(bool fExecute = true);
float B(bool fExecute = true);
float R(bool fExecute = true);
float T(bool fExecute = true);
float F(bool fExecute = true);
float U(bool fExecute = true);

// Function declarations for checking whether the current token is in the first set of each production rule
bool IsFirstOfP(void);
bool IsFirstOfS(void);
bool IsFirstOfA(void);
bool IsFirstOfE(void);
bool IsFirstOfB(void);
bool IsFirstOfR(void);
bool IsFirstOfT(void);
bool IsFirstOfF(void);
bool IsFirstOfU(void);
bool IsFirstOfG(void);
bool IsFirstOfO(void);
bool IsFirstOfC(void);
bool IsFirstOfW(void);


int         iTok;           // The current token
CSymbolMap  SymbolTable;    // A set of (string, float) value pairs

// The follwing function is complete
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: rdp1 <program file>" << endl;
        return -1;
    }

    yyin = fopen(argv[1], "rt");
    if (yyin == NULL)
    {
        cout << "Error opening program file " << argv[1] << endl;
        return -1;
    }
    
    // Get the first token from the input program file.
    iTok = yylex();

    try
    {
        if (!IsFirstOfP())  // Check for '{'
            throw "Missing '{'";

        P(true);

        if (iTok != TOK_EOF)
            throw "End of file expected.  Extra statements will not be processed.";
    }
    catch(char *pszStr)
    {
        cerr << endl << argv[1] << "(" << myLine << "): " << pszStr << endl;
    }

    fclose(yyin);
	system("pause");
    return 0;
}

// This function P() is complete
void P(bool fExecute)
{
    // <P> --> '{' { <S> } '}'
    // We already know that the current token is a '{'
    iTok = yylex();     // Read past '{'

    while (IsFirstOfS())
    {
        S(fExecute);
    }

    if (iTok != TOK_CLOSEBRACE)  // Check for '}'
        throw "Missing '}'";
    iTok = yylex();     // Read past '}'

    return;
}

// This function S() is complete
void S(bool fExecute)
{
    // <S> --> <A> | <G> | <O> | <C> | <W>

    switch (iTok)
    {
    case TOK_LET:
        A(fExecute);
        break;

    case TOK_READ:
        G(fExecute);
        break;

    case TOK_PRINT:
        O(fExecute);
        break;

    case TOK_IF:
        C(fExecute);
        break;

    case TOK_WHILE:
        W(fExecute);
        break;

    default:
        throw "Syntax error -- unkown statement type.";
    }

    return;
}


// This function O() is complete
void O(bool fExecute)
{
    // <O> --> print [<STRINGLIT>] [ID] ;

    // We already know that iTok is TOK_PRINT.  Fetch the next token.
    iTok = yylex();     // Read past the 'print'

    if (iTok == TOK_STRINGLIT)
    {
        if (fExecute)
        {
            string s(yytext);

            s = s.substr(1, s.length() - 2);
            cout << s;
        }

        iTok = yylex(); // Read past the string literal
    }

    if (iTok == TOK_IDENTIFIER)
    {
        if (fExecute)
        {
            CSymbolMap::iterator    it = SymbolTable.find(yytext);

            if (it == SymbolTable.end())
                throw "Uninitialized variable used.";

            cout << it->second;
        }

        iTok = yylex(); // Read past the identifier
    }

    if (fExecute)
         cout << endl;

    if (iTok != TOK_SEMICOLON)      // Check for ';' at the end of the statement
        throw "Missing ';'";

    iTok = yylex();     // Read past the ';'
    return;
}


// This function G() is complete
void G(bool fExecute)
{
    // <R> --> read [STRINGLIT] ID ;

    // We already know that iTok is TOK_READ.  Fetch the next token.
    iTok = yylex();     // Read past 'read'

    if (iTok == TOK_STRINGLIT)
    {
        if (fExecute)
        {
            string s(yytext);

            s = s.substr(1, s.length() - 2);
            cout << s;
        }

        iTok = yylex(); // Read past the string literal
    }

    if (iTok == TOK_IDENTIFIER)
    {
        if (fExecute)
        {
            float                   rValue;
            CSymbolMap::iterator    it;

            cin >> rValue;

            it = SymbolTable.find(yytext);

            if (it == SymbolTable.end())
            {
                // This is a new varaible
                SymbolTable.insert(pair<string, float>(yytext, rValue));
            }
            else
            {
                // Change the value of the existing variable
                it->second = rValue;
            }
        }
    }
    else
        throw "Missing identifier in read statment.";

    iTok = yylex(); // Read past the identifier

    if (iTok != TOK_SEMICOLON)      // Check for ';' at the end of the statement
        throw "Missing ';'";

    iTok = yylex();     // Read past the ';'
    return;
}


// This function A() is partially implemented 
void A(bool fExecute)
{
    float                   rValue;
    string                  strId;
    // <A> --> let ID ':=' <E>;

    // We already know that iTok is TOK_LET.  Fetch the next token.
    iTok = yylex();     // Read past 'let'

    if (iTok != TOK_IDENTIFIER)
        throw "Missing identifier in assignment statement.";

    strId = yytext; // Save a copy of the variable identifier

    iTok = yylex(); // Read past the variable identifier

    if (iTok != TOK_ASSIGN)     // check for :=
        throw "Expecting ':='.";

    iTok = yylex(); // Read past the ':='
	
    if (IsFirstOfE())
        rValue = E(fExecute);
    else
        throw "Expecting an expression in the assignment statement.";

    if (fExecute)
    {
        CSymbolMap::iterator    it;
        
        it = SymbolTable.find(strId);

        if (it != SymbolTable.end())
			it->second = rValue;   // Change the value of the existing variable
        else
			SymbolTable.insert(pair<string, float>(strId, rValue));  // Add a new varaible to the symbol table
    }

    if (iTok != TOK_SEMICOLON)      // Check for ';' at the end of the statement
        throw "Missing ';'";

    iTok = yylex();     // Read past the ';'
    return;
}



void C(bool fExecute)
{
    // <C> --> if '('<E> ')'  <P> [ else <P> ]
    float rValue;   // Value of <E>
    bool  fValue;   // Is <E> true?

    // We already know that iTok is TOK_IF.  Fetch the next token.
    iTok = yylex();   // Read past 'if'


    if (iTok != TOK_OPENPAREN)     
        throw "Missing '('";  // Check for '('


    iTok = yylex();    // Read past '('

	if (IsFirstOfE())       // Check first of E
		rValue = E(fExecute);
    else
        throw "Expecting an expression in the assignment statement.";

    fValue = (rValue != 0.0);

    if (iTok != TOK_CLOSEPAREN)     
        throw "Missing ')'";  // Check for ')'

    iTok = yylex();   // Read past ')'

    if (iTok != TOK_OPENBRACE)     
        throw "Missing '{'"; // Check for '{'			

    if (fValue)
        P(fExecute);
    else
        P(false);
													
    if (iTok == TOK_ELSE)
    {
        iTok = yylex();    // Read past the else

        if (iTok != TOK_OPENBRACE)     
        throw "Missing '{'";   // Check for '{'

        if (fValue)
            P(false);
        else
            P(fExecute);
    }

    return;
}


void W(bool fExecute)
{
    // <W> --> while ( <E> ) <P>
    bool            fContinue = fExecute;
    unsigned long   ulPos =  myPos;
    unsigned int    uiLine = myLine;
    float           rValue;

    while (1)
    {
        // We know the first token is 'while'
        iTok = yylex();     // read past 'while'

        if (iTok != TOK_OPENPAREN)     
			throw "Missing '('";      // Check for '('

        iTok = yylex();    // Read past the '('

        if (IsFirstOfE())       // Check first of E
			rValue = E(fExecute);
	    else
			throw "Expecting an expression.";


        if (iTok != TOK_CLOSEPAREN)     
			throw "Missing ')'";      // Check for ')'

        iTok = yylex();      // Read past the ')'
        
        if (!IsFirstOfP())       // Check first of P
            throw "Expecting program statement";

        if (fExecute)
        {
            if (rValue != 0.0)
            {
                P(true);
                fseek(yyin, ulPos + uiLine - 1, SEEK_SET);
                myLine = uiLine;
                myPos = ulPos;
                yyrestart(yyin);
            }
            else
            {
                P(false);
                break;
            }
        }
        else
        {
            P(false);
            break;
        }
    }

}


// This function E() is complete
float E(bool fExecute)
{
    // <E> --> <B> {(and | or ) <B>}
    float   rValue1, rValue2;
    int     iOpTok;

    // We kanow the current token is in the First set of E.
    rValue1 = B(fExecute);


    while ((iTok == TOK_AND) || (iTok == TOK_OR))
    {
        iOpTok = iTok;      // Save a copy of the operator

        iTok = yylex();     // Read past the 'and' or 'or'

        if (!IsFirstOfB())
            throw "Syntax error in expression.";

        rValue2 = B(fExecute);

        if (fExecute)
        {
            if (iOpTok == TOK_AND)
                rValue1 = (float)((rValue1 != 0) && (rValue2 != 0));
            else
                rValue1 = (float)((rValue1 != 0) || (rValue2 != 0));			//shld be ||
        }
    }

    return rValue1;
}


// This function B() is complete
float B(bool fExecute)
{
    // <B> --> <R> [( '<' | '>' | '==') <R>]
    float   rValue1, rValue2;
    int     iOpTok;

    rValue1 = R(fExecute);

    if ((iTok == TOK_LESSTHAN) || (iTok == TOK_GREATERTHAN) || (iTok == TOK_EQUALTO))
    {
        iOpTok = iTok;      // Save the current operator

        iTok = yylex();     // Read past the '<' or '>' or '=='

        if (!IsFirstOfR())
            throw "Syntax error in expression.";

        rValue2 = R(fExecute);

        if (fExecute)
        {
            if (iOpTok == TOK_LESSTHAN)
                rValue1 = (float)(rValue1 < rValue2);
            else if (iOpTok == TOK_GREATERTHAN)
                rValue1 = (float)(rValue1 > rValue2);
            else
                rValue1 = (float)(rValue1 == rValue2);
        }
    }

    return rValue1;
}


float R(bool fExecute)
{
    // <R> --> <T> {( '+' | '-' ) <T>}

    float   rValue1, rValue2;
    int     iOpTok;

    // We kanow the current token is in the First set of R.
    rValue1 = T(fExecute);


    while ((iTok == TOK_PLUS) || (iTok == TOK_MINUS))
    {
        iOpTok = iTok;      // Save a copy of the operator

        iTok = yylex();     // Read past the 'plus' or 'minus'

        if (!IsFirstOfT())
            throw "Syntax error in expression.";

        rValue2 = T(fExecute);

        if (fExecute)
        {
            if (iOpTok == TOK_PLUS)
                rValue1 = (float)((rValue1) + (rValue2));
            else
                rValue1 = (float)((rValue1) - (rValue2));			
        }
    }

    return rValue1;
}


float T(bool fExecute)
{
    // <T> --> <F> {( '*' | '/' ) <F>}

	float   rValue1, rValue2;
    int     iOpTok;

    // We kanow the current token is in the First set of R.
    rValue1 = F(fExecute);


    while ((iTok == TOK_MULTIPLY) || (iTok == TOK_DIVIDE))
    {
        iOpTok = iTok;      // Save a copy of the operator

        iTok = yylex();     // Read past the 'multiply' or 'divide'

        if (!IsFirstOfF())
            throw "Syntax error in expression.";

        rValue2 = F(fExecute);

        if (fExecute)
        {
            if (iOpTok == TOK_MULTIPLY)
                rValue1 = (float)((rValue1) * (rValue2));
            else
                rValue1 = (float)((rValue1) / (rValue2));			
        }
    }

    return rValue1;
}


float F(bool fExecute)
{
	//F -> [not | -] U
	float   value;
    int     iOpTok=0;

    if ((iTok == TOK_NOT) || (iTok == TOK_MINUS))
    {
        iOpTok = iTok;      // Save a copy of the operator

        iTok = yylex();     // Read past the 'not' or '-'

    }

	if (!IsFirstOfU())
        throw "Syntax error in expression.";
	value = U(fExecute);

	if(fExecute){
		if(iOpTok==TOK_NOT){
			value = (value==0.0); 
		}
		if(iOpTok==TOK_MINUS)
			value = -value; 
	}
    return value;
}


float U(bool fExecute)
{
	float result=0;
	//U -> (E) | ID | FLOATLIT
	CSymbolMap::iterator it;
    string  strId;
	switch (iTok)
    {

    case TOK_OPENPAREN:
		iTok = yylex();				//read past '('

		if (!IsFirstOfE())
			throw "Expected an expression.";
        
		result = E(fExecute);
		
		if (iTok != TOK_CLOSEPAREN)     
			throw "Missing ')'";      // Check for ')
		
		iTok = yylex();				 //read past ')'
        break;

    case TOK_IDENTIFIER:
		strId = yytext;
		if(fExecute){
			it = SymbolTable.find(strId);
			if(it==SymbolTable.end())
				throw "Undefined identifer found";
			result = it->second;
		}
		iTok = yylex();				 //read past 'Id'
        break;

    case TOK_FLOATLIT:
		if(fExecute)
			result = atof(yytext);
		iTok = yylex();				 //read past 'floatlit'
        break;

	default:
        throw "Syntax error -- unkown statement type.";

	}

	return result;
}




bool IsFirstOfP()
{
    return iTok == TOK_OPENBRACE;
}


bool IsFirstOfS(void)
{
    return IsFirstOfA() || IsFirstOfG() || IsFirstOfO() || IsFirstOfC() || IsFirstOfW();
}


bool IsFirstOfA(void)
{
    return iTok == TOK_LET;
}


bool IsFirstOfE()
{
    return IsFirstOfB();
}


bool IsFirstOfB()
{
    return IsFirstOfR();
}


bool IsFirstOfR()
{
	return IsFirstOfT();
}


bool IsFirstOfT()
{
	return IsFirstOfF();
}


bool IsFirstOfF()
{
	return iTok == TOK_NOT || iTok == TOK_MINUS ||iTok == TOK_OPENPAREN||iTok == TOK_IDENTIFIER||iTok == TOK_FLOATLIT;
}


bool  IsFirstOfU()
{
	return iTok == TOK_OPENPAREN||iTok == TOK_IDENTIFIER||iTok == TOK_FLOATLIT;
}


bool  IsFirstOfG()
{
	return iTok == TOK_READ;
}

  
bool  IsFirstOfO()
{
	return iTok == TOK_PRINT;
}


bool  IsFirstOfC()
{
	return iTok == TOK_IF;
}


bool  IsFirstOfW()
{
	return iTok == TOK_WHILE;
}
