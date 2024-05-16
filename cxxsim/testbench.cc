#include "testbench.h"

Testbench::Testbench() {
  assert(_inst == nullptr);
  _inst = this;
}

Testbench::~Testbench() { _inst = nullptr; }

Testbench &Testbench::inst() { return *_inst; }

bool Testbench::eval(performer *performer) { return true; }

Testbench *Testbench::_inst = nullptr;

std::unique_ptr<cxxrtl_design::bb_p_CXXRTLTestbench>
cxxrtl_design::bb_p_CXXRTLTestbench::create(std::string name,
                                            metadata_map parameters,
                                            metadata_map attributes) {
  return std::make_unique<Testbench>();
}