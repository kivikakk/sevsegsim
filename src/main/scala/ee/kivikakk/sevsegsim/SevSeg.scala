package ee.kivikakk.sevsegsim

import chisel3._
import chisel3.util._

class SevSeg extends Module {
  val io = IO(new Bundle {
    val char = Input(SevSegChar())
    val abcdefg = Output(Vec(7, Bool()))
  })

  io.abcdefg := Seq(true.B, true.B, true.B, true.B, true.B, true.B, true.B)

  //  aaa
  // f   b
  // f   b
  //  ggg
  // e   c
  // e   c
  //  ddd  p

  switch(io.char) {
    is(SevSegChar.P) {
      io.abcdefg := Seq(false.B, false.B, true.B, true.B, false.B, false.B, false.B)
    }
    is(SevSegChar.O) {
      io.abcdefg := Seq(false.B, false.B, false.B, false.B, false.B, false.B, true.B)
    }
    is(SevSegChar.N) {
      io.abcdefg := Seq(true.B, true.B, false.B, true.B, false.B, true.B, false.B)
    }
    is(SevSegChar.G) {
      io.abcdefg := Seq(false.B, false.B, false.B, false.B, true.B, false.B, false.B)
    }
    is(SevSegChar.Off) {
      io.abcdefg := Seq(true.B, true.B, true.B, true.B, true.B, true.B, true.B)
    }
  }
}

object SevSegChar extends ChiselEnum {
  val P, O, N, G, Off = Value
}