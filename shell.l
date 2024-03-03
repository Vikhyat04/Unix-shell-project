
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


[^ \t\n;][^ \t\n;]*  {
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  return WORD;
}

["][^\n\"]*["] { 
	yylval.string_val = strdup(yytext+1);

    if (yylval.string_val[yyleng-2] == '"')
		yylval.string_val[yyleng-2] = 0;
	return WORD;
} 