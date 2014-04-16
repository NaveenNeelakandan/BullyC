%option noyywrap
%{
#include <malloc.h>
#include <process.h>
#include "lexer.h"

#define fileno _fileno

unsigned int myLine = 1;
unsigned long myPos = 0;

#define YY_USER_ACTION myPos += yyleng;
%}

%%

let         {
                return TOK_LET;
            }

if          {
                return TOK_IF;
            }

else        {
                return TOK_ELSE;
            }
			
and         {
                return TOK_AND;
            }

or          {
                return TOK_OR;
            }
			
			
not         {
                return TOK_NOT;
            }
			
			
while   	{
				return TOK_WHILE;	
			}

print   	{
				return TOK_PRINT;	
			}			


read   		{
				return TOK_READ;	
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
			

"{"   		{
				return TOK_OPENBRACE;	
			}
			
"}"   		{
				return TOK_CLOSEBRACE;	
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

"&&"        {
                return TOK_AND;
            }
			
"||"        {
                return TOK_OR;
            }
			
"~"        	{
                return TOK_NOT;
            }			

			
[_a-zA-Z][_a-zA-Z0-9]*  {
							return TOK_IDENTIFIER;
						}
			
[0-9]+(\.[0-9]*)?  {
                    return TOK_FLOATLIT;
                }

\"[^"\n]*\" {
                return TOK_STRINGLIT;
            }

[ \t]+
			
[\n]        {
                myLine++;
            }

.           {
                return TOK_UNKNOWN;
            }

"<<EOF>>"   {
				return TOK_EOF;
			}

%%