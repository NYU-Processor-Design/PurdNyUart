#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VBaudRateGenVar.h>

constexpr unsigned rxWidth = 16;
constexpr unsigned txWidth = 16;

constexpr unsigned ClockRate = 50 * 1000000;
constexpr unsigned BaudRate = 115200;
constexpr unsigned Oversample = 16;

constexpr unsigned rxRate = ClockRate / (BaudRate * Oversample);
constexpr unsigned txRate = ClockRate / BaudRate;
constexpr unsigned deltaRate = txRate % rxRate;

static void reset(VBaudRateGenVar& rg, int phase = 0) {
  rg.phase = !!phase;
  rg.syncReset = 0;
  nyu::reset(rg);
}

TEST_CASE("BaudRateGenVar, Reset") {
  VBaudRateGenVar& rg {nyu::getDUT<VBaudRateGenVar>()};
  rg.rxRate = 2;
  rg.txRate = 2;
  reset(rg, 0);

  REQUIRE(rg.rxClk == 0);
  REQUIRE(rg.txClk == 0);

  reset(rg, 1);

  REQUIRE(rg.rxClk == 1);
  REQUIRE(rg.txClk == 1);
}

TEST_CASE("BaudRateGenVar, phase") {
  VBaudRateGenVar& rg {nyu::getDUT<VBaudRateGenVar>()};
  rg.rxRate = 5;
  rg.txRate = 5;
  reset(rg);

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


TEST_CASE("BaudRateGenVar, rxClk") {
  VBaudRateGenVar& rg {nyu::getDUT<VBaudRateGenVar>()};

  for(unsigned i {2}; i < (1 << rxWidth); i <<= 1) {
    rg.rxRate = i;
    rg.txRate = i;
    reset(rg);

    for(unsigned j {0}; j < i - 1; ++j) {
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

TEST_CASE("BaudRateGenVar, txClk") {
  VBaudRateGenVar& rg {nyu::getDUT<VBaudRateGenVar>()};

  for(unsigned i {2}; i < (1 << txWidth); i <<= 1) {
    rg.txRate = i;
    reset(rg);
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

TEST_CASE("BaudRateGenVar, rx/tx sync") {
  VBaudRateGenVar& rg {nyu::getDUT<VBaudRateGenVar>()};
  rg.rxRate = rxRate;
  rg.txRate = txRate;
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
