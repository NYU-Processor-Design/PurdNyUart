#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <NyuTestUtil.hpp>

#include <VUartRx.h>

constexpr unsigned Oversample = 16;

TEST_CASE("UartRx, reset") {
  VUartRx rx;

  nyu::reset(rx);

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

  nyu::reset(rx);

  REQUIRE(rx.data == 0x00);
}
