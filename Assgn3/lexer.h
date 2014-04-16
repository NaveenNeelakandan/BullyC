#ifndef __LEXER_H
#define __LEXER_H


#define TOK_LET             1001
#define TOK_IF              1002
#define TOK_ELSE            1003
#define TOK_WHILE           1004
#define TOK_PRINT           1005
#define TOK_READ            1006
#define TOK_SEMICOLON       2001
#define TOK_OPENPAREN       2002
#define TOK_CLOSEPAREN      2003
#define TOK_OPENBRACE       2004
#define TOK_CLOSEBRACE      2005
#define TOK_PLUS            3001
#define TOK_MINUS           3002
#define TOK_MULTIPLY        3003
#define TOK_DIVIDE          3004
#define TOK_ASSIGN          3005
#define TOK_EQUALTO         4001
#define TOK_LESSTHAN        4002
#define TOK_GREATERTHAN     4003
#define TOK_AND             4004
#define TOK_OR              4005
#define TOK_NOT             4006
#define TOK_FLOATLIT        5001
#define TOK_STRINGLIT       5002
#define TOK_IDENTIFIER      5003
#define TOK_EOF             0
#define TOK_UNKNOWN         6000


#endif  // #ifndef __LEXER_H