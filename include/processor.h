#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  

  // Declare any necessary private members
 private:
  long pActive_{0};
  long pIdle_{0};
};

#endif