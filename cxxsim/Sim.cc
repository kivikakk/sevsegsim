#include <fstream>

#include "Sim.h"

static int simmain(void *vsim) {
  static_cast<Sim *>(vsim)->main();
  return 0;
}

bool SimController::running() { return SDL_AtomicGet(&_running); }
void SimController::halt() { SDL_AtomicSet(&_running, 0); }
void SimController::wait() { SDL_WaitThread(_thread, nullptr); }

SimController::SimController() : _thread(nullptr) {
  SDL_AtomicSet(&_running, 0);
}

SimController *Sim::start(const std::optional<std::string> &vcd_out) {
  Sim *sim = new Sim(vcd_out);
  sim->_thread = SDL_CreateThread(simmain, "simmain", sim);
  SDL_AtomicSet(&sim->_running, 1);
  return sim;
}

void Sim::main() {
  while (!SDL_AtomicGet(&_running))
    ;

  while (SDL_AtomicGet(&_running)) {
    cycle();
  }

  write_vcd();
}

uint64_t Sim::cycle_number() { return SDL_AtomicGet(&_cycle_number); }

Sim::Sim(const std::optional<std::string> &vcd_out)
    : _top(), _vcd_out(vcd_out), _vcd_time(0), _vcd() {
  SDL_AtomicSet(&_cycle_number, 0);
  if (_vcd_out) {
    debug_items di;
    _top.debug_info(&di, nullptr, "top ");
    _vcd.add(di);
  }
}

void Sim::sync_reset() {
  _top.p_reset.set(true);
  cycle();
  _top.p_reset.set(false);
}

void Sim::cycle() {
  _top.p_clock.set(true);
  _top.step();
  _vcd.sample(_vcd_time++);

  _top.p_clock.set(false);
  _top.step();
  _vcd.sample(_vcd_time++);

  SDL_AtomicAdd(&_cycle_number, 1);
}

void Sim::write_vcd() {
  if (_vcd_out) {
    std::ofstream of(*_vcd_out);
    of << _vcd.buffer;
  }
}
