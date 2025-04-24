#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor();
  float Utilization();  // Returns the CPU utilization as a percentage (0-1)

 private:
  long prev_idle_ = 0;
  long prev_total_ = 0;
  long prev_non_idle_ = 0;
};

#endif