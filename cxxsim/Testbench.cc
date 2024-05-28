#include <cxxrtl/cxxrtl.h>

#include "Testbench.h"

Testbench::Testbench() {
  assert(_inst == nullptr);
  _inst = this;
}

Testbench::~Testbench() { _inst = nullptr; }

Testbench &Testbench::inst() { return *_inst; }

void Testbench::reset() { p_io__ubtn.set(false); }

bool Testbench::eval(performer *performer) {
  int ix = -1;

  if (p_io__ds__0)
    ix = 0;
  if (p_io__ds__1)
    ix = 1;
  if (p_io__ds__2)
    ix = 2;
  if (p_io__ds__3)
    ix = 3;

  if (ix != -1) {
    _segacts[ix * 7 + 0] = !p_io__abcdefgp__0;
    _segacts[ix * 7 + 1] = !p_io__abcdefgp__1;
    _segacts[ix * 7 + 2] = !p_io__abcdefgp__2;
    _segacts[ix * 7 + 3] = !p_io__abcdefgp__3;
    _segacts[ix * 7 + 4] = !p_io__abcdefgp__4;
    _segacts[ix * 7 + 5] = !p_io__abcdefgp__5;
    _segacts[ix * 7 + 6] = !p_io__abcdefgp__6;
  }

  return true;
}

void Testbench::segacts(segment_t out[4 * 7]) const {
  memcpy(out, _segacts, sizeof(_segacts));
}

Testbench *Testbench::_inst = nullptr;

std::unique_ptr<cxxrtl_design::bb_p_CXXRTLTestbench>
cxxrtl_design::bb_p_CXXRTLTestbench::create(std::string name,
                                            metadata_map parameters,
                                            metadata_map attributes) {
  return std::make_unique<Testbench>();
}