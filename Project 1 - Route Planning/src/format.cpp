#include "format.h"

#include <string>
#include <iomanip>
#include <sstream>

using std::string;
using std::stringstream;

// Format seconds as HH:MM:SS
string Format::ElapsedTime(long seconds) {
    long hours = seconds / 3600;
    seconds %= 3600;
    long minutes = seconds / 60;
    seconds %= 60;
    
    stringstream ss;
    ss << std::setw(2) << std::setfill('0') << hours << ":";
    ss << std::setw(2) << std::setfill('0') << minutes << ":";
    ss << std::setw(2) << std::setfill('0') << seconds;
    
    return ss.str();
}