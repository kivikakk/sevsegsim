#ifndef TESTBENCH_H
#define TESTBENCH_H

#include <sevsegsim.h>

class Testbench : public cxxrtl_design::bb_p_CXXRTLTestbench {
public:
  Testbench();
  ~Testbench();

  static Testbench &inst();

private:
  static Testbench *_inst;
};

#endif
