#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor
Process::Process(int pid) : pid_(pid) {
    this->command_ = LinuxParser::Command(pid_);
    this->user_ = LinuxParser::User(pid_);
    UpdateCpuUtilization();
}

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return the username that generated this process
string Process::User() const { return user_; }

// Return the command that generated this process
string Process::Command() const { return command_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const { return cpu_utilization_; }

// Update this process's CPU utilization
void Process::UpdateCpuUtilization() {
    // Get process active jiffies
    long active_jiffies = LinuxParser::ActiveJiffies(pid_);
    // Get system jiffies
    long system_jiffies = LinuxParser::Jiffies();
    
    // Get process uptime
    long seconds = LinuxParser::UpTime() - LinuxParser::UpTime(pid_);
    
    // Calculate CPU utilization
    long total_time = active_jiffies;
    
    // First time calculation
    if (cache_seconds_ == 0) {
        cache_active_jiffies_ = active_jiffies;
        cache_system_jiffies_ = system_jiffies;
        cache_total_time_ = total_time;
        cache_seconds_ = seconds;
        cpu_utilization_ = 0.0;
        return;
    }
    
    // Calculate delta values
    long delta_total_time = total_time - cache_total_time_;
    long delta_seconds = seconds - cache_seconds_;
    
    // Calculate CPU utilization (avoid division by zero)
    if (delta_seconds > 0) {
        cpu_utilization_ = (static_cast<float>(delta_total_time) / sysconf(_SC_CLK_TCK)) / delta_seconds;
    } else {
        cpu_utilization_ = 0.0;
    }
    
    // Update cache values
    cache_active_jiffies_ = active_jiffies;
    cache_system_jiffies_ = system_jiffies;
    cache_total_time_ = total_time;
    cache_seconds_ = seconds;
}

// Return this process's memory utilization in MB
string Process::Ram() const { 
    try {
        // Get RAM in KB
        string ram_kb = LinuxParser::Ram(pid_);
        if (ram_kb.empty()) return "0";
        
        // Convert from KB to MB
        long ram_kb_long = std::stol(ram_kb);
        long ram_mb_long = ram_kb_long / 1024;
        
        return to_string(ram_mb_long);
    } catch (...) {
        return "0";
    }
}

// Return the age of this process in seconds
long int Process::UpTime() const { 
    try {
        return LinuxParser::UpTime() - LinuxParser::UpTime(pid_);
    } catch (...) {
        return 0;
    }
}

// Overload the 'less than' comparison operator for Process objects
// This is used for sorting processes by CPU utilization
bool Process::operator<(Process const& a) const { 
    return this->cpu_utilization_ < a.cpu_utilization_; 
}

// Overload the 'greater than' comparison operator for Process objects
bool Process::operator>(Process const& a) const { 
    return this->cpu_utilization_ > a.cpu_utilization_; 
}