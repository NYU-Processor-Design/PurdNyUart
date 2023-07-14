#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VBaudRateGenVar.h>

constexpr unsigned rxWidth = 16;
constexpr unsigned txWidth = 16;

constexpr unsigned ClockRate = 50 * 1000000;
constexpr unsigned BaudRate = 115200;
constexpr unsigned Oversample = 16;

constexpr unsigned rxRate = ClockRate / (BaudRate * Oversample) - 1;
constexpr unsigned txRate = ClockRate / BaudRate - 1;
constexpr unsigned deltaRate = txRate % rxRate;

static void reset(VBaudRateGenVar& rg, int phase = 0) {
  rg.phase = !!phase;
  nyu::reset(rg);
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

  for(unsigned i {1}; i < (1 << rxWidth); i <<= 1) {
    rg.rxRate = i;
    rg.txRate = i;
    reset(rg);

    for(unsigned j {0}; j < i; ++j) {
      nyu::tick(rg);
      REQUIRE(rg.rxClk == 0);
    }

    nyu::tick(rg);
    REQUIRE(rg.rxClk == 1);

    for(unsigned j {0}; j < i; ++j) {
      nyu::tick(rg);
      REQUIRE(rg.rxClk == 0);
    }

    nyu::tick(rg);
    REQUIRE(rg.rxClk == 1);
  }
}

TEST_CASE("BaudRateGenVar, txClk") {
  VBaudRateGenVar rg;

  for(unsigned i {1}; i < (1 << txWidth); i <<= 1) {
    rg.txRate = i;
    reset(rg);
    for(unsigned j {0}; j < i; ++j) {
      nyu::tick(rg);
      REQUIRE(rg.txClk == 0);
    }

    nyu::tick(rg);
    REQUIRE(rg.txClk == 1);

    for(unsigned j {0}; j < i; ++j) {
      nyu::tick(rg);
      REQUIRE(rg.txClk == 0);
    }

    nyu::tick(rg);
    REQUIRE(rg.txClk == 1);
  }
}

TEST_CASE("BaudRateGenVar, rx/tx sync") {
  VBaudRateGenVar rg;
  rg.rxRate = rxRate;
  rg.txRate = txRate;
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
