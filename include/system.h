#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

class System {
 public:

  Processor& Cpu();                   // DONE
  std::vector<Process>& Processes();  
  float MemoryUtilization()const;     // DONE
  long UpTime();                      // DONE
  int TotalProcesses();               // DONE
  int RunningProcesses();             // DONE
  std::string Kernel() const;         // DONE
  std::string OperatingSystem();      // DONE

  // TODO: Define any necessary private members
 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
};

#endif