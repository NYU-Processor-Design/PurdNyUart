#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <VBaudRateGen4.h>

constexpr unsigned ClockRate = 50 * 1000000;
constexpr unsigned Oversample = 16;
constexpr unsigned Bauds[] = {9600, 38400, 57600, 115200};

static void reset(VBaudRateGen4& rg, int phase = 0) {
  rg.phase = !!phase;
  rg.nReset = 1;
  rg.eval();
  rg.nReset = 0;
  rg.eval();
  rg.nReset = 1;
}

static void tick(VBaudRateGen4& rg) {
  rg.clk = 0;
  rg.eval();
  rg.clk = 1;
  rg.eval();
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

    unsigned rate {ClockRate / (2 * Bauds[sel] * Oversample)};

    for(unsigned i {0}; i < rate; ++i) {
      tick(rg);
      REQUIRE(rg.rxClk == 0);
    }

    for(unsigned i {0}; i <= rate; ++i) {
      tick(rg);
      REQUIRE(rg.rxClk == 1);
    }

    tick(rg);
    REQUIRE(rg.rxClk == 0);
  }
}

TEST_CASE("BaudRateGen4, txClk") {
  VBaudRateGen4 rg;
  reset(rg);

  for(unsigned sel {0}; sel < 4; ++sel) {
    rg.sel = sel;
    reset(rg);

    unsigned rate {ClockRate / (2 * Bauds[sel])};

    for(unsigned i {0}; i < rate; ++i) {
      tick(rg);
      REQUIRE(rg.txClk == 0);
    }

    for(unsigned i {0}; i <= rate; ++i) {
      tick(rg);
      REQUIRE(rg.txClk == 1);
    }

    tick(rg);
    REQUIRE(rg.txClk == 0);
  }
}
