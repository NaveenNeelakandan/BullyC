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



put all other rules here


let         {
                return TOK_LET;
            }

if          {
                return TOK_IF;
            }

";"         {
                return TOK_SEMICOLON;
            }

"("         {
                return TOK_OPENPAREN;
            }

")"         {
                return TOK_CLOSEPAREN;
            }

":="        {
                return TOK_ASSIGN;
            }

"=="        {
                return TOK_EQUALTO;
            }

and         {
                return TOK_AND;
            }

or          {
                return TOK_OR;
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


%%