#include "gtest/gtest.h"

#include "geometry/geometry.h"
#include "constructions/constructions.h"

static constexpr double EPS = 1e-9;

TEST(ExtendedTest, PolygonBecomesInvalidWhenNonkonvex) {
    Point P1(0,0);
    Point P2(0,2);
    Point P3(2,2);
    Point P4(2,0);
    Polygon PG({&P1, &P2, &P3, &P4});

    EXPECT_TRUE(PG.isValid());

    // Tausche P1 und P4 -> selbstschneidendes, nichtkonvexes Polygon
    P1.moveTo(2, 0);
    P4.moveTo(0, 0);

    EXPECT_FALSE(PG.isValid());
}

TEST(ExtendedTest, MultipleObjectsMoving) {
    Point P1(0,0), P2(1,1);
    Point P3(0,0), P4(-1,1);
    Line L1(&P1, &P2), L2(&P3, &P4); // L1 = x, L2 = -x

    LineLineIntersection IL1L2(&L1, &L2);
    Circle C(IL1L2.first(), 2.0);
    LineCircleIntersection IL1C(&L1, &C);
    LineCircleIntersection IL2C(&L2, &C);

    // L1 ∩ L2 = (0,0)
    EXPECT_NEAR(IL1L2.first()->x(), 0.0, EPS);
    EXPECT_NEAR(IL1L2.first()->y(), 0.0, EPS);

    // L1 ∩ C = {(-√2,-√2), (√2,√2)}
    EXPECT_NEAR(IL1C.first()->x(), -M_SQRT2, EPS);
    EXPECT_NEAR(IL1C.first()->y(), -M_SQRT2, EPS);
    EXPECT_NEAR(IL1C.second()->x(), M_SQRT2, EPS);
    EXPECT_NEAR(IL1C.second()->y(), M_SQRT2, EPS);

    // L2 ∩ C = {(-√2,√2), (√2,-√2)}
    EXPECT_NEAR(IL2C.first()->x(), -M_SQRT2, EPS);
    EXPECT_NEAR(IL2C.first()->y(), M_SQRT2, EPS);
    EXPECT_NEAR(IL2C.second()->x(), M_SQRT2, EPS);
    EXPECT_NEAR(IL2C.second()->y(), -M_SQRT2, EPS);

    P1.moveTo(0,1);
    P2.moveTo(1,2);

    // L1 ∩ L2 = (-0.5,0.5)
    EXPECT_NEAR(IL1L2.first()->x(), -0.5, EPS);
    EXPECT_NEAR(IL1L2.first()->y(), 0.5, EPS);

    // C verschiebt sich mit
    EXPECT_NEAR(C.center()->x(), -0.5, EPS);
    EXPECT_NEAR(C.center()->y(), 0.5, EPS);
    EXPECT_NEAR(C.radius(), 2.0, EPS);

    // L1 ∩ C = {(-0.5-√2,0.5-√2), (-0.5+√2,0.5+√2)}
    EXPECT_NEAR(IL1C.first()->x(), -0.5-M_SQRT2, EPS);
    EXPECT_NEAR(IL1C.first()->y(), 0.5-M_SQRT2, EPS);
    EXPECT_NEAR(IL1C.second()->x(), -0.5+M_SQRT2, EPS);
    EXPECT_NEAR(IL1C.second()->y(), 0.5+M_SQRT2, EPS);

    // L2 ∩ C = {(-0.5-√2,0.5+√2), (-0.5+√2,0.5-√2)}
    EXPECT_NEAR(IL2C.first()->x(), -0.5-M_SQRT2, EPS);
    EXPECT_NEAR(IL2C.first()->y(), 0.5+M_SQRT2, EPS);
    EXPECT_NEAR(IL2C.second()->x(), -0.5+M_SQRT2, EPS);
    EXPECT_NEAR(IL2C.second()->y(), 0.5-M_SQRT2, EPS);
}