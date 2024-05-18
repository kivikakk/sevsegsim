#ifndef SIM_H
#define SIM_H

#include <SDL.h>
#include <cxxrtl/cxxrtl_vcd.h>
#include <optional>
#include <sevsegsim.h>

#include "Testbench.h"

class SimThread {
private:
  SimThread(const std::optional<std::string> &vcd_out);

public:
  ~SimThread();

  static std::tuple<SimThread *, SDL_Thread *>
  start(const std::optional<std::string> &vcd_out);

  void main();

  bool lock_if_running();
  void lock();
  void unlock();

  uint64_t cycle_number();
  void halt();

private:
  void sync_reset();
  void cycle();
  void write_vcd();

  cxxrtl_design::p_top _top;
  std::optional<std::string> _vcd_out;
  uint64_t _vcd_time;
  cxxrtl::vcd_writer _vcd;

  SDL_mutex *_mutex;
  bool _running;
};

#endif
