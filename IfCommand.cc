
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Command.hh"
#include "SimpleCommand.hh"
#include "IfCommand.hh"
#include "PipeCommand.hh"

IfCommand::IfCommand(bool _isif) {
    _condition = NULL;
    _listCommands =  NULL;
    this->_isif = _isif;
}


// Run condition with command "test" and return the exit value.
int
IfCommand::runTest(SimpleCommand * condition) {
    //condition->print();
    PipeCommand p = PipeCommand();
    condition->_arguments.insert(condition->_arguments.begin(), new std::string("test"));
    p.insertSimpleCommand(condition);
    p.execute();
    char *index = (getenv("?"));
    if(index==NULL) {

    } else if(index[0]=='0') {
        return 0;
    } 
    return 1;
}

void 
IfCommand::insertCondition( SimpleCommand * condition ) {
    _condition = condition;
}

void 
IfCommand::insertListCommands( ListCommands * listCommands) {
    _listCommands = listCommands;
}

void 
IfCommand::clear() {
}

void 
IfCommand::print() {
    printf("IF [ \n"); 
    this->_condition->print();
    printf("   ]; then\n");
    this->_listCommands->print();
}
  
void 
IfCommand::execute() {
    // Run command if test is 0
    _condition->_arguments.insert(_condition->_arguments.begin(), new std::string("test"));
    if(_isif) {
        if (runTest(this->_condition) == 0) {
	    _listCommands->execute();
        }
    } else {
        while (runTest(this->_condition) == 0) {
            _listCommands->execute();
        }
    }

}

