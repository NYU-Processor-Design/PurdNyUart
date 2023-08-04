#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VBaudRateGen.h>

constexpr unsigned ClockRate = 50 * 1000000;
constexpr unsigned BaudRate = 115200;
constexpr unsigned Oversample = 16;

constexpr unsigned rxRate = ClockRate / (BaudRate * Oversample);
constexpr unsigned txRate = ClockRate / BaudRate;
constexpr unsigned deltaRate = txRate % rxRate;

static void reset(VBaudRateGen& rg, int phase = 0) {
  rg.phase = !!phase;
  nyu::reset(rg);
}

TEST_CASE("BaudRateGen, Reset") {
  VBaudRateGen& rg {nyu::getDUT<VBaudRateGen>()};
  reset(rg, 0);

  REQUIRE(rg.rxClk == 0);
  REQUIRE(rg.txClk == 0);

  reset(rg, 1);

  REQUIRE(rg.rxClk == 1);
  REQUIRE(rg.txClk == 1);
}

TEST_CASE("BaudRateGen, rxClk") {
  VBaudRateGen& rg {nyu::getDUT<VBaudRateGen>()};
  reset(rg);

  for(unsigned i {0}; i < rxRate - 1; ++i) {
    REQUIRE(rg.rxClk == 0);
    nyu::tick(rg);
  }

  REQUIRE(rg.rxClk == 1);
  nyu::tick(rg);

  for(unsigned i {0}; i < rxRate - 1; ++i) {
    REQUIRE(rg.rxClk == 0);
    nyu::tick(rg);
  }

  REQUIRE(rg.rxClk == 1);
}

TEST_CASE("BaudRateGen, txClk") {
  VBaudRateGen& rg {nyu::getDUT<VBaudRateGen>()};
  reset(rg);

  for(unsigned i {0}; i < txRate - 1; ++i) {
    REQUIRE(rg.txClk == 0);
    nyu::tick(rg);
  }

  REQUIRE(rg.txClk == 1);
  nyu::tick(rg);

  for(unsigned i {0}; i < txRate - 1; ++i) {
    REQUIRE(rg.txClk == 0);
    nyu::tick(rg);
  }

  REQUIRE(rg.txClk == 1);
}

TEST_CASE("BaudRateGen, rx/tx sync") {
  VBaudRateGen& rg {nyu::getDUT<VBaudRateGen>()};
  reset(rg);

  unsigned ticks {0};
  for(unsigned i {0}; i < txRate - 1 - deltaRate; ++i, ticks += rg.rxClk)
    nyu::tick(rg);

  REQUIRE(ticks == Oversample - 1);

  for(unsigned i {0}; i < deltaRate; ++i) {
    REQUIRE(rg.rxClk == 0);
    REQUIRE(rg.txClk == 0);
    nyu::tick(rg);
  }

  REQUIRE(rg.rxClk == 1);
  REQUIRE(rg.txClk == 1);
}
