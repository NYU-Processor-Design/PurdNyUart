#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VBaudRateGen.h>

constexpr unsigned ClockRate = 50 * 1000000;
constexpr unsigned BaudRate = 115200;
constexpr unsigned Oversample = 16;

constexpr unsigned rxRate = ClockRate / (2 * BaudRate * Oversample);
constexpr unsigned txRate = ClockRate / (2 * BaudRate);
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

  for(unsigned i {0}; i <= rxRate; ++i) {
    nyu::tick(rg);
    REQUIRE(rg.rxClk == 1);
  }

  nyu::tick(rg);
  REQUIRE(rg.rxClk == 0);
}

TEST_CASE("BaudRateGen, txClk") {
  VBaudRateGen rg;
  reset(rg);

  for(unsigned i {0}; i < txRate; ++i) {
    nyu::tick(rg);
    REQUIRE(rg.txClk == 0);
  }

  for(unsigned i {0}; i <= txRate; ++i) {
    nyu::tick(rg);
    REQUIRE(rg.txClk == 1);
  }

  nyu::tick(rg);
  REQUIRE(rg.txClk == 0);
}

TEST_CASE("BaudRateGen, rx/tx sync") {
  VBaudRateGen rg;
  reset(rg);

  for(unsigned i {0}; i < txRate - deltaRate; ++i)
    nyu::tick(rg);

  unsigned rxClkStatus = rg.rxClk;
  REQUIRE(rg.txClk == 0);


  for(unsigned i {0}; i < deltaRate; ++i) {
    nyu::tick(rg);

    REQUIRE(rg.rxClk == rxClkStatus);
    REQUIRE(rg.txClk == 0);
  }

  nyu::tick(rg);

  REQUIRE(rg.rxClk == !rxClkStatus);
  REQUIRE(rg.txClk == 1);
}