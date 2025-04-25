#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <iostream>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  float memTotal = 0;
  float memFree = 0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memTotal = stof(value);
        } else if (key == "MemFree:") {
          memFree = stof(value);
          break;
        }
      }
    }
  }
  // Calculate memory utilization
  return (memTotal - memFree) / memTotal;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string uptime_str;
  long uptime = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime_str;
    try {
      uptime = std::stol(uptime_str);
    } catch (const std::invalid_argument& arg) {
      uptime = 0;
    }
  }
  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> cpu_stats = CpuUtilization();
  long total_jiffies = 0;
  
  for (int i = kUser_; i <= kSteal_; i++) {
    try {
      total_jiffies += stol(cpu_stats[i]);
    } catch (const std::invalid_argument &) {
      // Skip invalid values
    }
  }
  
  return total_jiffies;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string value;
  vector<string> stat_values;
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    
    while (linestream >> value) {
      stat_values.push_back(value);
    }
    
    // Check if we have enough values
    if (stat_values.size() >= 17) {
      // Extract the relevant fields for active jiffies calculation
      // Fields 14, 15, 16, 17 in stat file (but 13, 14, 15, 16 in zero-indexed vector)
      // utime, stime, cutime, cstime
      try {
        long utime = stol(stat_values[13]);
        long stime = stol(stat_values[14]);
        long cutime = stol(stat_values[15]);
        long cstime = stol(stat_values[16]);
        
        return utime + stime + cutime + cstime;
      } catch (const std::invalid_argument &) {
        return 0;
      }
    }
  }
  
  return 0;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_stats = CpuUtilization();
  long active_jiffies = 0;
  
  // Sum up all non-idle CPU time
  try {
    active_jiffies = stol(cpu_stats[kUser_]) + 
                     stol(cpu_stats[kNice_]) + 
                     stol(cpu_stats[kSystem_]) + 
                     stol(cpu_stats[kIRQ_]) + 
                     stol(cpu_stats[kSoftIRQ_]) + 
                     stol(cpu_stats[kSteal_]);
  } catch (const std::invalid_argument &) {
    active_jiffies = 0;
  }
  
  return active_jiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_stats = CpuUtilization();
  long idle_jiffies = 0;
  
  // Sum up idle and iowait time
  try {
    idle_jiffies = stol(cpu_stats[kIdle_]) + stol(cpu_stats[kIOwait_]);
  } catch (const std::invalid_argument &) {
    idle_jiffies = 0;
  }
  
  return idle_jiffies;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string cpu_label;
  string value;
  vector<string> cpu_stats;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);  // Get the first line (overall CPU)
    std::istringstream linestream(line);
    
    linestream >> cpu_label;  // Skip the "cpu" label
    
    // Get all CPU stats
    while (linestream >> value) {
      cpu_stats.push_back(value);
    }
  }
  
  return cpu_stats;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value = 0;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      
      if (key == "processes") {
        linestream >> value;
        break;
      }
    }
  }
  
  return value;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value = 0;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      
      if (key == "procs_running") {
        linestream >> value;
        break;
      }
    }
  }
  
  return value;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string command = "";
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, command);
    
    // Truncate if it's too long (for display purposes)
    if (command.length() > 50) {
      command = command.substr(0, 47) + "...";
    }
  }
  
  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value = "";
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      
      // Using VmRSS for physical memory instead of VmSize (virtual memory)
      if (key == "VmRSS:") {
        linestream >> value;
        break;
      }
    }
  }
  
  return value;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value = "";
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      
      if (key == "Uid:") {
        linestream >> value;  // Get the first UID (real)
        break;
      }
    }
  }
  
  return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line;
  string username, x, user_id;
  
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      
      linestream >> username >> x >> user_id;
      
      if (user_id == uid) {
        return username;
      }
    }
  }
  
  return "";
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  vector<string> values;
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    
    while (linestream >> value) {
      values.push_back(value);
    }
    
    // The 22nd field (21 in zero-indexed) is the starttime in clock ticks since boot
    if (values.size() > 21) {
      try {
        long starttime = stol(values[21]) / sysconf(_SC_CLK_TCK);
        return starttime;
      } catch (const std::invalid_argument &) {
        return 0;
      }
    }
  }
  
  return 0;
}