%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"
#include "ast.h"

extern int yylval;
%}

%option noyywrap
%option yylineno

%%

"int"           { return INT; }
"float"         { return FLOAT; }
"char"          { return CHAR; }
"void"          { return VOID; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"for"           { return FOR; }
"return"        { return RETURN; }
"break"         { return BREAK; }
"continue"      { return CONTINUE; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULTIPLY; }
"/"             { return DIVIDE; }
"%"             { return MODULO; }
"="             { return ASSIGN; }
"=="            { return EQUAL; }
"!="            { return NOT_EQUAL; }
"<"             { return LESS; }
"<="            { return LESS_EQUAL; }
">"             { return GREATER; }
">="            { return GREATER_EQUAL; }
"&&"            { return AND; }
"||"            { return OR; }
"!"             { return NOT; }

"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
";"             { return SEMICOLON; }
","             { return COMMA; }

[0-9]+          { yylval = atoi(yytext); return INTEGER; }
[0-9]*\.[0-9]+  { yylval = (int)(atof(yytext) * 1000); return FLOATING; }
'[^']'          { yylval = yytext[1]; return CHARACTER; }
\"[^\"]*\"      { yylval = (int)strdup(yytext); return STRING; }

[a-zA-Z_][a-zA-Z0-9_]* { yylval = (int)strdup(yytext); return IDENTIFIER; }

[ \t\n\r]       { /* ignore */ }

"//".*          { /* ignore single line comments */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* ignore multi-line comments */ }

.               { printf("Unknown character: %s\n", yytext); }

%%
