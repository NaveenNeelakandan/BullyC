#include <string>
#include <iostream>
#include "header.h"
using namespace std;

//set parameters
#define numberOfStates 97
#define noOfChars 255
#define stndAscii 127

//define global variables
FILE *yyin;
FILE *yyout;
char *yytext;
int yyleng;
int state = 0;
bool readingFirst = 1; 
int lastRead;

//2D array which acts as the finite state machine
int fsa[numberOfStates][noOfChars];
//array to translate states to the respective tokens
int statestotokens[numberOfStates];

//initializing fsa array
void init_fsa(){

	//intialize all to 0
	for(int i=0;i<numberOfStates;i++){
		for(int j=0;j<noOfChars;j++)
			fsa[i][j] = 0;
	}

	//starting state transitions
	int starttemp[95] = {0,95,94,95,95,95,80,95,62,63,71,69,68,70,93,72,92,92,92,92,92,92,92,92,92,92,73,61,77,75,78,95,95,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,64,95,65,95,95,95,91,35,27,40,4,8,91,91,2,91,91,85,91,91,91,16,91,21,55,91,91,91,11,91,91,91,66,82,67,84};
	for(int i=32;i<stndAscii;i++)
		fsa[0][i]=starttemp[i-32];
	fsa[0][147]=94;

	
	//keywords
	int keywordstemp[95]={0};
	for(int i=48;i<58;i++) keywordstemp[i-32]=91; for(int i=65;i<91;i++) keywordstemp[i-32]=91; for(int i=97;i<123;i++) keywordstemp[i-32]=91; keywordstemp[67]=1;
	
	for(int i=2;i<60;i++){
		for(int j=32;j<stndAscii;j++)
			fsa[i][j] = keywordstemp[j-32];
	}

	fsa[2][102]=3;fsa[2][110]=49;
	fsa[4][108]=5;
	fsa[5][115]=6;
	fsa[6][101]=7;
	fsa[8][108]=51;fsa[8][111]=9;
	fsa[9][114]=10;
	fsa[11][104]=12;
	fsa[12][105]=13;
	fsa[13][108]=14;
	fsa[14][101]=15;
	fsa[16][114]=17;
	fsa[17][105]=18;
	fsa[18][110]=19;
	fsa[19][116]=20;
	fsa[21][101]=22;
	fsa[22][97]=47;fsa[22][116]=23;
	fsa[23][117]=24;
	fsa[24][114]=25;
	fsa[25][110]=26;
	fsa[27][111]=28;
	fsa[28][110]=29;
	fsa[29][116]=30;
	fsa[30][105]=31;
	fsa[31][110]=32;
	fsa[32][117]=33;
	fsa[33][101]=34;
	fsa[35][114]=36;
	fsa[36][101]=37;
	fsa[37][97]=38;
	fsa[38][107]=39;
	fsa[40][101]=41;
	fsa[41][98]=42;
	fsa[42][117]=43;
	fsa[43][103]=44;
	fsa[47][100]=48;
	fsa[49][116]=50;
	fsa[51][111]=52;
	fsa[52][97]=53;
	fsa[53][116]=54;
	fsa[55][116]=56;
	fsa[56][114]=57;
	fsa[57][105]=58;
	fsa[58][110]=59;
	fsa[59][103]=60;

	
	//punctuation/operators
	fsa[73][61]=74;
	fsa[75][61]=76;
	fsa[77][62]=79;
	fsa[80][38]=81;
	fsa[82][124]=83;

	
	//length
	for(int i=85;i<91;i++){
		for(int j=32;j<stndAscii;j++)
			fsa[i][j] = keywordstemp[j-32];
	}
	fsa[85][101]=86;
	fsa[86][110]=87;
	fsa[87][103]=88;
	fsa[88][116]=89;
	fsa[89][104]=90;

	
	//identifier
	for(int j=32;j<stndAscii;j++)			
		fsa[91][j] = keywordstemp[j-32];
	fsa[91][99]=91;
	
	//int
	for(int j=48;j<58;j++)fsa[92][j] = 92;fsa[92][46]=93;	

	//float
	for(int j=48;j<58;j++)fsa[93][j] = 93;

	//string
	for(int j=0;j<127;j++)fsa[94][j] = 94;fsa[94][148]=96;fsa[94][34]=96;

};

