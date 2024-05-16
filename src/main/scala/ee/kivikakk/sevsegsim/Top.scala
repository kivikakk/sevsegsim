package ee.kivikakk.sevsegsim

import _root_.circt.stage.ChiselStage
import chisel3._
import chisel3.util._
import ee.hrzn.chryse.ChryseApp
import ee.hrzn.chryse.HasIO
import ee.hrzn.chryse.platform.BoardPlatform
import ee.hrzn.chryse.platform.Platform
import ee.hrzn.chryse.platform.cxxrtl.CXXRTLOptions
import ee.hrzn.chryse.platform.cxxrtl.CXXRTLPlatform
import ee.hrzn.chryse.platform.ice40.ICE40Platform

class TopIO extends Bundle {
  val ubtn     = Input(Bool())
  val pmod1a1  = Output(Bool())
  val pmod1a2  = Output(Bool())
  val pmod1a3  = Output(Bool())
  val pmod1a4  = Output(Bool())
  val pmod1a7  = Output(Bool())
  val pmod1a8  = Output(Bool())
  val pmod1a9  = Output(Bool())
  val pmod1a10 = Output(Bool())
  val pmod2_1  = Output(Bool())
  val pmod2_2  = Output(Bool())
  val pmod2_3  = Output(Bool())
  val pmod2_4  = Output(Bool())
}

// This feels very un-Chisel.
case class Pinout(
    d1: Data,
    a: Data,
    f: Data,
    d2: Data,
    d3: Data,
    b: Data,
    e: Data,
    d: Data,
    p: Data,
    c: Data,
    g: Data,
    d4: Data,
)

class Top(implicit platform: Platform) extends Module with HasIO[TopIO] {
  def createIo() = new TopIO

  val pinout = platform match {
    case CXXRTLPlatform(_) =>
      val bb = Module(new CXXRTLTestbench)
      bb.io.clock := clock

      // TODO: change the IO based on the platform to not necessitate this.
      io.pmod1a1  := DontCare
      io.pmod1a2  := DontCare
      io.pmod1a3  := DontCare
      io.pmod1a4  := DontCare
      io.pmod1a7  := DontCare
      io.pmod1a8  := DontCare
      io.pmod1a9  := DontCare
      io.pmod1a10 := DontCare
      io.pmod2_1  := DontCare
      io.pmod2_2  := DontCare
      io.pmod2_3  := DontCare
      io.pmod2_4  := DontCare

      Pinout(
        d1 = bb.io.ds(0),
        d2 = bb.io.ds(1),
        d3 = bb.io.ds(2),
        d4 = bb.io.ds(3),
        a = bb.io.abcdefgp(0),
        b = bb.io.abcdefgp(1),
        c = bb.io.abcdefgp(2),
        d = bb.io.abcdefgp(3),
        e = bb.io.abcdefgp(4),
        f = bb.io.abcdefgp(5),
        g = bb.io.abcdefgp(6),
        p = bb.io.abcdefgp(7),
      )
    case _ =>
      Pinout(
        d1 = io.pmod2_4,
        a = io.pmod2_3,
        f = io.pmod2_2,
        d2 = io.pmod2_1,
        d3 = io.pmod1a10,
        b = io.pmod1a9,
        e = io.pmod1a8,
        d = io.pmod1a7,
        p = io.pmod1a4,
        c = io.pmod1a3,
        g = io.pmod1a2,
        d4 = io.pmod1a1,
      )
  }

  val flipReg     = RegInit(false.B)
  val ubtnRelease = io.ubtn & RegNext(~io.ubtn)
  when(ubtnRelease)(flipReg := !flipReg)

  val ds      = VecInit(Seq.fill(4)(false.B))
  val abcdefg = Wire(Vec(7, Bool()))
  when(flipReg) {
    pinout.a  := abcdefg(0)
    pinout.b  := abcdefg(1)
    pinout.c  := abcdefg(2)
    pinout.d  := abcdefg(3)
    pinout.e  := abcdefg(4)
    pinout.f  := abcdefg(5)
    pinout.d1 := ds(0)
    pinout.d2 := ds(1)
    pinout.d3 := ds(2)
    pinout.d4 := ds(3)
  }.otherwise {
    pinout.d  := abcdefg(0)
    pinout.e  := abcdefg(1)
    pinout.f  := abcdefg(2)
    pinout.a  := abcdefg(3)
    pinout.b  := abcdefg(4)
    pinout.c  := abcdefg(5)
    pinout.d1 := ds(3)
    pinout.d2 := ds(2)
    pinout.d3 := ds(1)
    pinout.d4 := ds(0)
  }
  pinout.g := abcdefg(6)

  val ss = Module(new SevSeg)
  abcdefg  := ss.io.abcdefg
  pinout.p := true.B

  val chars     = VecInit(SevSegChar.P, SevSegChar.O, SevSegChar.N, SevSegChar.G)
  val charIxReg = RegInit(0.U(2.W))
  ss.io.char := chars(charIxReg)

  ds(charIxReg) := true.B

  val perSegTime = platform.clockHz / 1_000_000
  val timerReg   = RegInit(0.U(unsignedBitLength(perSegTime - 1).W))
  timerReg := Mux(timerReg === (perSegTime - 1).U, 0.U, timerReg + 1.U)
  when(timerReg === (perSegTime - 1).U) {
    charIxReg := Mux(charIxReg === 3.U, 0.U, charIxReg + 1.U)
  }
}

object Top extends ChryseApp {
  override val name            = "sevsegsim"
  override val targetPlatforms = Seq(ICE40Platform())
  override val cxxrtlOptions = Some(
    CXXRTLOptions(
      clockHz = 3_000_000,
      blackboxes = Seq(classOf[CXXRTLTestbench]),
      pkgConfig = Seq("sdl2"),
    ),
  )

  override def genTop(implicit platform: Platform) = new Top()
}
