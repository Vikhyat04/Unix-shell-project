#ifndef pipecommand_hh
#define pipecommand_hh

#include "Command.hh"
#include "SimpleCommand.hh"

// Command Data Structure

class PipeCommand : public Command {
public:
  std::vector<SimpleCommand *> _simpleCommands;
  std::string * _outFile;
  std::string * _inFile;
  std::string * _errFile;
  bool _background;
  bool _boolappend;
  int _ambin;
  int _ambout;
  int _amberr;

  PipeCommand();
  void insertSimpleCommand( SimpleCommand * simpleCommand );

  void clear();
  void print();
  void execute();

  // Expands environment vars and wildcards of a SimpleCommand and
  // returns the arguments to pass to execvp.
  std::vector<std::string> expandEnvVars(int simpleCommandNumber);
  std::vector<std::string> getMatchingFiles(const std::string& wildcard)
  std::vector<std::string> subshells(std::vector<std::string> args);

};

#endif