//initialize statestotokens array
void init_statestotokens(){
	statestotokens[0]=TOK_WHTSP;
	statestotokens[1]=TOK_EOF;
	//IF STATES
	statestotokens[2]=TOK_IDENTIFIER;
	statestotokens[3]=TOK_IF;
	//ELSE STATES
	statestotokens[4]=TOK_IDENTIFIER;
	statestotokens[5]=TOK_IDENTIFIER;
	statestotokens[6]=TOK_IDENTIFIER;
	statestotokens[7]=TOK_ELSE;
	//FOR STATES
	statestotokens[8]=TOK_IDENTIFIER;
	statestotokens[9]=TOK_IDENTIFIER;
	statestotokens[10]=TOK_FOR;
	//WHILE STATES
	statestotokens[11]=TOK_IDENTIFIER;
	statestotokens[12]=TOK_IDENTIFIER;
	statestotokens[13]=TOK_IDENTIFIER;
	statestotokens[14]=TOK_IDENTIFIER;
	statestotokens[15]=TOK_WHILE;
	//PRINT STATES
	statestotokens[16]=TOK_IDENTIFIER;
	statestotokens[17]=TOK_IDENTIFIER;
	statestotokens[18]=TOK_IDENTIFIER;
	statestotokens[19]=TOK_IDENTIFIER;
	statestotokens[20]=TOK_PRINT;
	//RETURN STATES
	statestotokens[21]=TOK_IDENTIFIER;
	statestotokens[22]=TOK_IDENTIFIER;
	statestotokens[23]=TOK_IDENTIFIER;
	statestotokens[24]=TOK_IDENTIFIER;
	statestotokens[25]=TOK_IDENTIFIER;
	statestotokens[26]=TOK_RETURN;
	//CONTINUE STATES
	statestotokens[27]=TOK_IDENTIFIER;
	statestotokens[28]=TOK_IDENTIFIER;
	statestotokens[29]=TOK_IDENTIFIER;
	statestotokens[30]=TOK_IDENTIFIER;
	statestotokens[31]=TOK_IDENTIFIER;
	statestotokens[32]=TOK_IDENTIFIER;
	statestotokens[33]=TOK_IDENTIFIER;
	statestotokens[34]=TOK_CONTINUE;
	//BREAK STATES
	statestotokens[35]=TOK_IDENTIFIER;
	statestotokens[36]=TOK_IDENTIFIER;
	statestotokens[37]=TOK_IDENTIFIER;
	statestotokens[38]=TOK_IDENTIFIER;
	statestotokens[39]=TOK_BREAK;
	//DEBUG STATES
	statestotokens[40]=TOK_IDENTIFIER;
	statestotokens[41]=TOK_IDENTIFIER;
	statestotokens[42]=TOK_IDENTIFIER;
	statestotokens[43]=TOK_IDENTIFIER;
	statestotokens[44]=TOK_DEBUG;
	//READ STATES
	statestotokens[45]=TOK_IDENTIFIER;//Not used-- use 21
	statestotokens[46]=TOK_IDENTIFIER;//not used -- use 22
	statestotokens[47]=TOK_IDENTIFIER;
	statestotokens[48]=TOK_READ;
	//INT STATES (SHARES A STATE WITH IF)
	statestotokens[49]=TOK_IDENTIFIER;
	statestotokens[50]=TOK_INT;
	//FLOAT STATES (SHARES A STATE WITH FOR)
	statestotokens[51]=TOK_IDENTIFIER;
	statestotokens[52]=TOK_IDENTIFIER;
	statestotokens[53]=TOK_IDENTIFIER;
	statestotokens[54]=TOK_FLOAT;
	//STRING STATES
	statestotokens[55]=TOK_IDENTIFIER;
	statestotokens[56]=TOK_IDENTIFIER;
	statestotokens[57]=TOK_IDENTIFIER;
	statestotokens[58]=TOK_IDENTIFIER;
	statestotokens[59]=TOK_IDENTIFIER;
	statestotokens[60]=TOK_STRING;
	//PUNCTUATION STATES
	statestotokens[61]=TOK_SEMICOLON;
	statestotokens[62]=TOK_OPENPAREN;
	statestotokens[63]=TOK_CLOSEPAREN;
	statestotokens[64]=TOK_OPENBRACKET;
	statestotokens[65]=TOK_CLOSEBRACKET;
	statestotokens[66]=TOK_OPENBRACE;
	statestotokens[67]=TOK_CLOSEBRACE;
	statestotokens[68]=TOK_COMMA;
	//OPERATOR STATES
	statestotokens[69]=TOK_PLUS;
	statestotokens[70]=TOK_MINUS;
	statestotokens[71]=TOK_MULTIPLY;
	statestotokens[72]=TOK_DIVIDE;
	//:=
	statestotokens[73]=TOK_UNKNOWN;
	statestotokens[74]=TOK_ASSIGN;
	//==
	statestotokens[75]=TOK_UNKNOWN;
	statestotokens[76]=TOK_EQUALTO;
	statestotokens[77]=TOK_LESSTHAN;
	statestotokens[78]=TOK_GREATERTHAN;
	//SHARES A STATE WITH LESSTHAN
	statestotokens[79]=TOK_NOTEQUALTO;
	//&&
	statestotokens[80]=TOK_UNKNOWN;
	statestotokens[81]=TOK_AND;
	//||
	statestotokens[82]=TOK_UNKNOWN;
	statestotokens[83]=TOK_OR;
	statestotokens[84]=TOK_NOT;
	//LENGTH STATES
	statestotokens[85]=TOK_IDENTIFIER;
	statestotokens[86]=TOK_IDENTIFIER;
	statestotokens[87]=TOK_IDENTIFIER;
	statestotokens[88]=TOK_IDENTIFIER;
	statestotokens[89]=TOK_IDENTIFIER;
	statestotokens[90]=TOK_LENGTH;
	//GENERAL IDENTIFIER STATE
	statestotokens[91]=TOK_IDENTIFIER;
	//INT
	statestotokens[92]=TOK_INTLIT;
	//FLOAT
	statestotokens[93]=TOK_FLOATLIT;
	//STRING
	statestotokens[94]=TOK_UNKNOWN;
	statestotokens[96]=TOK_STRINGLIT;
	//UNKNOWN
	statestotokens[95]=TOK_UNKNOWN;

};

