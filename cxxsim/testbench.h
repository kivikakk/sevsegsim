#ifndef TESTBENCH_H
#define TESTBENCH_H

#include <sevsegsim.h>

class Testbench : public cxxrtl_design::bb_p_CXXRTLTestbench {
public:
  Testbench();
  ~Testbench();

  static Testbench &inst();

  bool eval(performer *performer) override;

private:
  static Testbench *_inst;
};

#endif
