#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VBaudRateGenVar.h>

static constexpr unsigned flog2(unsigned x) {
  return x == 1 ? 0 : 1 + flog2(x >> 1);
}

static constexpr unsigned clog2(unsigned x) {
  return x == 1 ? 0 : flog2(x - 1) + 1;
}

constexpr unsigned MaxClockRate = 100 * 1000000;
constexpr unsigned MinBaudRate = 9600;
constexpr unsigned Oversample = 16;

constexpr unsigned txWidth = clog2(MaxClockRate / (2 * MinBaudRate));
constexpr unsigned rxShift = clog2(Oversample);
constexpr unsigned rxWidth = txWidth - rxShift;

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

  for(std::uint16_t i {1}; i < (1 << rxWidth); i <<= 1) {
    rg.count = i << rxShift;
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

  for(std::uint16_t i {1}; i < (1 << txWidth); i <<= 1) {
    rg.count = i;
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

  for(std::uint16_t i {1 << rxShift}; i < (1 << txWidth); i <<= 1) {
    rg.count = i;
    reset(rg);

    unsigned deltaRate {i % (unsigned) (i >> rxShift)};

    for(unsigned j {0}; j < i - deltaRate; ++j)
      nyu::tick(rg);

    for(unsigned j {0}; j < deltaRate; ++j) {
      nyu::tick(rg);

      REQUIRE(rg.rxClk == 0);
      REQUIRE(rg.txClk == 0);
    }

    nyu::tick(rg);

    REQUIRE(rg.rxClk == 1);
    REQUIRE(rg.txClk == 1);
  }
}
