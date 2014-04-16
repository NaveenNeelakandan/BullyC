%option noyywrap
%{
#include <malloc.h>
#include <process.h>
#include "lexer.h"

#define fileno _fileno

unsigned int myLine = 1;
%}

%%

[ \t]+

if          {
                return TOK_IF;
            }

else        {
                return TOK_ELSE;
            }
			
for   		{
				return TOK_FOR;	
			}
			
while   	{
				return TOK_WHILE;	
			}

print   	{
				return TOK_PRINT;	
			}			
			
return  	{
				return TOK_RETURN;	
			}

continue   	{
				return TOK_CONTINUE;	
			}

break   	{
				return TOK_BREAK;	
			}
			
debug   	{
				return TOK_DEBUG;	
			}
			
read   		{
				return TOK_READ;	
			}
			
int 	    {
                return TOK_INT;
            }
			
float   	{
				return TOK_FLOAT;	
			}
			
string   	{
				return TOK_STRING;	
			}
			
";"			{
				return TOK_SEMICOLON;
			}

"("			{
				return TOK_OPENPAREN;
			}			
")"   		{
				return TOK_CLOSEPAREN;	
			}
			
"["   		{
				return TOK_OPENBRACKET;	
			}
			
"]"   		{
				return TOK_CLOSEBRACKET;	
			}
			
"{"   		{
				return TOK_OPENBRACE;	
			}
			
"}"   		{
				return TOK_CLOSEBRACE;	
			}
			
","		   	{
				return TOK_COMMA;	
			}			

"+"         {
                return TOK_PLUS;
            }

"-"   		{
				return TOK_MINUS;	
			}

"*"   		{
				return TOK_MULTIPLY;	
			}
			
"/"   		{
				return TOK_DIVIDE;	
			}

":="   		{
				return TOK_ASSIGN;	
			}

"=="   		{
				return TOK_EQUALTO;	
			}

"<"   		{
				return TOK_LESSTHAN;	
			}
			
">"   		{
				return TOK_GREATERTHAN;	
			}			
			
"<>"        {
                return TOK_NOTEQUALTO;
            }

"&&"        {
                return TOK_AND;
            }
			
"||"        {
                return TOK_OR;
            }
			
"~"        	{
                return TOK_NOT;
            }			

length      {
                return TOK_LENGTH;
            }

[0-9]+\.[0-9]*  {
                    return TOK_FLOATLIT;
                }

\"[^"\n]*\" {
                return TOK_STRINGLIT;
            }

[0-9]+ 		{
                return TOK_INTLIT;
            }

[a-z A-Z_][a-z A-Z 0-9_]* {
							return TOK_IDENTIFIER;
						  }
						  
"<<EOF>>"   {
				return TOK_EOF;
			}

[\n]        {
                myLine++;
            }

.           {
                return TOK_UNKNOWN;
            }

%%