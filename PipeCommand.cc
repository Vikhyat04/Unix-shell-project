/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT 
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE 
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM  
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "PipeCommand.hh"
#include "Shell.hh"


PipeCommand::PipeCommand() {
    // Initialize a new vector of Simple PipeCommands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
}

void PipeCommand::insertSimpleCommand( SimpleCommand * simplePipeCommand ) {
    // add the simple command to the vector
    _simpleCommands.push_back(simplePipeCommand);
}

void PipeCommand::clear() {
    // deallocate all the simple commands in the command vector
    for (auto simplePipeCommand : _simpleCommands) {
        delete simplePipeCommand;
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    _simpleCommands.clear();

      if ( _outFile ) {
        delete _outFile;
        if ( _outFile == _errFile ) {
            _errFile = NULL;
        }
        _outFile = NULL;
    }


    if ( _inFile ) {
        delete _inFile;
            _inFile = NULL;
    }

    if ( _errFile ) {
        delete _errFile;
                _errFile = NULL;
    }

    _background = false;
}

void PipeCommand::print() {
    printf("\n\n");
    //printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple PipeCommands\n");
    printf("  --- ----------------------------------------------------------\n");

    int i = 0;
    // iterate over the simple commands and print them nicely
    for ( auto & simplePipeCommand : _simpleCommands ) {
        printf("  %-3d ", i++ );
        simplePipeCommand->print();
    }

    printf( "\n\n" );
    printf( "  Output       Input        Error        Background\n" );
    printf( "  ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s\n",
            _outFile?_outFile->c_str():"default",
            _inFile?_inFile->c_str():"default",
            _errFile?_errFile->c_str():"default",
            _background?"YES":"NO");
    printf( "\n\n" );
}

void PipeCommand::execute() {
    // Don't do anything if there are no simple commands
    if ( _simpleCommands.size() == 0 ) {
        Shell::TheShell->prompt();
        return;
    }

    // Print contents of PipeCommand data structure
    //print();

    if(strcmp(_simpleCommands[0]->_arguments[0]->c_str(),"exit") == 0){
		printf("Good bye!!\n");
		exit(1);
	}

    // Add execution here
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec
        if (_ambin > 1 || _ambout > 1 || _amberr > 1) {
		  printf("Ambiguous output redirect.\n");
      clear();
		  return;
	}
        int defin = dup( 0 );
	int defout = dup( 1 );
	int deferr = dup( 2 );
     int fdin;
    int	fdout;
	int fderr;
    if (_inFile) {
        fdin = open(_inFile->c_str(), O_RDONLY);
    } else {
        fdin = dup(defin);
    }

    if(_errFile){
		if(_boolappend){
			fderr = open(_errFile->c_str(), O_WRONLY | O_APPEND | O_CREAT, 0600);
		}
		else {
			fderr = open(_errFile->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
		}
	}
	else {
		fderr = dup(deferr);
	}
    dup2(fderr,2);
	close(fderr);
    int ret;
     unsigned long num_of_commands = _simpleCommands.size();
     for (unsigned long i = 0; i < num_of_commands; i++) {
        //
        if(strcmp(_simpleCommands[0]->_arguments[0]->c_str(),"setenv") == 0){
		    int error = setenv(_simpleCommands[i]->_arguments[1]->c_str(), _simpleCommands[i]->_arguments[2]->c_str(), 1);
		    if(error) {
			    perror("setenv");
		    }
		    clear();
            Shell::TheShell->prompt();
		    return;
	    }
        if(strcmp(_simpleCommands[0]->_arguments[0]->c_str(),"source") == 0){
            //int fn = fopen();
		    clear();
            Shell::TheShell->prompt();
		    return;
	    }
        if(strcmp(_simpleCommands[0]->_arguments[0]->c_str(),"unsetenv") == 0){
		    int error = unsetenv(_simpleCommands[i]->_arguments[1]->c_str());
		    if(error) {
			    perror("unsetenv");
		    }
		    clear();
            Shell::TheShell->prompt();
		    return;
	    }
        if(strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "cd") == 0){
		    int index;
            std::string p;
		    if(_simpleCommands[i]->_arguments.size() == 1){
			    index = chdir(getenv("HOME"));
                p = getenv("HOME");
		    } else {
			    index = chdir(_simpleCommands[i]->_arguments[1]->c_str());
                p = _simpleCommands[i]->_arguments[1]->c_str();
            }

		    if (index == -1) { 
			    fprintf(stderr, "cd: can't cd to %s\n", p.c_str());
            }

		    clear();
		    Shell::TheShell->prompt();
		    return;
	    }

        SimpleCommand * s = _simpleCommands[i];

        std::vector<std::string> args3 = expandEnvVarsAndWildcards(i);


        //
        dup2(fdin, 0);
        close(fdin);
        if(i == num_of_commands - 1) {
            if(_outFile) {
                if(_boolappend) {
                    fdout = open(_outFile->c_str(), O_WRONLY | O_CREAT | O_APPEND, 0600);
                }
                else {
                    fdout = open(_outFile->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
                }
            } else {
                fdout = dup(defout);
            }
        } else {
			int fdpipe[2];
            if (pipe(fdpipe) == -1) {
                perror("pipe");
                exit(1);
            }
			pipe(fdpipe);
			fdout = fdpipe[1];
			fdin = fdpipe[0];
		}
        dup2(fdout, 1);
        close(fdout);
        
        const char ** args = (const char **)
        malloc((args3.size()+1)*sizeof(char*));
        for ( unsigned long j=0;j < args3.size(); j++) {
            args[j] = args3[j].c_str();
        }
        args[args3.size()] = NULL;
        ret = fork();
        if (ret == 0) {
            if(strcmp(args[0], "printenv") == 0){
				char ** envi = environ;

				while(*envi){
					printf("%s\n", *envi);
					envi++;
				}
                fflush(stdout);
			} else {
                execvp(args[0], (char* const*)args);
                perror("execvp");
                exit(1);
            }
        }
    }
    dup2(defin, 0);
    dup2(defout, 1);
    dup2(deferr, 2);
    close(defin);
	close(defout);
	close(deferr);
    if (!_background) {
        int status;
        waitpid(ret, &status, 0);
        setenv("?", std::to_string(WEXITSTATUS(status)).c_str(), 1);
    } else {
        setenv("!",std::to_string(ret).c_str(),1);
    }
    


    // Clear to prepare for next command
    clear();

    // Print new prompt
    //Shell::TheShell->prompt();
}

// Expands environment vars and wildcards of a SimpleCommand and
// returns the arguments to pass to execvp.
std::vector<std::string> PipeCommand::expandEnvVarsAndWildcards(int simpleCommandNumber) {
    int ind = simpleCommandNumber; 

    SimpleCommand* s = _simpleCommands[ind];
    
    std::vector<std::string> args(s->_arguments.size());

    for(int i = 0; i < s->_arguments.size(); i++) {
        args[i] = *s->_arguments[i];
    }

    for (int i = 0; i < args.size(); i++) {
        std::string &arg = args[i];
        std::string updatedArg = "";
        
        for (int j = 0; j < arg.length(); j++) {
            if (arg[j] == '$' && j + 1 != arg.length()) {
                if (arg[j + 1] == '{') {
                    std::string var = arg.substr(j + 2, arg.find('}', j) - j - 2);
                    setenv("$", std::to_string(getpid()).c_str(), 1);
                    char* envValue = getenv(var.c_str());
                    if (envValue) {
                        updatedArg += envValue;
                        j += var.length() + 2;
                    }
                }
            } else {
                updatedArg += arg[j];
            }
        }
        args[i] = updatedArg;
    }

    setenv("_", args[args.size() - 1].c_str(), 1);
    return args;
}



