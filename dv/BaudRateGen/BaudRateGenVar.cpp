#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <VBaudRateGenVar.h>

constexpr unsigned ClockRate = 50 * 1000000;
constexpr unsigned BaudRate = 115200;
constexpr unsigned Oversample = 16;

constexpr unsigned rxRate = ClockRate / (2 * BaudRate * Oversample);
constexpr unsigned txRate = ClockRate / (2 * BaudRate);

static void reset(VBaudRateGenVar& rg, int phase = 0) {
  rg.phase = !!phase;
  rg.nReset = 1;
  rg.eval();
  rg.nReset = 0;
  rg.eval();
  rg.nReset = 1;
}

static void tick(VBaudRateGenVar& rg) {
  rg.clk = 0;
  rg.eval();
  rg.clk = 1;
  rg.eval();
}

TEST_CASE("BaudRateGenVar, Reset") {
  VBaudRateGenVar rg;
  reset(rg, 0);

  REQUIRE(rg.rxClk == 0);
  REQUIRE(rg.txClk == 0);

  reset(rg, 1);

  REQUIRE(rg.rxClk == 1);
  REQUIRE(rg.txClk == 1);
}

TEST_CASE("BaudRateGenVar, rxClk") {
  VBaudRateGenVar rg;

  for(std::uint16_t i {1}; i < (1 << 9); i <<= 1) {
    rg.count = i << 4;
    reset(rg);

    for(unsigned j {0}; j < i; ++j) {
      tick(rg);
      REQUIRE(rg.rxClk == 0);
    }

    for(unsigned j {0}; j <= i; ++j) {
      tick(rg);
      REQUIRE(rg.rxClk == 1);
    }

    tick(rg);
    REQUIRE(rg.rxClk == 0);
  }
}

TEST_CASE("BaudRateGenVar, txClk") {
  VBaudRateGenVar rg;

  for(std::uint16_t i {1}; i < (1 << 9); i <<= 1) {
    rg.count = i;
    reset(rg);

    for(unsigned j {0}; j < i; ++j) {
      tick(rg);
      REQUIRE(rg.txClk == 0);
    }

    for(unsigned j {0}; j <= i; ++j) {
      tick(rg);
      REQUIRE(rg.txClk == 1);
    }

    tick(rg);
    REQUIRE(rg.txClk == 0);
  }
}
