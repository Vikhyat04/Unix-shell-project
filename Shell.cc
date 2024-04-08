
#include <unistd.h>
#include <cstdio>
#include <sys/wait.h>

#include "Command.hh"
#include "Shell.hh"
#include "signal.h"

int yyparse(void);


Shell * Shell::TheShell;

Shell::Shell() {
    this->_level = 0;
    this->_loop = false;
    this->_enablePrompt = true;
    this->_listCommands = new ListCommands(); 
    this->_simpleCommand = new SimpleCommand();
    this->_pipeCommand = new PipeCommand();
    this->_currentCommand = this->_pipeCommand;
    if ( !isatty(0)) {
	this->_enablePrompt = false;
    }
}

extern "C" void ctrlC(int sig) {
    printf("\nctrl C works\n");
    Shell::TheShell->prompt();
}


void Shell::prompt() {
    if (_enablePrompt) {
	printf("myshell>");
	fflush(stdout);
    }
}

extern "C" void zombie(int sig) {
	  int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if(Shell::TheShell->_enablePrompt) {
          printf("%d exited\n", pid);
        }
    }

}

void Shell::print() {
    printf("\n--------------- Command Table ---------------\n");
    this->_listCommands->print();
}

void Shell::clear() {
    this->_listCommands->clear();
    this->_simpleCommand->clear();
    this->_pipeCommand->clear();
    this->_currentCommand->clear();
    this->_level = 0;
}

void Shell::execute() {
  if (this->_level == 0 ) {
    //this->print();
    this->_listCommands->execute();
    this->_listCommands->clear();
    this->prompt();
  }
}

void yyset_in (FILE *  in_str );

int 
main(int argc, char **argv) {

    struct sigaction signalAction;
    signalAction.sa_handler = ctrlC;  // Use ctrlC as the handler
    sigemptyset(&signalAction.sa_mask);
    signalAction.sa_flags = SA_RESTART;
    int error = sigaction(SIGINT, &signalAction, NULL);
    if (error) {
        perror("sigaction");
        exit(-1);
    }

    struct sigaction signalAction2;
	  signalAction2.sa_handler = zombie;
	  sigemptyset(&signalAction2.sa_mask);
	  signalAction2.sa_flags = SA_RESTART;
	  error =  sigaction(SIGCHLD, &signalAction2, NULL);
	  if (error) {
		  perror("sigaction");
		  exit(-1);
	  }

  char * input_file = NULL;
  if ( argc > 1 ) {
    input_file = argv[1];
    FILE * f = fopen(input_file, "r");
    if (f==NULL) {
	fprintf(stderr, "Cannot open file %s\n", input_file);
        perror("fopen");
        exit(1);
    }
    yyset_in(f);
  }
  const char* relativePath = argv[0];
  char resolvedPath[256];

  char* result = realpath(relativePath, resolvedPath);

  setenv("SHELL",result,1);

  Shell::TheShell = new Shell();

  if (input_file != NULL) {
    // No prompt if running a script
    Shell::TheShell->_enablePrompt = false;
  }
  else {
    Shell::TheShell->prompt();
  }
  yyparse();
}


