#include <cstdint>
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VLoopback_tb.h>

static void send(VLoopback_tb& lb, std::uint8_t val) {
  lb.data_tx = val;
  lb.valid = 1;
  nyu::tick(lb);
  lb.valid = 0;
}

TEST_CASE("Loopback_tb") {
  VLoopback_tb lb;

  nyu::reset(lb);

  size_t i {0};


  send(lb, 0xAA);

  while(!lb.done_rx) {
    nyu::tick(lb);
    REQUIRE(lb.err == 0);
  }

  REQUIRE(lb.data_rx == 0xAA);
}
