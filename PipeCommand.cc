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
#include <dirent.h>
#include <regex.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "PipeCommand.hh"
#include "Shell.hh"
#include "forCommand.hh"

void push_buffer(FILE *yyin);

PipeCommand::PipeCommand() {
    // Initialize a new vector of Simple PipeCommands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
    _amberr = 0;
    _ambin = 0;
    _ambout = 0;
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

std::vector<std::string>PipeCommand::for_argumemnts() {

        std::vector<std::string> args3 = expandEnvVars(0);

        args3 = subshells(args3);

        for (int i = 0; i < args3.size(); i++) {
            std::string &arg = args3[i];

            if (arg.find('*') != std::string::npos || arg.find('?') != std::string::npos) {
                std::vector<std::string> wild = wildcards(arg, ""); 
                if (!wild.empty()) {
                    std::sort(wild.begin(), wild.end());
                    args3.erase(args3.begin() + i);
                    args3.insert(args3.begin() + i, wild.begin(), wild.end()); 
                    i += wild.size();
                }
            }
        }
    return args3;
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
        if (Shell::TheShell->_enablePrompt) {
		    printf("Good bye!!\n");
        }
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

        printf("3\n");
        dup2(fdin, 0);
        close(fdin);

        printf("i: %d\n", i);

        //
        SimpleCommand * s = _simpleCommands[i];

        std::vector<std::string> args3 = expandEnvVars(i);

        args3 = subshells(args3);

        for (int i = 0; i < args3.size(); i++) {
            std::string &arg = args3[i];

            if (arg.find('*') != std::string::npos || arg.find('?') != std::string::npos) {
                std::vector<std::string> wild = wildcards(arg, ""); 
                if (!wild.empty()) {
                    std::sort(wild.begin(), wild.end());
                    args3.erase(args3.begin() + i);
                    args3.insert(args3.begin() + i, wild.begin(), wild.end()); 
                    i += wild.size();
                }
            }
        }

        printf("i: %d\n", i);


        //
        if(strcmp(args3[0].c_str(),"setenv") == 0){
		    int error = setenv(args3[1].c_str(), args3[2].c_str(), 1);
		    if(error) {
			    perror("setenv");
		    }
            if(Shell::TheShell->_loop == false) {
                clear();
            }
		    return;
	    }
        if(strcmp(args3[0].c_str(),"source") == 0){
            FILE* yyin = fopen(args3[1].c_str(), "r");
            push_buffer(yyin);
		    return;
	    }
        if(strcmp(args3[0].c_str(),"unsetenv") == 0){
		    int error = unsetenv(args3[1].c_str());
		    if(error) {
			    perror("unsetenv");
		    }
		    if(Shell::TheShell->_loop == false) {
                clear();
            }
            return;
	    }
        if(strcmp(args3[0].c_str(), "cd") == 0){
		    int index;
            std::string p;
            if (args3.size()==1) {
                index = chdir(getenv("HOME"));
                p = args3[0].c_str();
            } else {
		        index = chdir(args3[1].c_str());
                p = args3[1].c_str();
            }

		    if (index == -1) { 
			    fprintf(stderr, "cd: can't cd to %s\n", p.c_str());
            }

		    if(Shell::TheShell->_loop == false) {
                clear();
            }

            dup2(defin, 0);
            dup2(defout, 1);
            dup2(deferr, 2);
            close(defin);
            close(defout);
            close(deferr);

            return;
	    }

        printf("i: %d\n", i);

        
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
            printf("1\n");
			fdout = fdpipe[1];
			fdin = fdpipe[0];
		}
        printf("2\n");
        dup2(fdout, 1);
        close(fdout);

        printf("i: %d\n", i);
        
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

        printf("i: %d\n", i);
    }
    printf("ooga:\n");
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
    if(Shell::TheShell->_loop == false) {
            clear();
    }

    // Print new prompt
    //Shell::TheShell->prompt();
}

