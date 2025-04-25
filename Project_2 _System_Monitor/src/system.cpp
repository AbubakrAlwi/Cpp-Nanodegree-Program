#include "system.h"

#include <unistd.h>
#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container of processes
vector<Process>& System::Processes() {
    UpdateProcesses();
    return processes_;
}

// Update the container of processes
void System::UpdateProcesses() {
    // Get all process IDs
    vector<int> pids = LinuxParser::Pids();
    
    // Create a set of existing process IDs for fast lookup
    set<int> current_pids;
    for (Process& process : processes_) {
        current_pids.insert(process.Pid());
    }
    
    // Add new processes
    for (int pid : pids) {
        // Check if this process is already in our list
        if (current_pids.find(pid) == current_pids.end()) {
            processes_.emplace_back(Process(pid));
        }
    }
    
    // Remove terminated processes (not in pids anymore)
    processes_.erase(
        std::remove_if(processes_.begin(), processes_.end(),
                     [&pids](const Process& process) {
                         return std::find(pids.begin(), pids.end(), process.Pid()) == pids.end();
                     }),
        processes_.end());
    
    // Update CPU utilization for all processes
    for (auto& process : processes_) {
        process.UpdateCpuUtilization();
    }
    
    // Sort processes by CPU utilization (highest first)
    std::sort(processes_.begin(), processes_.end(), std::greater<Process>());
}

// Return the system's memory utilization
float System::MemoryUtilization() const { 
    return LinuxParser::MemoryUtilization(); 
}

// Return the system's uptime
long System::UpTime() const { 
    return LinuxParser::UpTime(); 
}

// Return the total number of processes
int System::TotalProcesses() const { 
    return LinuxParser::TotalProcesses(); 
}

// Return the number of running processes
int System::RunningProcesses() const { 
    return LinuxParser::RunningProcesses(); 
}

// Return the kernel name
string System::Kernel() const {
    if (kernel_.empty()) {
        // Cache kernel name
        const_cast<System*>(this)->kernel_ = LinuxParser::Kernel();
    }
    return kernel_;
}

// Return the operating system name
string System::OperatingSystem() const {
    if (operating_system_.empty()) {
        // Cache operating system name
        const_cast<System*>(this)->operating_system_ = LinuxParser::OperatingSystem();
    }
    return operating_system_;
}