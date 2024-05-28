package ee.kivikakk.sevsegsim

import chisel3._
import chisel3.experimental.ExtModule

class CXXRTLTestbench extends ExtModule {
  val io = IO(new Bundle {
    val clock    = Input(Clock())
    val ubtn     = Output(Bool())
    val abcdefgp = Input(Vec(8, Bool()))
    val ds       = Input(Vec(4, Bool()))
  })
}
