package ee.kivikakk.sevsegsim

import _root_.circt.stage.ChiselStage
import chisel3._
import chisel3.experimental.dataview._
import chisel3.util._
import ee.hrzn.chryse.ChryseApp
import ee.hrzn.chryse.platform.BoardPlatform
import ee.hrzn.chryse.platform.Platform
import ee.hrzn.chryse.platform.cxxrtl.CXXRTLOptions
import ee.hrzn.chryse.platform.cxxrtl.CXXRTLPlatform
import ee.hrzn.chryse.platform.ice40.IceBreakerPlatform
import ee.hrzn.chryse.platform.resource.implicits._

class Top(platform: Platform) extends Module {
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

      ubtn := bb.io.ubtn

    case plat: IceBreakerPlatform =>
      ubtn                     := plat.resources.ubtn
      plat.resources.pmod2_3.o := abcdefg(0)
      plat.resources.pmod1a9.o := abcdefg(1)
      plat.resources.pmod1a3.o := abcdefg(2)
      plat.resources.pmod1a7.o := abcdefg(3)
      plat.resources.pmod1a8.o := abcdefg(4)
      plat.resources.pmod2_2.o := abcdefg(5)
      plat.resources.pmod1a2.o := abcdefg(6)

      plat.resources.pmod2_4.o  := ds(0)
      plat.resources.pmod2_1.o  := ds(1)
      plat.resources.pmod1a10.o := ds(2)
      plat.resources.pmod1a1.o  := ds(3)

      plat.resources.pmod1a4.o := true.B // period
    case _ =>
      // Unit test.
      ubtn := false.B
  }

  val flipReg     = RegInit(false.B)
  val ubtnRelease = ~ubtn & RegNext(ubtn)
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
  override val genTop          = new Top(_)
  override val targetPlatforms = Seq(IceBreakerPlatform())
  override val cxxrtlOptions = Some(
    CXXRTLOptions(
      clockHz = 3_000_000,
      blackboxes = Seq(classOf[CXXRTLTestbench]),
      pkgConfig = Seq("sdl2"),
    ),
  )
}