// Expands environment vars and wildcards of a SimpleCommand and
// returns the arguments to pass to execvp.
std::vector<std::string> PipeCommand::expandEnvVars(int simpleCommandNumber) {
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
            if (j == 0 && arg[j] == '~') {
                int k = j + 1;
                while (k < arg.length() && arg[k] != '/') {
                    k++;
                }
                updatedArg += "/homes/" + ((k == j + 1) ? getenv("USER") : arg.substr(j + 1, k - j - 1));
                j = k - 1;
            }
            else if (arg[j] == '$' && j + 1 != arg.length()) {
                if (arg[j + 1] == '{') {
                    std::string var = arg.substr(j + 2, arg.find('}', j) - j - 2);
                    setenv("$", std::to_string(getpid()).c_str(), 1);
                    char* envValue = getenv(var.c_str());
                    if (envValue) {
                        updatedArg += envValue;
                        j += var.length() + 2;
                    }
                } else {
                    updatedArg += arg[j];
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

std::vector<std::string> PipeCommand::wildcards(std::string arg, std::string basePath) {

    std::vector<std::string> finalPaths;

    // Base case
    if (arg.empty()) {
        finalPaths.push_back(basePath);
        return finalPaths;
    }

    if (arg[0] == '/') {
        basePath += '/';
        return wildcards(arg.substr(1, arg.length()), basePath);
    }

    size_t slashPosition = arg.find('/');

    if (slashPosition == std::string::npos) {
        slashPosition = arg.size();
    }

    std::string prevDir = arg.substr(0, slashPosition);

    if (prevDir.find('?') == std::string::npos && prevDir.find('*') == std::string::npos) {
        return wildcards(arg.substr(slashPosition), basePath + prevDir);
    } else {

        std::string regexPattern = regex_func(prevDir); 

        regex_t regexCompiled;
        int res = regcomp(&regexCompiled, regexPattern.c_str(), REG_EXTENDED | REG_NOSUB);
        if (res != 0) {
            perror("regcomp");
            return finalPaths;
        }

        std::string prevDirPath = basePath + '.';
        DIR * directory = opendir(prevDirPath.c_str());
        if (directory == NULL) {
            if (errno != ENOENT && errno != ENOTDIR) {
                perror("opendir");
            }
            regfree(&regexCompiled);
            return finalPaths;
        }

        struct dirent * dirEntry;
        while ((dirEntry = readdir(directory)) != NULL) {
            if (dirEntry->d_name[0] == '.' && prevDir[0] != '.') {
                continue;
            }

            regmatch_t match;
            if (regexec(&regexCompiled, dirEntry->d_name, 1, &match, 0) == 0) {
                std::vector<std::string> subPaths = wildcards(arg.substr(slashPosition), basePath + dirEntry->d_name);
                finalPaths.insert(finalPaths.end(), subPaths.begin(), subPaths.end());
            }
        }
        regfree(&regexCompiled);
        closedir(directory);
        return finalPaths;
    }
}

std::string PipeCommand::regex_func(std::string prev_dir) {
    std::string regex_string = "^";
    for (auto chr : prev_dir) {
        switch (chr) {
            case '*':
                regex_string += ".*";
                break;
            case '?':
                regex_string += ".";
                break;
            case '.':
                regex_string += "\\.";
                break;
            default:
                regex_string += chr;
        }
    }
    regex_string += '$';
    return regex_string;
}

std::vector<std::string> PipeCommand::subshells(std::vector<std::string> args) {
        for (int i = 0; i < args.size(); i++) {
            std::string &arg = args[i];
            int flag=0;
            std::string exp;

            if (arg.size() >= 2 && arg[0] == '`' && arg[arg.size()-1] == '`') {
                exp = arg.substr(1, arg.size()-2);
                flag = 1;
            } else if (arg.size() >= 3 && arg[0] == '$' && arg[1] == '(' && arg[arg.size()-1] == ')') {
                exp = arg.substr(2, arg.size()-3);
                flag = 1;
            }
            if(flag) {
                int tmpin = dup(0);
	    int tmpout = dup(1);

                int fdpipein[2];
                int fdpipeout[2];
                pipe(fdpipein);
                pipe(fdpipeout);

                write(fdpipein[1], exp.c_str(), strlen(exp.c_str()));
                write(fdpipein[1], "\n", 1);
                write(fdpipein[1], "exit", 4);
                write(fdpipein[1], "\n", 1);

                close(fdpipein[1]);

                dup2(fdpipein[0], 0);
                close(fdpipein[0]);
                dup2(fdpipeout[1], 1);
                close(fdpipeout[1]);

                int ret = fork();
                if (ret == 0) {
                    char **aa = (char **) malloc(2 * sizeof(char *));
                    aa[0] = strdup("/proc/self/exe");
                    aa[1] = NULL;
                    execvp(aa[0], aa);
                    _exit(1);
                } else if (ret < 0) {
                    perror("fork");
                    exit(1);
                    }
                dup2(tmpin, 0);
                dup2(tmpout, 1);
                close(tmpin);
                close(tmpout);

                char ch;
                char * buffer = (char *) malloc (4096);
                int k = 0;

                args.erase(args.begin() + i);

                while (read(fdpipeout[0], &ch, 1)) {
                    if (ch == '\n'  || ch == ' ' || ch == '\t'){
                        buffer[k++] = '\0';
                        args.insert(args.begin() + i, std::string(buffer));
                        i++;
                        k = 0;
                    }
                        else buffer[k++] = ch;
                    }
                close(fdpipeout[0]);
                free(buffer);
                } else {

                }
                flag=0;
            }
        // for(int i=0;i<args.size();i++) {
        //     std::cout << args[i] << std::endl;
        // }
        return args;
}




