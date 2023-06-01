#include <catch2/catch_test_macros.hpp>
#include <VBaudRateGen.h>

constexpr unsigned ClockRate = 50 * 1000000;
constexpr unsigned BaudRate = 115200;
constexpr unsigned Oversample = 16;

constexpr unsigned rxRate = ClockRate / (2 * BaudRate * Oversample);
constexpr unsigned txRate = ClockRate / (2 * BaudRate);

static void reset(VBaudRateGen& rg, int phase = 0) {
  rg.phase = !!phase;
  rg.nReset = 1;
  rg.eval();
  rg.nReset = 0;
  rg.eval();
  rg.nReset = 1;
}

static void tick(VBaudRateGen& rg) {
  rg.clk = 0;
  rg.eval();
  rg.clk = 1;
  rg.eval();
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
    tick(rg);
    REQUIRE(rg.rxClk == 0);
  }

  for(unsigned i {0}; i <= rxRate; ++i) {
    tick(rg);
    REQUIRE(rg.rxClk == 1);
  }

  tick(rg);
  REQUIRE(rg.rxClk == 0);
}

TEST_CASE("BaudRateGen, txClk") {
  VBaudRateGen rg;
  reset(rg);

  for(unsigned i {0}; i < txRate; ++i) {
    tick(rg);
    REQUIRE(rg.txClk == 0);
  }

  for(unsigned i {0}; i <= txRate; ++i) {
    tick(rg);
    REQUIRE(rg.txClk == 1);
  }

  tick(rg);
  REQUIRE(rg.txClk == 0);
}
