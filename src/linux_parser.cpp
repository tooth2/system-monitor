#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::cout;

// read data from the filesystem
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

// read data from the filesystem
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

// Update to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // check directory?
    if (file->d_type == DT_DIR) {
      // check if every character of the name is a digit
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
  float mem_utilization = 0.0;
  long mem_total = 1, mem_free = 0, mem_used = 0; //memory_total = 1 to avoid divided by 0
  string line, key, value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) { 
        if (key == "MemTotal:"){
          mem_total = std::stol(value);
        }
        if (key == "MemFree:"){ 
          if(value !="") mem_free = std::stol(value); 
          
        }
      }
    }  
  }
  mem_used = mem_total - mem_free;
  mem_utilization = 1.0* mem_used / mem_total;
  
  return mem_utilization; 
 
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, uptime_s;
  long uptime_l = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime_s;
    if(uptime_s!="") uptime_l = std::stol(uptime_s);
  }

  return uptime_l;  
}
vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return ActiveJiffies() + IdleJiffies(); 
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 

  vector<string> splited_words; 
  string line;
  long utime = 0, stime = 0, cutime = 0, cstime = 0, active_jiffies=0; 


  std::ifstream stream(kProcDirectory + "/"+ std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    splited_words = split(line, " ");

    if(splited_words[13]!="") utime = std::stol(splited_words[13]);
    if(splited_words[14]!="") stime = std::stol(splited_words[14]);
    if(splited_words[15]!="") cutime = std::stol(splited_words[15]);
    if(splited_words[16]!="") cstime = std::stol(splited_words[16]);
    
    active_jiffies =  utime + stime + cutime + cstime ;
  }
  return active_jiffies; 
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  long active_jiffies = 0;
  string cpu, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
   if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq \
      >> softirq >> steal >> guest >> guest_nice;
 
    active_jiffies = user + nice + system + irq + softirq + steal + guest + guest_nice;
  }
  return active_jiffies; 
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  long idle_jiffies = 0;
  string cpu, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
   if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq \
      >> softirq >> steal >> guest >> guest_nice;
    idle_jiffies =  idle + iowait;
  }
  return idle_jiffies; 
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line, cpu;
  vector<string> cpu_utilizations;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;

    while (linestream >> cpu) {
      cpu_utilizations.emplace_back(cpu);
    }
  }
  return cpu_utilizations;
}



float LinuxParser::CpuUtilization(int pid){
  float cpu_utilization = 0.0f;
  vector<string> splited_words; 
  string line, key, value, uptime_s;
  long utime = 0, stime = 0, cutime = 0, cstime = 0, starttime = 0; 
  long hertz = sysconf(_SC_CLK_TCK);
  long uptime = 0;
  std::ifstream uptime_stream(kProcDirectory + kUptimeFilename);
  if (uptime_stream.is_open()) {
    std::getline(uptime_stream, line);
    splited_words = split(line, " ");
    if(splited_words[0]!="") uptime = std::stol(splited_words[0]);
  }


  std::ifstream stream(kProcDirectory + "/"+ std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    splited_words = split(line, " ");

    if(splited_words[13]!="") utime = std::stol(splited_words[13]);
    if(splited_words[14]!="") stime = std::stol(splited_words[14]);
    if(splited_words[15]!="") cutime = std::stol(splited_words[15]);
    if(splited_words[16]!="") cstime = std::stol(splited_words[16]);
    if(splited_words[21]!="") starttime = std::stol(splited_words[21]);
    
    long total_time =  utime + stime + cutime + cstime ;
    long seconds = uptime - (starttime/hertz); 
    cpu_utilization = 1.0f * (total_time/hertz) / seconds; 
    
    //if (cpu_utilization >100.0) cpu_utilization = 0.0;
   
  }

  return cpu_utilization;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int total_process = 0;  
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) { 
        if (key == "processes"){
          if(value!="") total_process = std::stoi(value);
          break;
        }
      }
    }  
  }
  
  return total_process; 
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int running_processes = 0;  
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) { 
        if (key == "procs_running"){
          if(value!="") running_processes = std::stoi(value);
          break;
        }
      }
    }  
  }
  
  return running_processes; 
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string command = "";
  std::ifstream stream(kProcDirectory + "/"+ std::to_string(pid) + "/cmdline");
  if (stream.is_open()){
    std::getline(stream, command);
  }
  return command; 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string ram = "";  
  string status_file = kProcDirectory + "/"+ std::to_string(pid) + kStatusFilename;
  string line, key, value;
  std::ifstream stream(status_file);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) { 
        if (key == "VmSize:"){
          ram = value;
          break;
        }
      }
    }  
  }
  if(ram!="") ram = std::to_string(std::stol(ram)/1024);

  return ram;  
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string uid = "";  
  string line, key, value;
  string status_file = kProcDirectory + "/"+ std::to_string(pid) + kStatusFilename;
  std::ifstream stream(status_file);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) { 
        if (key == "Uid:"){
          uid = value;
          break;
        }
      }
    }  
  }
  
  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string user = "";
  string line;
  std::ifstream stream(kPasswordPath);
  string name = ("x:" + Uid(pid));
  while (getline(stream, line)) {
    if (line.find(name) != string::npos)
      user = line.substr(0, line.find(':'));
  }
  
  return user; 
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  long uptime = 0;
  string line, key, value, uptime_s;

  std::ifstream stream(kProcDirectory + "/"+ std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    //linestream >> uptime;
    using StrIt = std::istream_iterator<std::string>;
    std::vector<std::string> container{StrIt{linestream}, StrIt{}};
    if (container.size() >= 22) {
      uptime_s= container.at(21);
    }
  } 
  if(uptime_s !="") {
  uptime = UpTime()- std::stol(uptime_s) / sysconf(_SC_CLK_TCK);
  }
  return uptime; 
}