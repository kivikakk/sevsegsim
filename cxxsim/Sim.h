#ifndef SIM_H
#define SIM_H

#include <SDL.h>
#include <cxxrtl/cxxrtl_vcd.h>
#include <optional>
#include <sevsegsim.h>

class SimController {
public:
  // All safe to call from the main thread.
  bool running();
  void halt();
  void wait();

  virtual uint64_t cycle_number() = 0;
  virtual uint64_t elapsed_cycles() = 0;

protected:
  SimController();

  SDL_atomic_t _running;
  SDL_Thread *_thread;
};

class Sim : public SimController {
public:
  static SimController *start(const std::optional<std::string> &vcd_out);

  void main();

  uint64_t cycle_number() override;
  uint64_t elapsed_cycles() override;

private:
  Sim(const std::optional<std::string> &vcd_out);

  void sync_reset();
  void cycle();
  void write_vcd();

  cxxrtl_design::p_top _top;
  std::optional<std::string> _vcd_out;
  uint64_t _vcd_time;
  cxxrtl::vcd_writer _vcd;

  SDL_atomic_t _cycle_number;
  uint64_t _last_elapsed;
};

#endif
