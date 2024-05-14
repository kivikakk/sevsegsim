#include <fstream>

#include <cxxrtl/cxxrtl_vcd.h>
#include <sevsegsim.h>

int main(int argc, char **argv) {
  cxxrtl_design::p_top top;

  bool do_vcd = argc >= 3 && std::string(argv[1]) == "--vcd";
  std::string vcd_out = do_vcd ? std::string(argv[2]) : "";
  cxxrtl::vcd_writer vcd;
  uint64_t vcd_time = 0;
  if (do_vcd) {
    debug_items di;
    top.debug_info(&di, nullptr, "top ");
    vcd.add(di);
  }

  top.p_reset.set(true);
  top.p_clock.set(true);
  top.step();
  vcd.sample(vcd_time++);
  top.p_clock.set(false);
  top.step();
  vcd.sample(vcd_time++);
  top.p_reset.set(false);

  if (do_vcd) {
    std::ofstream of(vcd_out);
    of << vcd.buffer;
  }

  return 0;
}