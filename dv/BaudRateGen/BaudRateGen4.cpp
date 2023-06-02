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
  return ClockRate / (2 * Bauds[sel] * Oversample);
}

static unsigned selTx(unsigned sel) {
  return ClockRate / (2 * Bauds[sel]);
}

TEST_CASE("BaudRateGen4, Reset") {
  VBaudRateGen4 rg;
  reset(rg, 0);

  REQUIRE(rg.rxClk == 0);
  REQUIRE(rg.txClk == 0);

  reset(rg, 1);

  REQUIRE(rg.rxClk == 1);
  REQUIRE(rg.txClk == 1);
}

TEST_CASE("BaudRateGen4, rxClk") {
  VBaudRateGen4 rg;

  for(unsigned sel {0}; sel < 4; ++sel) {
    rg.sel = sel;
    reset(rg);

    unsigned rate {selRx(sel)};

    for(unsigned i {0}; i < rate; ++i) {
      nyu::tick(rg);
      REQUIRE(rg.rxClk == 0);
    }

    for(unsigned i {0}; i <= rate; ++i) {
      nyu::tick(rg);
      REQUIRE(rg.rxClk == 1);
    }

    nyu::tick(rg);
    REQUIRE(rg.rxClk == 0);
  }
}

TEST_CASE("BaudRateGen4, txClk") {
  VBaudRateGen4 rg;
  reset(rg);

  for(unsigned sel {0}; sel < 4; ++sel) {
    rg.sel = sel;
    reset(rg);

    unsigned rate {selTx(sel)};

    for(unsigned i {0}; i < rate; ++i) {
      nyu::tick(rg);
      REQUIRE(rg.txClk == 0);
    }

    for(unsigned i {0}; i <= rate; ++i) {
      nyu::tick(rg);
      REQUIRE(rg.txClk == 1);
    }

    nyu::tick(rg);
    REQUIRE(rg.txClk == 0);
  }
}

TEST_CASE("BaudRateGen4, rx/tx sync") {
  VBaudRateGen4 rg;

  for(unsigned sel {0}; sel < 4; ++sel) {
    rg.sel = sel;
    reset(rg);

    unsigned txRate {selTx(sel)};
    unsigned deltaRate {txRate % selRx(sel)};

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
}
