/*
 * CS-252
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */
%code requires 
{
#include <string>
#if __cplusplus > 199711L
#define register      // Deprecated in C++11 so remove the keyword
#endif
}
%union
{
  char        *string_val;
  // Example of using a c++ type in yacc
  std::string *cpp_string;
}
%token <cpp_string> WORD
%token NOTOKEN GREAT GREATGREAT GREATAMPERSAND GREATGREATAMPERSAND 
%token AMPERSAND PIPE LESS NEWLINE IF FI THEN LBRACKET RBRACKET SEMI
%token DO DONE WHILE FOR IN TWOGREAT
%{
//#define yylex yylex
#include <cstdio>
#include "Shell.hh"
void yyerror(const char * s);
int yylex();
%}
%%
goal: command_list;
arg_list:
        arg_list WORD { 
          Shell::TheShell->_simpleCommand->insertArgument( $2 ); }
        | /*empty string*/
	;
cmd_and_args:
  	WORD { 
          Shell::TheShell->_simpleCommand = new SimpleCommand(); 
          Shell::TheShell->_simpleCommand->insertArgument( $1 );
        } 
        arg_list
	;
pipe_list:
        cmd_and_args 
	    { 
		Shell::TheShell->_pipeCommand->insertSimpleCommand( 
		    Shell::TheShell->_simpleCommand ); 
		Shell::TheShell->_simpleCommand = new SimpleCommand();
	    }
	| pipe_list PIPE cmd_and_args 
	    { 
		Shell::TheShell->_pipeCommand->insertSimpleCommand( 
		    Shell::TheShell->_simpleCommand ); 
		Shell::TheShell->_simpleCommand = new SimpleCommand();
	    }
	;
io_modifier:
	   GREATGREAT WORD {
			Shell::TheShell->_pipeCommand->_outFile = $2;
			Shell::TheShell->_pipeCommand->_boolappend = true;
      Shell::TheShell->_pipeCommand->_ambout++;
	   }
	 | GREAT WORD 
	    {
		Shell::TheShell->_pipeCommand->_outFile = $2;
    Shell::TheShell->_pipeCommand->_ambout++;
	    }
	 | GREATGREATAMPERSAND WORD {
			Shell::TheShell->_pipeCommand->_outFile = $2;
			Shell::TheShell->_pipeCommand->_errFile = $2;
			Shell::TheShell->_pipeCommand->_boolappend = true;
      Shell::TheShell->_pipeCommand->_background = true;
      Shell::TheShell->_pipeCommand->_ambout++;
	 }
	 | GREATAMPERSAND WORD {
			Shell::TheShell->_pipeCommand->_outFile = $2;
			Shell::TheShell->_pipeCommand->_errFile = $2;
      Shell::TheShell->_pipeCommand->_ambout++;
      Shell::TheShell->_pipeCommand->_background = true;
	 }
	 | LESS WORD {
		Shell::TheShell->_pipeCommand->_inFile = $2;
    Shell::TheShell->_pipeCommand->_ambin++;
	 }
	 | TWOGREAT WORD {
		Shell::TheShell->_pipeCommand->_errFile = $2;
	 }
	;
io_modifier_list:
	io_modifier_list io_modifier
	| /*empty*/
	;
background_optional: 
	AMPERSAND
	{
		Shell::TheShell->_pipeCommand->_background = true;
	}
	|
	;
SEPARATOR:
	NEWLINE
	| SEMI
	;
command_line:
	 pipe_list io_modifier_list background_optional SEPARATOR 
         { 
	    Shell::TheShell->_listCommands->
		insertCommand(Shell::TheShell->_pipeCommand);
	    Shell::TheShell->_pipeCommand = new PipeCommand(); 
         }
        | if_command SEPARATOR 
         {
	    Shell::TheShell->_listCommands->
		insertCommand(Shell::TheShell->_ifCommand);
         }
        | while_command SEPARATOR {printf("while\n"); }
        | for_command SEPARATOR {printf("for\n"); }
        | SEPARATOR /*accept empty cmd line*/
        | error SEPARATOR {yyerrok; Shell::TheShell->clear(); }
	;          /*error recovery*/
command_list :
     command_line 
	{ 
	   Shell::TheShell->execute();
	}
     | 
     command_list command_line 
	{
	    Shell::TheShell->execute();
	}
     ;  /* command loop*/
if_command:
    IF LBRACKET 
	{ 
	    Shell::TheShell->_level++; 
	    Shell::TheShell->_ifCommand = new IfCommand();
	} 
    arg_list RBRACKET SEMI THEN 
	{
	    Shell::TheShell->_ifCommand->insertCondition( 
		    Shell::TheShell->_simpleCommand);
	    Shell::TheShell->_simpleCommand = new SimpleCommand();
	}
    command_list FI 
	{ 
	    Shell::TheShell->_level--; 
	    Shell::TheShell->_ifCommand->insertListCommands( 
		    Shell::TheShell->_listCommands);
	    Shell::TheShell->_listCommands = new ListCommands();
	}
    ;
while_command:
    WHILE LBRACKET arg_list RBRACKET SEMI DO command_list DONE 
    ;
for_command:
    FOR WORD IN arg_list SEMI DO command_list DONE
    ;
%%
void
yyerror(const char * s)
{
  fprintf(stderr,"%s", s);
}
#if 0
main()
{
  yyparse();
}
#endif
