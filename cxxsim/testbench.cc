#include <cxxrtl/cxxrtl.h>

#include "Testbench.h"

Testbench::Testbench() : _was_ds3(false) {
  assert(_inst == nullptr);
  _inst = this;
}

Testbench::~Testbench() { _inst = nullptr; }

Testbench &Testbench::inst() { return *_inst; }

void Testbench::reset() { p_ubtn.set(true); }

#define INC_SEGACT(sa) (sa = (sa == 255) ? 255 : (sa + 1))
#define DEC_SEGACT(sa) (sa = (sa == 0) ? 0 : (sa - 1))

bool Testbench::eval(performer *performer) {
  int ix = -1;

  if (p_ds__0)
    ix = 0;
  if (p_ds__1)
    ix = 1;
  if (p_ds__2)
    ix = 2;
  if (p_ds__3)
    ix = 3;

  if (ix != -1) {
    if (!p_abcdefgp__0)
      INC_SEGACT(_segacts[ix * 7 + 0]);
    if (!p_abcdefgp__1)
      INC_SEGACT(_segacts[ix * 7 + 1]);
    if (!p_abcdefgp__2)
      INC_SEGACT(_segacts[ix * 7 + 2]);
    if (!p_abcdefgp__3)
      INC_SEGACT(_segacts[ix * 7 + 3]);
    if (!p_abcdefgp__4)
      INC_SEGACT(_segacts[ix * 7 + 4]);
    if (!p_abcdefgp__5)
      INC_SEGACT(_segacts[ix * 7 + 5]);
    if (!p_abcdefgp__6)
      INC_SEGACT(_segacts[ix * 7 + 6]);
  }

  if (_was_ds3 && p_ds__0) {
    _was_ds3 = false;
    for (int i = 0; i < 4 * 7; ++i)
      DEC_SEGACT(_segacts[i]);
  } else if (!_was_ds3 && p_ds__3) {
    _was_ds3 = true;
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