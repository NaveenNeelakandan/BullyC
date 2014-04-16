#include "header.h"
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
using namespace std;

//globals defined
extern FILE *yyin;
extern FILE *yyout;
extern char *yytext;
extern int yyleng;
int yylex();

int main(int argc, char *argv[]){
	
	//input from 1) file or 2)keyboard
	if (argc > 0)
		yyin = fopen(argv[1], "rt");
	else
		yyin = stdin;

	//output to file
	ofstream writeFile;
	writeFile.open("lexemes.txt");

	int iTok=0;
	while ((iTok = yylex()) != TOK_EOF){
		
		//do not report whitespace
		if(iTok==TOK_WHTSP)
			continue;
		
		//print yytext to file
		for(int i = 0; i<yyleng; i++)
		{
			writeFile<<yytext[i];
		}	
		writeFile<<" "<<iTok<<endl;
	}
	writeFile<<iTok;

system("pause");
return 0;}