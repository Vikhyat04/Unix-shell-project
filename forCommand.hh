#ifndef forcommand_hh
#define forcommand_hh

#include "Command.hh"
#include "SimpleCommand.hh"
#include "ListCommands.hh"

// Command Data Structure

class forCommand : public Command {
public:
  SimpleCommand * _condition;
  ListCommands * _listCommands; 
  std::string * word;
  std::vector<std::string> args;

  forCommand();
  void insertCondition( SimpleCommand * condition );
  void insertListCommands( ListCommands * listCommands);
  //static int runTest(SimpleCommand * condition);

  void clear();
  void execute();
  void print();

};

#endif