void initialize(){
	init_statestotokens();
	init_fsa();
};

//returns the token based on state
int token_lookup(int state){
	return statestotokens[state];
}


/*
 function yylex; 
 no parameters, 
 returns an integer token identifier value of the identified lexeme.
*/
int yylex(){

	//define local variables
	int token=0;
	int next_state=0;
	string lexeme="";
	yyleng=0;
	int ch=0;
	
	//check if reading for the first time
	if (readingFirst == true){ 
		initialize();
		ch=fgetc(yyin);
		readingFirst = false;
	}
	else
		ch=lastRead;
	
	//treat other line characters as whitespace
	if(ch==10||ch==9||ch==11||ch==13)
		ch=32;

	while(true){
	
		//check for end of file
		if(feof(yyin)){
			ch=0;
			token = TOK_EOF;
			break;
		}

		//lookup next state
		next_state = fsa[state][ch];

		//if next state is not 0, transition, update globals, get next character from input stream
		if (next_state != 0){
			state = next_state;
			lexeme+=(char)ch;
			yyleng++;
			ch = fgetc(yyin);
			if(ch==10||ch==9||ch==11||ch==13)
				ch==32;
			if(ch==-1){
				next_state=0;
			}
		}
		
		//if next state is 0, determine token and last read value and terminate loop
		if (next_state == 0)
		{
			token = token_lookup(state);
			state = next_state;
			if(ch==32)
				lastRead = fgetc(yyin);
			else
				lastRead = ch;
			break;
		}
	}
	
	//update global yytext
	delete []yytext;
    yytext = new char[yyleng];
    for(int i = 0; i<yyleng; i++)      
        yytext[i] = lexeme[i];
    
	return token;                       
}