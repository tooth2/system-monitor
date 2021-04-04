#include <unistd.h>
#include "string"
#include "vector"

#include "processor.h"
#include "linux_parser.h"

using std::string;
using std::vector;
//Return the aggregate CPU utilization
float Processor::Utilization() { 
  
  long prev_active = LinuxParser::ActiveJiffies();
  long prev_idle = LinuxParser::IdleJiffies();
  sleep(1);
  long current_active = LinuxParser::ActiveJiffies();
  long current_idle = LinuxParser::IdleJiffies();
    
  return (1.0 * (current_active - prev_active)) / (current_active - prev_active + current_idle - prev_idle);
  
}