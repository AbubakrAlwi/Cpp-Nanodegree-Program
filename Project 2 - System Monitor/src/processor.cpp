#include "processor.h"
#include "linux_parser.h"
#include <string>
#include <vector>

// Constructor
Processor::Processor() {
    // Initialize with first reading to prepare for first calculation
    std::vector<std::string> cpu_utilization = LinuxParser::CpuUtilization();
    
    if (cpu_utilization.size() >= 10) {
        // Get initial jiffies values
        long user = std::stol(cpu_utilization[0]);
        long nice = std::stol(cpu_utilization[1]);
        long system = std::stol(cpu_utilization[2]);
        long idle = std::stol(cpu_utilization[3]);
        long iowait = std::stol(cpu_utilization[4]);
        long irq = std::stol(cpu_utilization[5]);
        long softirq = std::stol(cpu_utilization[6]);
        long steal = std::stol(cpu_utilization[7]);
        
        // Calculate idle and non-idle time
        prev_idle_ = idle + iowait;
        prev_non_idle_ = user + nice + system + irq + softirq + steal;
        prev_total_ = prev_idle_ + prev_non_idle_;
    }
}

// Calculate the current CPU utilization
float Processor::Utilization() {
    std::vector<std::string> cpu_utilization = LinuxParser::CpuUtilization();
    
    if (cpu_utilization.size() < 10) {
        return 0.0;  // Return 0 if we can't get valid data
    }
    
    // Get current jiffies values from /proc/stat
    long user = std::stol(cpu_utilization[0]);
    long nice = std::stol(cpu_utilization[1]);
    long system = std::stol(cpu_utilization[2]);
    long idle = std::stol(cpu_utilization[3]);
    long iowait = std::stol(cpu_utilization[4]);
    long irq = std::stol(cpu_utilization[5]);
    long softirq = std::stol(cpu_utilization[6]);
    long steal = std::stol(cpu_utilization[7]);
    
    // Calculate current idle and non-idle time
    long current_idle = idle + iowait;
    long current_non_idle = user + nice + system + irq + softirq + steal;
    long current_total = current_idle + current_non_idle;
    
    // Calculate the delta values compared to previous measurement
    long total_delta = current_total - prev_total_;
    long idle_delta = current_idle - prev_idle_;
    
    // Update previous values for next calculation
    prev_idle_ = current_idle;
    prev_non_idle_ = current_non_idle;
    prev_total_ = current_total;
    
    // Calculate CPU utilization (avoid division by zero)
    if (total_delta == 0) {
        return 0.0;
    }
    
    // CPU utilization = (total_delta - idle_delta) / total_delta
    return static_cast<float>(total_delta - idle_delta) / total_delta;
}