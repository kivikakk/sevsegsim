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
import ee.hrzn.chryse.platform.ice40.IceBreakerPlatform

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
class Top(implicit platform: Platform) extends Module with HasIO[TopIO] {
  def createIo() = new TopIO

  val ubtn    = Wire(Bool())
  val ds      = Wire(Vec(4, Bool()))
  val abcdefg = Wire(Vec(7, Bool()))

  platform match {
    case CXXRTLPlatform(_) =>
      val bb = Module(new CXXRTLTestbench)
      bb.io.clock := clock

      for { (o, i) <- bb.io.abcdefgp.zip(abcdefg.appended(true.B)) }
        o      := i
      bb.io.ds := ds

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

      ubtn := bb.io.ubtn

    case _ =>
      ubtn       := io.ubtn
      io.pmod2_3 := abcdefg(0)
      io.pmod1a9 := abcdefg(1)
      io.pmod1a3 := abcdefg(2)
      io.pmod1a7 := abcdefg(3)
      io.pmod1a8 := abcdefg(4)
      io.pmod2_2 := abcdefg(5)
      io.pmod1a2 := abcdefg(6)

      io.pmod2_4  := ds(0)
      io.pmod2_1  := ds(1)
      io.pmod1a10 := ds(2)
      io.pmod1a1  := ds(3)

      io.pmod1a4 := true.B // period
  }

  val flipReg     = RegInit(false.B)
  val ubtnRelease = ubtn & RegNext(~ubtn)
  when(ubtnRelease)(flipReg := !flipReg)

  val flipper = Module(new Flipper)
  flipper.io.flip := flipReg
  abcdefg         := flipper.io.segs_out
  ds              := flipper.io.d_out

  val ss = Module(new SevSeg)
  flipper.io.segs_in := ss.io.abcdefg

  val chars     = VecInit(SevSegChar.P, SevSegChar.O, SevSegChar.N, SevSegChar.G)
  val charIxReg = RegInit(0.U(2.W))
  val dsCycle   = VecInit(Seq.fill(4)(false.B))
  ss.io.char := chars(charIxReg)

  dsCycle(charIxReg) := true.B
  flipper.io.d_in    := dsCycle

  val perSegTime = Seq(platform.clockHz / 1_000_000, 2).max
  val timerReg   = RegInit(0.U(unsignedBitLength(perSegTime - 1).W))
  timerReg := Mux(timerReg === (perSegTime - 1).U, 0.U, timerReg + 1.U)
  when(timerReg === (perSegTime - 1).U) {
    charIxReg := Mux(charIxReg === 3.U, 0.U, charIxReg + 1.U)
  }
}

object Top extends ChryseApp {
  override val name            = "sevsegsim"
  override val targetPlatforms = Seq(IceBreakerPlatform())
  override val cxxrtlOptions = Some(
    CXXRTLOptions(
      clockHz = 3_000_000,
      blackboxes = Seq(classOf[CXXRTLTestbench]),
      pkgConfig = Seq("sdl2"),
    ),
  )

  override def genTop(implicit platform: Platform) = new Top()
}
