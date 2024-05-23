package ee.kivikakk.sevsegsim

import chisel3._
import chiseltest._
import chiseltest.simulator.WriteVcdAnnotation
import ee.hrzn.chryse.platform.Platform
import org.scalatest.flatspec.AnyFlatSpec

class TopSpec extends AnyFlatSpec with ChiselScalatestTester {
  behavior.of("Top")

  implicit val plat: Platform = new Platform {
    val id      = "topspec"
    val clockHz = 10_000
  }

  it should "do something :)" in {
    test(new Top).withAnnotations(Seq(WriteVcdAnnotation)) { c => }
  }
}
