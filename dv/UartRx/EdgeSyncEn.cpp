#include <catch2/catch_test_macros.hpp>
#include <NyuTestUtil.hpp>

#include <VEdgeSyncEn.h>

static void reset(VEdgeSyncEn& es, unsigned phase = 1) {
  es.phase = !!phase;
  es.en = 1;
  nyu::reset(es);
}

TEST_CASE("EdgeSyncEn, reset") {
  VEdgeSyncEn es;
  reset(es);

  es.in = 0;
  nyu::tick(es, 3);

  REQUIRE(es.out == 0);

  reset(es);

  REQUIRE(es.out == 1);
}

TEST_CASE("EdgeSyncEn, phase") {
  VEdgeSyncEn es;

  reset(es, 0);
  REQUIRE(es.out == 0);

  reset(es, 1);
  REQUIRE(es.out == 1);
}

TEST_CASE("EdgeSyncEn, enable") {
  VEdgeSyncEn es;

  reset(es);

  REQUIRE(es.out == 1);
  es.en = 0;

  es.in = 0;
  nyu::tick(es, 2);

  REQUIRE(es.out == 1);
}

TEST_CASE("EdgeSyncEn, input") {
  VEdgeSyncEn es;
  reset(es);

  es.in = 1;
  nyu::tick(es);
  es.in = 0;

  nyu::tick(es);
  REQUIRE(es.out == 1);

  nyu::tick(es);
  REQUIRE(es.out == 0);
}

TEST_CASE("EdgeSyncEn, rise/fall") {
  VEdgeSyncEn es;
  reset(es, 0);

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
