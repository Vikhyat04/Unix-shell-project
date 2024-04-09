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
    std::string iterator = NULL;
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
    

    

}



void
forCommand::print() {

}