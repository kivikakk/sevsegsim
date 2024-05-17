package ee.kivikakk.sevsegsim

import chisel3._

class CXXRTLTestbench extends BlackBox {
  val io = IO(new Bundle {
    val clock    = Input(Clock())
    val ubtn     = Output(Bool())
    val abcdefgp = Input(Vec(8, Bool()))
    val ds       = Input(Vec(4, Bool()))
  })
}
