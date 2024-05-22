package ee.kivikakk.sevsegsim

import chisel3._

class Flipper extends RawModule {
  val io = IO(new Bundle {
    val flip     = Input(Bool())
    val d_in     = Input(Vec(4, Bool()))
    val segs_in  = Input(Vec(7, Bool()))
    val d_out    = Output(Vec(4, Bool()))
    val segs_out = Output(Vec(7, Bool()))
  })

  io.segs_out := io.segs_in
  io.d_out    := io.d_in

  when(io.flip) {
    for { (oIx, iIx) <- Seq(3, 4, 5, 0, 1, 2, 6).zipWithIndex }
      io.segs_out(oIx) := io.segs_in(iIx)
    for { (o, i) <- io.d_out.reverseIterator.zip(io.d_in) } o := i
  }
}
