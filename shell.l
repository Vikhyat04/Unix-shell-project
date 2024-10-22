
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
YY_BUFFER_STATE yy_create_buffer ( FILE *file, int size );
void yypush_buffer_state ( YY_BUFFER_STATE buffer );
void yypop_buffer_state ( );

void myunputc(int c) {
  unput(c);
}

void push_buffer (FILE *yyin) {
  yypush_buffer_state(
      yy_create_buffer( yyin, YY_BUF_SIZE ) );
  
}

%}

%option noyywrap

%%

<<EOF>> {
			yypop_buffer_state();

            if ( !YY_CURRENT_BUFFER )
                {
                yyterminate();
                }
            }

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

\$\(([^\)]*)\) {
  yylval.cpp_string = new std::string(yytext);
  return WORD;
}

[^ \t\n|><&]*\\[^ \t\n]* {

  int i=0;
  char *esc;
  esc = new char[strlen(yytext)*2];
  char *args = yytext;
  while(*args) {
    if(*args == '\\') {
      esc[i] = *(++args);
      i++;
    } else {
      esc[i] = *args;
      i++;
    }
    args++;
  }
  esc[i] = '\0';
  yylval.cpp_string = new std::string(esc);
  return WORD;
}

\"[^\n\"]*\" {
    yylval.cpp_string = new std::string(yytext+1, yyleng-2);
    return WORD;
}


[^ \t\n;|<>&][^ \t\n;|<>&]*  {
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  return WORD;
}

`[^\n`]*` {
  yylval.cpp_string = new std::string(yytext);
  return WORD;
}



%%