#include "processor.h"

#include <string>

#include "linux_parser.h"

// Return the aggregate CPU utilization based on the explanation from
float Processor::Utilization() {
  float utilization{0};
  long cpuActive = LinuxParser::ActiveJiffies();
  long cpuIdle = LinuxParser::IdleJiffies();

  long timeActive = cpuActive - pActive_;
  long timeIdle = cpuIdle - pIdle_;
  long totalTimeElapse = timeActive + timeIdle;

  utilization = static_cast<float>(timeActive) / totalTimeElapse;

  pActive_ = cpuActive;
  pIdle_ = cpuIdle;
  return utilization;
}
