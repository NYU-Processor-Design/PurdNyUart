#include <catch2/catch_test_macros.hpp>
#include <VEdgeSync.h>

static void tick(VEdgeSync& es, unsigned cycles = 1) {
  for(unsigned i {0}; i < cycles; ++i) {
    es.clk = 0;
    es.eval();
    es.clk = 1;
    es.eval();
  }
}

static void reset(VEdgeSync& es) {
  es.in = 0;
  tick(es, 3);
}

TEST_CASE("EdgeSync, input") {
  VEdgeSync es;
  reset(es);

  es.in = 1;
  tick(es);
  es.in = 0;

  tick(es);
  REQUIRE(es.out == 1);

  tick(es);
  REQUIRE(es.out == 0);
}

TEST_CASE("EdgeSync, rise/fall") {
  VEdgeSync es;
  reset(es);

  es.in = 1;
  tick(es);
  es.in = 0;

  REQUIRE(es.rise == 0);
  REQUIRE(es.fall == 0);

  tick(es);
  REQUIRE(es.rise == 1);
  REQUIRE(es.fall == 0);

  tick(es);
  REQUIRE(es.rise == 0);
  REQUIRE(es.fall == 1);

  tick(es);
  REQUIRE(es.rise == 0);
  REQUIRE(es.fall == 0);
}
