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

  static SimThread *start(const std::optional<std::string> &vcd_out);

  int main();

  bool lock_if_running();
  void lock();
  void unlock();
  int wait();

  uint64_t cycle_number();
  void halt();

private:
  void sync_reset();
  void cycle();
  int write_vcd();

  cxxrtl_design::p_sevsegsim _top;
  std::optional<std::string> _vcd_out;
  uint64_t _vcd_time;
  cxxrtl::vcd_writer _vcd;

  SDL_Thread *_thread;
  SDL_mutex *_mutex;
  bool _running;
};

#endif
