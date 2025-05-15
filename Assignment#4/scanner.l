%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

extern void yyerror(const char *msg);
int line = 1, column = 1;

void update_position(char *text) {
    for (char *p = text; *p; p++) {
        if (*p == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
}
%}

%option noyywrap
%option noinput
%option nounput

%%

"{"                     { update_position(yytext); return LBRACE; }
"}"                     { update_position(yytext); return RBRACE; }
"["                     { update_position(yytext); return LBRACKET; }
"]"                     { update_position(yytext); return RBRACKET; }
":"                     { update_position(yytext); return COLON; }
","                     { update_position(yytext); return COMMA; }
"true"                  { update_position(yytext); return TRUE; }
"false"                 { update_position(yytext); return FALSE; }
"null"                  { update_position(yytext); return NULLVAL; }

\"(\\.|[^\\"])*\"       {
    update_position(yytext);
    yylval.str = strdup(yytext);
    return STRING;
}

-?[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+)? {
    update_position(yytext);
    yylval.str = strdup(yytext);
    return NUMBER;
}

[ \t\r\n]+              { update_position(yytext); /* ignore whitespace */ }

.                       {
    char msg[50];
    snprintf(msg, 50, "Unexpected character '%s'", yytext);
    yyerror(msg);
    exit(1);
}

%%
