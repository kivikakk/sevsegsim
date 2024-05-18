#include <fstream>
#include <tuple>

#include "SimThread.h"

static int simmain(void *vsim) {
  static_cast<SimThread *>(vsim)->main();
  return 0;
}

SimThread::SimThread(const std::optional<std::string> &vcd_out)
    : _top(), _vcd_out(vcd_out), _vcd_time(0), _vcd(), _running(true) {
  if (_vcd_out) {
    debug_items di;
    _top.debug_info(&di, nullptr, "top ");
    _vcd.add(di);
  }
  _mutex = SDL_CreateMutex();
}

SimThread::~SimThread() { SDL_DestroyMutex(_mutex); }

SimThread *SimThread::start(const std::optional<std::string> &vcd_out) {
  auto sim = new SimThread(vcd_out);
  sim->_thread = SDL_CreateThread(simmain, "simmain", sim);
  return sim;
}

void SimThread::main() {
  while (lock_if_running()) {
    cycle();
    unlock();
  }

  write_vcd();
}

bool SimThread::lock_if_running() {
  lock();
  if (!_running) {
    unlock();
    return false;
  }
  return true;
}

void SimThread::lock() { SDL_LockMutex(_mutex); }
void SimThread::unlock() { SDL_UnlockMutex(_mutex); }
void SimThread::wait() { SDL_WaitThread(_thread, nullptr); }

uint64_t SimThread::cycle_number() { return _vcd_time >> 1; }

void SimThread::halt() { _running = false; }

void SimThread::sync_reset() {
  _top.p_reset.set(true);
  cycle();
  _top.p_reset.set(false);
}

void SimThread::cycle() {
  _top.p_clock.set(true);
  _top.step();
  _vcd.sample(_vcd_time++);

  _top.p_clock.set(false);
  _top.step();
  _vcd.sample(_vcd_time++);
}

void SimThread::write_vcd() {
  if (_vcd_out) {
    std::ofstream of(*_vcd_out);
    of << _vcd.buffer;
  }
}
