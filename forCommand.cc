#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Command.hh"
#include "SimpleCommand.hh"
#include "forCommand.hh"
#include "PipeCommand.hh"
#include "Shell.hh"

forCommand::forCommand() {
    _condition = NULL;
    _listCommands =  NULL;
    std::string *word = NULL;
}

void 
forCommand::insertCondition( SimpleCommand * condition ) {
    _condition = condition;
    
}

void 
forCommand::insertListCommands( ListCommands * listCommands) {
    _listCommands = listCommands;
}

void 
forCommand::clear() {
}


  
void 
forCommand::execute() {
     PipeCommand p = PipeCommand();
    p.insertSimpleCommand();
    //p.print();
    std::vector<std::string> initial_condition = p.for_argumemnts();

    for(int i=0;i<initial_condition.size();i++) {
        int error = setenv(word->c_str(), initial_condition[i].c_str(),1);
         _listCommands->execute();
    }
    
}



void
forCommand::print() {

}