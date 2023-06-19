#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VUartRxEn.h>

constexpr unsigned Oversample = 16;

static void reset(VUartRxEn& rx) {
  rx.en = 1;
  nyu::reset(rx);
}

TEST_CASE("UartRxEn, reset") {
  VUartRxEn rx;

  reset(rx);

  rx.in = 1;
  nyu::tick(rx);

  rx.in = 0;
  nyu::tick(rx, Oversample);
  REQUIRE(rx.err == 0);

  rx.in = 1;
  while(!rx.done) {
    nyu::tick(rx);
    REQUIRE(rx.err == 0);
  }

  REQUIRE(rx.data == 0xFF);

  reset(rx);

  REQUIRE(rx.data == 0x00);
}
