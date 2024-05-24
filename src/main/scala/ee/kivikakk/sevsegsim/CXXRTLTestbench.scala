package ee.kivikakk.sevsegsim

import chisel3._
import ee.hrzn.chryse.chisel.addAttribute

class CXXRTLTestbench extends RawModule {
  val io = IO(new Bundle {
    val clock    = Input(Clock())
    val ubtn     = Output(Bool())
    val abcdefgp = Input(Vec(8, Bool()))
    val ds       = Input(Vec(4, Bool()))
  })

  addAttribute(io.clock, "cxxrtl_edge = \"p\"")
  addAttribute(io.ubtn, "cxxrtl_sync")

  io.ubtn := DontCare
  dontTouch(io)
}
