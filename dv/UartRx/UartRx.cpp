#include <catch2/catch_test_macros.hpp>
#include <NyuCatch2TestUtil.hpp>

#include <TestHelpers.hpp>
#include <VSyncUartRx.h>

using VUartRx = VSyncUartRx;

TEST_CASE("UartRx, reset") {
  auto& rx {nyu::get_dut_catch2<VUartRx>()};

  nyu::reset(rx);

  start_transmit(rx, 0xFF);

  nyu::tick(rx);

  REQUIRE(rx.data == 0xFF);

  nyu::reset(rx);

  REQUIRE(rx.data == 0x00);
}

TEST_CASE("UartRx, done") {
  auto& rx {nyu::get_dut_catch2<VUartRx>()};

  nyu::reset(rx);

  start_transmit(rx, 0);
  rx.in = 1;

  nyu::tick(rx);
  REQUIRE(rx.done == 1);
  nyu::tick(rx);
  REQUIRE(rx.done == 0);
}

TEST_CASE("UartRx, resync") {
  auto& rx {nyu::get_dut_catch2<VUartRx>()};

  nyu::reset(rx);

  start(rx, 14);
  transmit(rx, 0xAA, 13);

  nyu::tick(rx, 4);
  REQUIRE(rx.data == 0xAA);
  REQUIRE(rx.done == 1);
  nyu::tick(rx, 9);

  start(rx);
  transmit(rx, 0x55, 13);

  rx.in = 1;
  nyu::tick(rx);
  REQUIRE(rx.data == 0x55);
  REQUIRE(rx.done == 1);
}

void check_data_error(auto& rx) {
  rx.in = 1;
  nyu::tick(rx);
  rx.in = 0;
  nyu::tick(rx);
  rx.in = 1;

  REQUIRE(rx.err == 1);
  nyu::tick(rx);
  REQUIRE(rx.err == 0);
}

TEST_CASE("UartRx, error") {
  auto& rx {nyu::get_dut_catch2<VUartRx>()};

  nyu::reset(rx);

  start(rx, 1);

  rx.in = 1;
  nyu::tick(rx);

  REQUIRE(rx.err == 1);

  start(rx);
  check_data_error(rx);

  start(rx);
  nyu::tick(rx, 9);
  check_data_error(rx);

  start_transmit(rx, 0);
  nyu::tick(rx, 9);
  REQUIRE(rx.err == 1);
}

TEST_CASE("UartRx, idle") {
  auto& rx {nyu::get_dut_catch2<VUartRx>()};
  nyu::reset(rx);

  start_transmit(rx, 0xAA);
  nyu::tick(rx, Oversample + 1);
  REQUIRE(rx.data == 0xAA);

  start_transmit(rx, 0x55);
  rx.in = 1;
  nyu::tick(rx, Oversample + 1);
  REQUIRE(rx.data == 0x55);
}
