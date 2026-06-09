#include "geometry/Line.h"
#include "constructions/LineCircleIntersection.h"
#include "constructions/LineLineIntersection.h"
#include "constructions/Midpoint.h"
#include "geometry/Circle.h"
#include "geometry/Point.h"
#include <gtest/gtest.h>

static constexpr double EPS = 1e-9;

TEST(LineLineIntersectionTest, BasicIntersection) {
    // x-Achse ∩ y-Achse = (0,0)
    Point p1(-1,0);
    Point p2(1,0);
    Point p3(0,-1);
    Point p4(0,1);
    Line L1({.point1=&p1,.point2=&p2});
    Line L2({.point1=&p3,.point2=&p4});
    LineLineIntersection S(&L1,&L2);

    EXPECT_TRUE (S.first()->isValid());
    EXPECT_FALSE(S.second()->isValid());
    EXPECT_NEAR(S.first()->x(), 0.0, EPS);
    EXPECT_NEAR(S.first()->y(), 0.0, EPS);
}

TEST(LineLineIntersectionTest, DiagonalLines) {
    // y=x und y=-x+2 → (1,1)
    Point p1(0,0);
    Point p2(2,2);
    Point p3(0,2);
    Point p4(2,0);
    Line L1({.point1=&p1,.point2=&p2});
    Line L2({.point1=&p3,.point2=&p4});
    LineLineIntersection S(&L1,&L2);

    EXPECT_NEAR(S.first()->x(), 1.0, EPS);
    EXPECT_NEAR(S.first()->y(), 1.0, EPS);
}

TEST(LineLineIntersectionTest, ParallelLines) {
    Point p1(0,0);
    Point p2(1,0);
    Point p3(0,1);
    Point p4(1,1);
    Line L1({.point1=&p1,.point2=&p2});
    Line L2({.point1=&p3,.point2=&p4});
    LineLineIntersection S(&L1,&L2);

    EXPECT_FALSE(S.first()->isValid());
    EXPECT_FALSE(S.second()->isValid());
}

TEST(LineLineIntersectionTest, UpdatesOnMove) {
    Point p1(0,0);
    Point p2(4,0);
    Point p3(2,-1);
    Point p4(2,1);
    Line L1({.point1=&p1,.point2=&p2});
    Line L2({.point1=&p3,.point2=&p4});
    LineLineIntersection S(&L1,&L2);
    EXPECT_NEAR(S.first()->x(), 2.0, EPS);

    p3.moveTo(3,-1); p4.moveTo(3,1);
    EXPECT_NEAR(S.first()->x(), 3.0, EPS);
}

TEST(LineLineIntersectionTest, BecomesInvalidWhenParallel) {
    Point p1(0,0);
    Point p2(1,0);
    Point p3(0,1);
    Point p4(1,2);
    Line L1({.point1=&p1,.point2=&p2});
    Line L2({.point1=&p3,.point2=&p4});
    LineLineIntersection S(&L1,&L2);
    EXPECT_TRUE(S.first()->isValid());

    p4.moveTo(1,1); // L2 jetzt auch horizontal
    EXPECT_FALSE(S.first()->isValid());
}

TEST(LineLineIntersectionTest, UniformAPIWithLineCircle) {
    // Beide Typen liefern Point* über first() – uniform verwendbar
    Point p1(-2,0);
    Point p2(2,0);
    Point p3(0,-1);
    Point p4(0,1);
    Line L1({.point1=&p1,.point2=&p2});
    Line L2({.point1=&p3,.point2=&p4});
    LineLineIntersection S1(&L1,&L2);

    Point center(0,0);
    Point rp(2,0);
    Circle C({.center=&center,.radiusPoint=&rp});
    LineCircleIntersection S2(&L1,&C);

    // Beide first() sind Point* – Midpoint akzeptiert beide
    Midpoint M(S1.first(), S2.first());
    EXPECT_TRUE(M.isValid());
}