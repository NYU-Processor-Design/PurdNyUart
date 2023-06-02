#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VEdgeSync.h>

TEST_CASE("EdgeSync, reset") {
  VEdgeSync es;
  nyu::reset(es);

  es.in = 1;
  nyu::tick(es, 3);

  REQUIRE(es.out == 1);

  nyu::reset(es);

  REQUIRE(es.out == 0);
}

TEST_CASE("EdgeSync, input") {
  VEdgeSync es;
  nyu::reset(es);

  es.in = 1;
  nyu::tick(es);
  es.in = 0;

  nyu::tick(es);
  REQUIRE(es.out == 1);

  nyu::tick(es);
  REQUIRE(es.out == 0);
}

TEST_CASE("EdgeSync, rise/fall") {
  VEdgeSync es;
  nyu::reset(es);

  es.in = 1;
  nyu::tick(es);
  es.in = 0;

  REQUIRE(es.rise == 0);
  REQUIRE(es.fall == 0);

  nyu::tick(es);
  REQUIRE(es.rise == 1);
  REQUIRE(es.fall == 0);

  nyu::tick(es);
  REQUIRE(es.rise == 0);
  REQUIRE(es.fall == 1);

  nyu::tick(es);
  REQUIRE(es.rise == 0);
  REQUIRE(es.fall == 0);
}
