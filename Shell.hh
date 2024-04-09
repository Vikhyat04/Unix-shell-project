#ifndef shell_hh
#define shell_hh

#include "ListCommands.hh"
#include "PipeCommand.hh"
#include "IfCommand.hh"
#include "forCommand.hh"

class Shell {

public:
  int _level; // Only outer level executes.
  bool _enablePrompt;
  bool _loop;
  ListCommands * _listCommands; 
  SimpleCommand *_simpleCommand;
  PipeCommand * _pipeCommand;
  IfCommand * _ifCommand;
  Command * _currentCommand;
  static Shell * TheShell;
  std::vector<IfCommand*> _whiles;
  std::vector<ListCommands*> _listcommands_w;
  std::vector<ForCommand*> _fors;

  Shell();
  void execute();
  void print();
  void clear();
  void prompt();

};

#endif
