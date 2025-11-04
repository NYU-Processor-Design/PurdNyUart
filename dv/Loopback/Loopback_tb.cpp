#include <array>
#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <NyuCatch2TestUtil.hpp>

#include <VLoopback_tb.h>

static void send(auto& lb, std::uint8_t val) {
  lb.data_tx = val;
  lb.valid = 1;
  nyu::tick(lb);
  lb.valid = 0;
}

TEST_CASE("Loopback_tb") {
  auto& lb {nyu::get_dut_catch2<VLoopback_tb>()};

  nyu::reset(lb);

  constexpr std::array<std::uint8_t, 9> seq {0, 1, 2, 3, 4, 5, 0xAA, 0x55,
      0xFF};

  size_t i {0};
  size_t j {0};

  send(lb, seq[i++]);

  while(i < seq.size()) {
    if(lb.done_rx)
      REQUIRE(lb.data_rx == seq[j++]);

    if(lb.done_tx)
      send(lb, seq[i++]);
    else
      nyu::tick(lb);
  }

  while(j < seq.size()) {
    if(lb.done_rx)
      REQUIRE(lb.data_rx == seq[j++]);
    nyu::tick(lb);
  }
}
