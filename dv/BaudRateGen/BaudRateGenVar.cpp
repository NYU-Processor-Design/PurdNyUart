#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VBaudRateGenVar.h>

static constexpr unsigned flog2(unsigned x) {
  return x == 1 ? 0 : flog2(x >> 1) + 1;
}

static constexpr unsigned clog2(unsigned x) {
  return x == 1 ? 0 : flog2(x - 1) + 1;
}

constexpr unsigned MaxClockRate = 100 * 1000000;
constexpr unsigned MinBaudRate = 9600;
constexpr unsigned Oversample = 16;

constexpr unsigned rxRate = MaxClockRate / (MinBaudRate * Oversample);
constexpr unsigned txRate = MaxClockRate / MinBaudRate;

constexpr unsigned txWidth = clog2(txRate);
constexpr unsigned rxShift = clog2(Oversample);
constexpr unsigned rxWidth = txWidth - rxShift;

static void reset(VBaudRateGenVar& rg, int phase = 0, unsigned rate = txRate) {
  rg.phase = !!phase;
  rg.rate = rate;
  rg.syncReset = 0;
  nyu::reset(rg);
}

TEST_CASE("BaudRateGenVar, Reset") {
  VBaudRateGenVar& rg {nyu::getDUT<VBaudRateGenVar>()};
  reset(rg, 0);

  REQUIRE(rg.rxClk == 0);
  REQUIRE(rg.txClk == 0);

  reset(rg, 1);

  REQUIRE(rg.rxClk == 1);
  REQUIRE(rg.txClk == 1);
}

TEST_CASE("BaudRateGenVar, phase") {
  VBaudRateGenVar& rg {nyu::getDUT<VBaudRateGenVar>()};
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
    reset(rg, 0, i << rxShift);

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
    reset(rg, 0, i);

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

  for(unsigned rate {16}; rate < 100; rate += 5) {
    reset(rg, 0, rate);
    for(unsigned i {0}; i < 5; ++i) {
      unsigned ticks {rg.rxClk};
      for(; rg.txClk != 1; ticks += rg.rxClk)
        nyu::tick(rg);

      REQUIRE(ticks == Oversample);

      nyu::tick(rg);
    }
  }
}
