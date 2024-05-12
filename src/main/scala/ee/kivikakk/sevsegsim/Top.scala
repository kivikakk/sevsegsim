package ee.kivikakk.sevsegsim

import _root_.circt.stage.ChiselStage
import chisel3._
import chisel3.util._
import ee.hrzn.chryse.ChryseApp
import ee.hrzn.chryse.HasIO
import ee.hrzn.chryse.platform.BoardPlatform
import ee.hrzn.chryse.platform.Platform
import ee.hrzn.chryse.platform.ice40.ICE40Platform

import java.io.PrintWriter
import ee.hrzn.chryse.platform.cxxrtl.CXXRTLOptions

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

  val d1 = io.pmod2_4
  val a  = io.pmod2_3
  val f  = io.pmod2_2
  val d2 = io.pmod2_1
  val d3 = io.pmod1a10
  val b  = io.pmod1a9
  val e  = io.pmod1a8
  val d  = io.pmod1a7
  val p  = io.pmod1a4
  val c  = io.pmod1a3
  val g  = io.pmod1a2
  val d4 = io.pmod1a1

  val flipReg     = RegInit(false.B)
  val ubtnRelease = io.ubtn & RegNext(~io.ubtn)
  when(ubtnRelease)(flipReg := !flipReg)

  val ds      = VecInit(Seq.fill(4)(false.B))
  val abcdefg = Wire(Vec(7, Bool()))
  when(flipReg) {
    a  := abcdefg(0)
    b  := abcdefg(1)
    c  := abcdefg(2)
    d  := abcdefg(3)
    e  := abcdefg(4)
    f  := abcdefg(5)
    d1 := ds(0)
    d2 := ds(1)
    d3 := ds(2)
    d4 := ds(3)
  }.otherwise {
    d  := abcdefg(0)
    e  := abcdefg(1)
    f  := abcdefg(2)
    a  := abcdefg(3)
    b  := abcdefg(4)
    c  := abcdefg(5)
    d1 := ds(3)
    d2 := ds(2)
    d3 := ds(1)
    d4 := ds(0)
  }
  g := abcdefg(6)

  val ss = Module(new SevSeg)
  abcdefg := ss.io.abcdefg
  p       := true.B

  val chars     = VecInit(SevSegChar.P, SevSegChar.O, SevSegChar.G, SevSegChar.Off)
  val charIxReg = RegInit(0.U(2.W))
  ss.io.char := chars(charIxReg)

  ds(0)         := false.B
  ds(1)         := false.B
  ds(2)         := false.B
  ds(3)         := false.B
  ds(charIxReg) := true.B

  val perSegTime = platform.clockHz / 1000
  val timerReg   = RegInit(0.U(unsignedBitLength(perSegTime - 1).W))
  timerReg := Mux(timerReg === (perSegTime - 1).U, 0.U, timerReg + 1.U)
  when(timerReg === (perSegTime - 1).U) {
    charIxReg := Mux(charIxReg === 3.U, 0.U, charIxReg + 1.U)
  }
}

object Top extends ChryseApp {
  override val name: String = "sevsegsim"
  override val targetPlatforms: Seq[BoardPlatform] = Seq(
    ICE40Platform(),
  )
  override val cxxrtlOptions: Option[CXXRTLOptions] = Some(
    CXXRTLOptions(clockHz = 3_000_000),
  )

  override def genTop(implicit platform: Platform) = new Top()
}
