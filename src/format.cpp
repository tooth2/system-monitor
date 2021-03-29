#include <string>

#include "format.h"

using std::string;


// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function --> long seconds
string Format::ElapsedTime(long seconds) { 
    int hh, mm, ss;
    string elapsed_time = "";
    hh = seconds / 3600;
    mm = (seconds - hh * 3600) / 60; 
    ss = seconds % 60;
    if (hh < 10){
        elapsed_time += "0";
    }
    elapsed_time += std::to_string(hh) + ":" ;
    if (mm < 10){
        elapsed_time += "0";
    }
    elapsed_time += std::to_string(mm) + ":";
    if (ss < 10){
        elapsed_time += "0";
    }
    elapsed_time += std::to_string(ss);
  
  return elapsed_time; 
}