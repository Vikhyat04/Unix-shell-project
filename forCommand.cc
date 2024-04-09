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

forCommand::forCommand(bool _isfor) {
    _condition = NULL;
    _listCommands =  NULL;
    this->_isfor = _isfor;
    std::string iterator = NULL;
}


