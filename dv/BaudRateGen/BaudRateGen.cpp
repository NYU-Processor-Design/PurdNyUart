#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VBaudRateGen.h>

constexpr unsigned ClockRate = 50 * 1000000;
constexpr unsigned BaudRate = 115200;
constexpr unsigned Oversample = 16;

constexpr unsigned rxRate = ClockRate / (BaudRate * Oversample) - 1;
constexpr unsigned txRate = ClockRate / BaudRate - 1;
constexpr unsigned deltaRate = txRate % rxRate;

static void reset(VBaudRateGen& rg, int phase = 0) {
  rg.phase = !!phase;
  nyu::reset(rg);
}

TEST_CASE("BaudRateGen, Reset") {
  VBaudRateGen rg;
  reset(rg, 0);

  REQUIRE(rg.rxClk == 0);
  REQUIRE(rg.txClk == 0);

  reset(rg, 1);

  REQUIRE(rg.rxClk == 1);
  REQUIRE(rg.txClk == 1);
}

TEST_CASE("BaudRateGen, rxClk") {
  VBaudRateGen rg;
  reset(rg);

  for(unsigned i {0}; i < rxRate; ++i) {
    nyu::tick(rg);
    REQUIRE(rg.rxClk == 0);
  }

  nyu::tick(rg);
  REQUIRE(rg.rxClk == 1);

  for(unsigned i {0}; i < rxRate; ++i) {
    nyu::tick(rg);
    REQUIRE(rg.rxClk == 0);
  }

  nyu::tick(rg);
  REQUIRE(rg.rxClk == 1);
}

TEST_CASE("BaudRateGen, txClk") {
  VBaudRateGen rg;
  reset(rg);

  for(unsigned i {0}; i < txRate; ++i) {
    nyu::tick(rg);
    REQUIRE(rg.txClk == 0);
  }

  nyu::tick(rg);
  REQUIRE(rg.txClk == 1);

  for(unsigned i {0}; i < txRate; ++i) {
    nyu::tick(rg);
    REQUIRE(rg.txClk == 0);
  }

  nyu::tick(rg);
  REQUIRE(rg.txClk == 1);
}

TEST_CASE("BaudRateGen, rx/tx sync") {
  VBaudRateGen rg;
  reset(rg);

  unsigned ticks {0};
  for(unsigned i {0}; i < txRate - deltaRate; ++i, ticks += rg.rxClk)
    nyu::tick(rg);

  REQUIRE(ticks == Oversample - 1);

  for(unsigned i {0}; i < deltaRate; ++i) {
    nyu::tick(rg);

    REQUIRE(rg.rxClk == 0);
    REQUIRE(rg.txClk == 0);
  }

  nyu::tick(rg);

  REQUIRE(rg.rxClk == 1);
  REQUIRE(rg.txClk == 1);
}
