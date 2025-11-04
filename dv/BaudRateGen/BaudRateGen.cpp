#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <NyuCatch2TestUtil.hpp>

#include <TestHelpers.hpp>
#include <VBaudRateGen.h>

TEST_CASE("BaudRateGen, Reset") {
  auto& rg {nyu::get_dut_catch2<VBaudRateGen>()};
  nyu::reset(rg, 0);

  REQUIRE(rg.rxClk == 0);
  REQUIRE(rg.txClk == 0);

  nyu::reset(rg, 1);

  REQUIRE(rg.rxClk == 1);
  REQUIRE(rg.txClk == 1);
}

TEST_CASE("BaudRateGen, phase") {
  auto& rg {nyu::get_dut_catch2<VBaudRateGen>()};
  nyu::reset(rg);

  rg.syncReset = 1;
  rg.phase = 1;

  nyu::tick(rg);

  REQUIRE(rg.rxClk == 1);
  REQUIRE(rg.txClk == 1);

  rg.phase = 0;
  nyu::tick(rg);

  REQUIRE(rg.rxClk == 0);
  REQUIRE(rg.txClk == 0);
}


TEST_CASE("BaudRateGen, rxClk") {
  auto& rg {nyu::get_dut_catch2<VBaudRateGen>()};

  for(unsigned i {2}; i < (1 << rxWidth); i <<= 1) {
    nyu::reset(rg, 0, i << rxShift);
    unsigned offset {i - ((i >> 1) + 1)};

    for(unsigned j {offset}; j < i - 1; ++j) {
      REQUIRE(rg.rxClk == 0);
      nyu::tick(rg);
    }

    REQUIRE(rg.rxClk == 1);
    nyu::tick(rg);

    for(unsigned j {0}; j < i - 1; ++j) {
      REQUIRE(rg.rxClk == 0);
      nyu::tick(rg);
    }

    REQUIRE(rg.rxClk == 1);
    nyu::tick(rg);
  }
}

TEST_CASE("BaudRateGen, txClk") {
  auto& rg {nyu::get_dut_catch2<VBaudRateGen>()};

  for(unsigned i {2}; i < (1 << txWidth); i <<= 1) {
    nyu::reset(rg, 0, i);

    for(unsigned j {0}; j < i - 1; ++j) {
      REQUIRE(rg.txClk == 0);
      nyu::tick(rg);
    }

    REQUIRE(rg.txClk == 1);
    nyu::tick(rg);

    for(unsigned j {0}; j < i - 1; ++j) {
      REQUIRE(rg.txClk == 0);
      nyu::tick(rg);
    }

    REQUIRE(rg.txClk == 1);
  }
}

TEST_CASE("BaudRateGen, rx/tx sync") {
  auto& rg {nyu::get_dut_catch2<VBaudRateGen>()};

  for(unsigned rate {2 << 4}; rate < 500; rate += 1) {
    nyu::reset(rg, 0, rate);
    for(unsigned i {0}; i < 5; ++i) {
      unsigned ticks {rg.rxClk};
      for(; rg.txClk != 1; ticks += rg.rxClk)
        nyu::tick(rg);

      REQUIRE(ticks == Oversample);

      nyu::tick(rg);
    }
  }
}
