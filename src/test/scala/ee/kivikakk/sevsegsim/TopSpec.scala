package ee.kivikakk.sevsegsim

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import ee.hrzn.chryse.platform.Platform
import org.scalatest.flatspec.AnyFlatSpec

class TopSpec extends AnyFlatSpec {
  behavior.of("Top")

  implicit val plat: Platform = new Platform {
    val id      = "topspec"
    val clockHz = 10_000
  }

  it should "do something :)" in {
    simulate(new Top) { c => }
  }
}
