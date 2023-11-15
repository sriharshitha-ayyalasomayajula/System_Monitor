#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

template <typename Type> 
Type getValueOfFile(std::string const &filename) {
  std::string line;
  Type value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    stream.close();
  }
  return value;
};


template <typename Type> 
Type findValueByKey(std::string const &keyFilter, std::string const &filename) {
  std::string line, key;
  Type value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key ) {
        if (key == keyFilter) {
          linestream >> value;
        }
      }
    }
 
  }
  stream.close();
  return value;
};

template <typename Type> 
vector<Type> getValueInaVector(std::string const &filename) {
  Type value;
  std::string line;
  vector<Type> status;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      status.emplace_back(value);
    }
  }
  stream.close();
  return status;
}

//  An example of how to read data from the filesystem
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
  filestream.close();
  }
  return value;
}

//  An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  stream.close();
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

//  Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float value;
  vector<float> memUsed;
  std::ifstream filemem(kProcDirectory + kMeminfoFilename);
  if (filemem.is_open()) {
    while (std::getline(filemem, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:" || key == "MemFree:") {
          memUsed.emplace_back(value);
        }
      }
    }
    filemem.close();
  }
  // Used MemTotal - MemFree to determine Memory used by the system and divided
  // by Memtotal to express in decimals
  return (memUsed[0] - memUsed[1]) / memUsed[0];
}

//  Read and return the system uptime
long LinuxParser::UpTime() {
  float uptime, suspendtime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> suspendtime;
  stream.close(); 
  }
 
  return uptime;
}

//  Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return LinuxParser::UpTime()/sysconf(_SC_CLK_TCK) ;
 }

// Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string value;
  vector<string> status;
  double pidCPUptime;
  double cputimeinseconds;

  std::ifstream filepids(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filepids.is_open()) {
    std::getline(filepids, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      status.emplace_back(value);
    }
  filepids.close();
  }
  pidCPUptime = (stof(status[kUtime_]) + stof(status[kStime_]) + stof(status[kCutime_]) + stof(status[kCstime_])) / sysconf(_SC_CLK_TCK);
  cputimeinseconds = (stof(status[kStarttime_])/sysconf(_SC_CLK_TCK));
  return cputimeinseconds-pidCPUptime;
}

//  Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  auto value = LinuxParser::CpuUtilization();
  return value[kUser_] + value[kNice_] + value[kSystem_] + value[kIRQ_] +
         value[kSoftIRQ_] + value[kSteal_];
}

//  Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  auto value = LinuxParser::CpuUtilization();
  return value[kIdle_] + value[kIOwait_];
}

//  Read and return CPU utilization
vector<long> LinuxParser::CpuUtilization() {
  string line;
  string key;
  float value;
  vector<long> cpusused;
  std::ifstream filestat(kProcDirectory + kStatFilename);
  if (filestat.is_open()) {
    while (std::getline(filestat, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu") {
          while (linestream >> value) {
            cpusused.emplace_back(value);
          }
        }
      }
    }
  filestat.close();
  }
  return cpusused;
}


//  Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value = 0;

  std::ifstream filemem(kProcDirectory + kStatFilename);
  if (filemem.is_open()) {
    while (std::getline(filemem, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  filemem.close();
  }
  return value;
}

//  Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value = 0;

  std::ifstream fileproc(kProcDirectory + kStatFilename);
  if (fileproc.is_open()) {
    while (std::getline(fileproc, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  fileproc.close();
  }

  return value;
}

//  Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
// Refactoring this function following menthor recommedation to use getValueofFile
string LinuxParser::Command(int pid) {
 return getValueOfFile<std::string>(to_string(pid)+kCmdlineFilename) ;
}

//  Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  return std::to_string(findValueByKey<long>(filterProcMem, to_string(pid) + kStatusFilename)/1024);
}

//  Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;

  std::ifstream fileuser(kProcDirectory + std::to_string(pid) +  kStatusFilename);
  if (fileuser.is_open()) {
    while (std::getline(fileuser, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "Uid:") {
          linestream >> value;
        }
      }
    }
  fileuser.close();
  }
  return value;
}

// Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line;
  string key;
  string passwd;
  string userid;

  std::ifstream fileuser(kPasswordPath);
  if (fileuser.is_open()) {
    while (std::getline(fileuser, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> passwd >> userid) {
        if (userid == LinuxParser::Uid(pid)) {
          return key;
        }
      }
    }
  fileuser.close();
  }
  return key;
}

// Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {

  string line;
  string value;
  vector<string> status;
  long pidUptime;
  
  std::ifstream filepids(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filepids.is_open()) {
    std::getline(filepids, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      status.emplace_back(value);
    }
  filepids.close();
  }
  pidUptime = stof(status[kStarttime_])/sysconf(_SC_CLK_TCK);
  return pidUptime;  
 
 // vector<long> status=getValueInaVector<long>(to_string(pid)+kStatFilename);
 //  return status[kStarttime_]/sysconf(_SC_CLK_TCK);
}