#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VBaudRateGen4.h>

constexpr unsigned ClockRate = 50 * 1000000;
constexpr unsigned Oversample = 16;
constexpr unsigned Bauds[] = {9600, 38400, 57600, 115200};

static void reset(VBaudRateGen4& rg, int phase = 0) {
  rg.phase = !!phase;
  nyu::reset(rg);
}

static unsigned selRx(unsigned sel) {
  return ClockRate / (Bauds[sel] * Oversample);
}

static unsigned selTx(unsigned sel) {
  return ClockRate / Bauds[sel];
}

TEST_CASE("BaudRateGen4, Reset") {
  VBaudRateGen4& rg {nyu::getDUT<VBaudRateGen4>()};
  reset(rg, 0);

  REQUIRE(rg.rxClk == 0);
  REQUIRE(rg.txClk == 0);

  reset(rg, 1);

  REQUIRE(rg.rxClk == 1);
  REQUIRE(rg.txClk == 1);
}

TEST_CASE("BaudRateGen4, rxClk") {
  VBaudRateGen4& rg {nyu::getDUT<VBaudRateGen4>()};

  for(unsigned sel {0}; sel < 4; ++sel) {
    rg.sel = sel;
    reset(rg);

    unsigned rate {selRx(sel)};

    for(unsigned i {0}; i < rate - 1; ++i) {
      REQUIRE(rg.rxClk == 0);
      nyu::tick(rg);
    }

    REQUIRE(rg.rxClk == 1);
    nyu::tick(rg);

    for(unsigned i {0}; i < rate - 1; ++i) {
      REQUIRE(rg.rxClk == 0);
      nyu::tick(rg);
    }

    REQUIRE(rg.rxClk == 1);
  }
}

TEST_CASE("BaudRateGen4, txClk") {
  VBaudRateGen4& rg {nyu::getDUT<VBaudRateGen4>()};

  for(unsigned sel {0}; sel < 4; ++sel) {
    rg.sel = sel;
    reset(rg);

    unsigned rate {selTx(sel)};

    for(unsigned i {0}; i < rate - 1; ++i) {
      REQUIRE(rg.txClk == 0);
      nyu::tick(rg);
    }

    REQUIRE(rg.txClk == 1);
    nyu::tick(rg);

    for(unsigned i {0}; i < rate - 1; ++i) {
      REQUIRE(rg.txClk == 0);
      nyu::tick(rg);
    }

    nyu::tick(rg);
  }
}

TEST_CASE("BaudRateGen4, rx/tx sync") {
  VBaudRateGen4& rg {nyu::getDUT<VBaudRateGen4>()};

  for(unsigned sel {0}; sel < 4; ++sel) {
    rg.sel = sel;
    reset(rg);

    unsigned ticks {0};
    unsigned txRate {selTx(sel)};
    unsigned deltaRate {txRate % selRx(sel)};

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
}
