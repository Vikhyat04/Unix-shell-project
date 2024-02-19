#ifndef command_hh
#define command_hh

// Command Data Structure

enum {
  PipeCommandType,
  IfCommandType,
  DoCommandType,
  ForCommandTYpe
};

// Command classes inherit form Command
class Command {

public:
  int commandType;
  
  virtual void clear()=0;
  virtual void print()=0;
  virtual void execute()=0;
};

#endif
