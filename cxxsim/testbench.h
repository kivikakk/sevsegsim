#ifndef TESTBENCH_H
#define TESTBENCH_H

#include <sevsegsim.h>

typedef bool segment_t;

class Testbench : public cxxrtl_design::bb_p_CXXRTLTestbench {
public:
  Testbench();
  ~Testbench();

  static Testbench &inst();

  void reset() override;
  bool eval(performer *performer) override;

  void segacts(segment_t out[4 * 7]) const;

private:
  static Testbench *_inst;

  segment_t _segacts[4 * 7];
};

#endif
