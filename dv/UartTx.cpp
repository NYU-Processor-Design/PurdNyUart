#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VUartTx.h>

TEST_CASE("UartTx, reset") {
  VUartTx tx;

  nyu::reset(tx);

  tx.data = 0x00;
  tx.valid = 1;
  nyu::tick(tx);

  tx.valid = 0;

  while(tx.out != 0)
    nyu::tick(tx);

  REQUIRE(tx.busy == 1);

  nyu::reset(tx);

  REQUIRE(tx.out == 1);
  REQUIRE(tx.busy == 0);
}
