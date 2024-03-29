#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VUartTx.h>

static void send(VUartTx& tx, std::uint8_t val) {
  tx.data = val;
  tx.valid = 1;
  nyu::tick(tx);
  tx.valid = 0;
}


TEST_CASE("UartTx, reset") {
  auto& tx {nyu::getDUT<VUartTx>()};

  nyu::reset(tx);

  tx.valid = 1;
  nyu::tick(tx);

  REQUIRE(tx.busy == 1);

  nyu::reset(tx);

  REQUIRE(tx.out == 1);
  REQUIRE(tx.busy == 0);
}

TEST_CASE("UartTx, busy/done") {
  auto& tx {nyu::getDUT<VUartTx>()};

  nyu::reset(tx);

  send(tx, 0xFF);

  for(unsigned i {0}; i < 8; ++i) {
    nyu::tick(tx);
    REQUIRE(tx.busy == 1);
    REQUIRE(tx.done == 0);
  }

  nyu::tick(tx);
  REQUIRE(tx.done == 1);
  nyu::tick(tx);
  REQUIRE(tx.done == 0);
  nyu::tick(tx);
  REQUIRE(tx.busy == 0);
}

TEST_CASE("UartTx, data") {
  auto& tx {nyu::getDUT<VUartTx>()};

  nyu::reset(tx);

  send(tx, 0xAA);
  REQUIRE(tx.out == 0);

  std::uint8_t data {0xAA};
  for(unsigned i {0}; i < 8; ++i) {
    nyu::tick(tx);
    REQUIRE(tx.out == (data & 0x1));
    data >>= 1;
  }

  nyu::tick(tx);
  REQUIRE(tx.out == 1);
  REQUIRE(tx.busy == 1);

  send(tx, 0x55);
  REQUIRE(tx.out == 0);

  data = 0x55;
  for(unsigned i {0}; i < 8; ++i) {
    nyu::tick(tx);
    REQUIRE(tx.out == (data & 0x1));
    data >>= 1;
  }

  nyu::tick(tx);
  REQUIRE(tx.out == 1);
}
