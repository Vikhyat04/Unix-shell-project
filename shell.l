
/*
 *
 * CS-252 
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <cstring>
#include "y.tab.hh"

static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

%}

%option noyywrap

%%

"#".*\n {
  /* Discard comments starting with # */
}

\n {
  return NEWLINE;
}

[ \t] {
  /* Discard spaces and tabs */
}

">" {
  return GREAT;
}

"<" {
  return LESS;
}

"|" {
  return PIPE;
}

">>" {
  return GREATGREAT;
}

">&" {
  return GREATAMPERSAND;
}

">>&" {
  return GREATGREATAMPERSAND;
}

"&" {
  return AMPERSAND;
}

"[" {
  return LBRACKET;
}

"]" {
  return RBRACKET;
}

"if" {
  return IF;
}

"then" {
  return THEN;
}

"fi" {
  return FI;
}

"while" {
  return WHILE;
}

"do" {
  return DO;
}

"done" {
  return DONE;
}

"for" {
  return FOR;
}

"in" {
  return IN;
}

";" {
  return SEMI;
}

"2>" {
  return TWOGREAT;
}

\"(\\.|[^\"\\])*\" {
    char *escapedString = (char *)malloc(yyleng);
    int escapedStringLength = 0;

    for (int i = 1; i < yyleng - 1; ++i) {
        if (yytext[i] == '\\' && i + 1 < yyleng - 1) {
            escapedString[escapedStringLength++] = yytext[++i];
        } else {
            escapedString[escapedStringLength++] = yytext[i];
        }
    }

    escapedString[escapedStringLength] = '\0';
    yylval.cpp_string = new std::string(escapedString);
    free(escapedString);
    return WORD;
}

\"[^\n\"]*\" {
    yylval.cpp_string = new std::string(yytext+1, yyleng-2);
    return WORD;
}

[^ \t\n;][^ \t\n;]*  {
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  return WORD;
}

%%