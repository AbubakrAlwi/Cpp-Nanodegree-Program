#ifndef PROCESS_H
#define PROCESS_H

#include <string>

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid() const;                         // Return this process's ID
  std::string User() const;                // Return the username that generated this process
  std::string Command() const;             // Return the command that generated this process
  float CpuUtilization() const;            // Return this process's CPU utilization
  void UpdateCpuUtilization();             // Update CPU utilization value
  std::string Ram() const;                 // Return this process's memory utilization in MB
  long int UpTime() const;                 // Return the age of this process in seconds
  bool operator<(Process const& a) const;  // Overload the 'less than' comparison operator for Process objects
  bool operator>(Process const& a) const;  // Overload the 'greater than' comparison operator for Process objects

 private:
  int pid_;
  std::string user_ = "";
  std::string command_ = "";
  float cpu_utilization_ = 0.0;
  std::string ram_ = "";
  long int up_time_ = 0;
  long cache_active_jiffies_ = 0;
  long cache_system_jiffies_ = 0;
  long cache_total_time_ = 0;
  long cache_seconds_ = 0;
};

#endif