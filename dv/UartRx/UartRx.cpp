#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VUartRx.h>

constexpr unsigned Oversample = 16;

static void start(VUartRx& rx, unsigned ticks = Oversample + 1) {
  rx.in = 1;
  nyu::tick(rx);
  rx.in = 0;
  nyu::tick(rx, ticks);
}

static void transmit(VUartRx& rx, std::uint8_t val,
    unsigned ticks = Oversample) {
  for(unsigned i {0}; i < 8; ++i) {
    rx.in = val & 0x1;
    nyu::tick(rx, ticks);
    val >>= 1;
  }
}

static void start_transmit(VUartRx& rx, std::uint8_t val) {
  start(rx);
  transmit(rx, val);
}

TEST_CASE("UartRx, reset") {
  VUartRx rx;

  nyu::reset(rx);

  start_transmit(rx, 0xFF);

  nyu::tick(rx);

  REQUIRE(rx.data == 0xFF);

  nyu::reset(rx);

  REQUIRE(rx.data == 0x00);
}

TEST_CASE("UartRx, done") {
  VUartRx rx;

  nyu::reset(rx);

  start_transmit(rx, 0);
  rx.in = 1;

  nyu::tick(rx);
  REQUIRE(rx.done == 1);
  nyu::tick(rx);
  REQUIRE(rx.done == 0);
}

TEST_CASE("UartRx, resync") {
  VUartRx rx;

  nyu::reset(rx);

  start(rx, 14);
  transmit(rx, 0xAA, 13);

  nyu::tick(rx, 5);
  REQUIRE(rx.data == 0xAA);
  REQUIRE(rx.done == 1);
  nyu::tick(rx, 9);

  start(rx);
  transmit(rx, 0x55, 13);

  rx.in = 1;
  nyu::tick(rx, 2);
  REQUIRE(rx.data == 0x55);
  REQUIRE(rx.done == 1);
}

TEST_CASE("UartRx, error") {
  VUartRx rx;

  nyu::reset(rx);

  start(rx, 1);

  rx.in = 1;
  nyu::tick(rx, 2);

  REQUIRE(rx.err == 1);

  start(rx);

  rx.in = 1;
  nyu::tick(rx);
  rx.in = 0;
  nyu::tick(rx);
  rx.in = 1;

  nyu::tick(rx);
  REQUIRE(rx.err == 1);
  nyu::tick(rx);
  REQUIRE(rx.err == 0);

  start_transmit(rx, 0);
  nyu::tick(rx, 9);
  REQUIRE(rx.err == 1);
}
