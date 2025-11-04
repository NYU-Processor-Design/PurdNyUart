#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <NyuCatch2TestUtil.hpp>

#include <TestHelpers.hpp>
#include <VBaudRateGen.h>

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

template <typename Dut, typename Phase = int, typename Rate = decltype(txRate),
    typename SyncReset = int>
requires can_phase_reset<Dut, Phase, Rate, SyncReset> &&
    nyu::reset_default_ok<Dut>
void tag_invoke(nyu::reset_t, Dut& dut, Phase phase = 0,
    Rate rate =
        txRate) noexcept(nothrow_can_phase_reset<Dut, Phase, Rate, SyncReset> &&
    nyu::nothrow_reset_default_ok<Dut>) {
  dut.phase = static_cast<bool>(phase);
  dut.rate = rate;
  dut.syncReset = SyncReset {0};
  nyu::reset_default(dut);
}

TEST_CASE("BaudRateGen, Reset") {
  auto& rg {nyu::get_dut_catch2<VBaudRateGen>()};
  nyu::reset(rg, 0);

  REQUIRE(rg.rxClk == 0);
  REQUIRE(rg.txClk == 0);

  nyu::reset(rg, 1);

  REQUIRE(rg.rxClk == 1);
  REQUIRE(rg.txClk == 1);
}

TEST_CASE("BaudRateGen, phase") {
  auto& rg {nyu::get_dut_catch2<VBaudRateGen>()};
  nyu::reset(rg);

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


TEST_CASE("BaudRateGen, rxClk") {
  auto& rg {nyu::get_dut_catch2<VBaudRateGen>()};

  for(unsigned i {2}; i < (1 << rxWidth); i <<= 1) {
    nyu::reset(rg, 0, i << rxShift);
    unsigned offset {i - ((i >> 1) + 1)};

    for(unsigned j {offset}; j < i - 1; ++j) {
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

TEST_CASE("BaudRateGen, txClk") {
  auto& rg {nyu::get_dut_catch2<VBaudRateGen>()};

  for(unsigned i {2}; i < (1 << txWidth); i <<= 1) {
    nyu::reset(rg, 0, i);

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

TEST_CASE("BaudRateGen, rx/tx sync") {
  auto& rg {nyu::get_dut_catch2<VBaudRateGen>()};

  for(unsigned rate {2 << 4}; rate < 500; rate += 1) {
    nyu::reset(rg, 0, rate);
    for(unsigned i {0}; i < 5; ++i) {
      unsigned ticks {rg.rxClk};
      for(; rg.txClk != 1; ticks += rg.rxClk)
        nyu::tick(rg);

      REQUIRE(ticks == Oversample);

      nyu::tick(rg);
    }
  }
}
