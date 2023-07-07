#include <array>
#include <cstdint>

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

  constexpr std::array<std::uint8_t, 9> seq {0, 1, 2, 3, 4, 5, 0xAA, 0x55,
      0xFF};

  for(auto val : seq) {
    while(lb.busy)
      nyu::tick(lb);

    send(lb, val);
    while(!lb.done_rx)
      nyu::tick(lb);

    REQUIRE(lb.data_rx == val);
  }
}
