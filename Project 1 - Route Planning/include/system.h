#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();                   // Returns the system's CPU
  std::vector<Process>& Processes();  // Returns a container of processes
  float MemoryUtilization() const;    // Returns the system's memory utilization
  long UpTime() const;                // Returns the system's uptime
  int TotalProcesses() const;         // Returns the total number of processes
  int RunningProcesses() const;       // Returns the number of running processes
  std::string Kernel() const;         // Returns the kernel name
  std::string OperatingSystem() const;// Returns the operating system name

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  std::string kernel_;
  std::string operating_system_;
  void UpdateProcesses();             // Updates the container of processes
};

#endif