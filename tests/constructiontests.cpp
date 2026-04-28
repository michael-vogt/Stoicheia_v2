#include <gtest/gtest.h>
#include <cmath>
#include "geometry/geometry.h"
#include "constructions/constructions.h"

static constexpr double EPS = 1e-9;

// ════════════════════════════════════════════════════════════════════════════
// Intersection
// ════════════════════════════════════════════════════════════════════════════

/*TEST(IntersectionTest, BasicIntersection) {
    // x-Achse und y-Achse schneiden sich in (0,0)
    Point p1(-1, 0), p2(1, 0);
    Point p3(0, -1), p4(0, 1);
    Line xAxis(&p1, &p2);
    Line yAxis(&p3, &p4);
    IntersectionPoint S(&xAxis, &yAxis);

    EXPECT_TRUE(S.isValid());
    EXPECT_NEAR(S.x(), 0.0, EPS);
    EXPECT_NEAR(S.y(), 0.0, EPS);
}

TEST(IntersectionTest, DiagonalLines) {
    // y=x und y=-x+2 → Schnittpunkt (1,1)
    Point p1(0,0), p2(2,2);
    Point p3(0,2), p4(2,0);
    Line L1(&p1, &p2);
    Line L2(&p3, &p4);
    IntersectionPoint S(&L1, &L2);

    EXPECT_TRUE(S.isValid());
    EXPECT_NEAR(S.x(), 1.0, EPS);
    EXPECT_NEAR(S.y(), 1.0, EPS);
}

TEST(IntersectionTest, ParallelLinesAreInvalid) {
    Point p1(0,0), p2(1,0);
    Point p3(0,1), p4(1,1);
    Line L1(&p1, &p2);
    Line L2(&p3, &p4);
    IntersectionPoint S(&L1, &L2);

    EXPECT_FALSE(S.isValid());
}

TEST(IntersectionTest, UpdatesWhenLinesMoves) {
    // Horizontale und vertikale Linie, Schnittpunkt (2,0)
    Point p1(0, 0), p2(4, 0);
    Point p3(2,-1), p4(2, 1);
    Line horiz(&p1, &p2);
    Line vert (&p3, &p4);
    IntersectionPoint S(&horiz, &vert);

    EXPECT_NEAR(S.x(), 2.0, EPS);

    // Vertikale Linie nach x=3 verschieben
    p3.moveTo(3, -1);
    p4.moveTo(3,  1);
    EXPECT_NEAR(S.x(), 3.0, EPS);
}

TEST(IntersectionTest, BecomesInvalidWhenLinesBecomesParallel) {
    Point p1(0,0), p2(1,0);
    Point p3(0,1), p4(1,2); // nicht parallel
    Line L1(&p1, &p2);
    Line L2(&p3, &p4);
    IntersectionPoint S(&L1, &L2);
    EXPECT_TRUE(S.isValid());

    p4.moveTo(1, 1); // L2 jetzt auch horizontal → parallel
    EXPECT_FALSE(S.isValid());
}*/

// ════════════════════════════════════════════════════════════════════════════
// Midpoint
// ════════════════════════════════════════════════════════════════════════════

TEST(MidpointTest, BasicMidpoint) {
    Point a(0, 0), b(4, 6);
    Midpoint M(&a, &b);

    EXPECT_NEAR(M.x(), 2.0, EPS);
    EXPECT_NEAR(M.y(), 3.0, EPS);
}

TEST(MidpointTest, MidpointOfLine) {
    Point p1(0, 0), p2(10, 0);
    Line L(&p1, &p2);
    Midpoint M(&L);

    EXPECT_NEAR(M.x(), 5.0, EPS);
    EXPECT_NEAR(M.y(), 0.0, EPS);
}

TEST(MidpointTest, UpdatesWhenPointMoves) {
    Point a(0, 0), b(2, 0);
    Midpoint M(&a, &b);
    EXPECT_NEAR(M.x(), 1.0, EPS);

    b.moveTo(4, 0);
    EXPECT_NEAR(M.x(), 2.0, EPS);
}

TEST(MidpointTest, IdenticalPoints) {
    Point a(3, 3), b(3, 3);
    Midpoint M(&a, &b);
    EXPECT_NEAR(M.x(), 3.0, EPS);
    EXPECT_NEAR(M.y(), 3.0, EPS);
}

// ════════════════════════════════════════════════════════════════════════════
// PerpendicularFoot
// ════════════════════════════════════════════════════════════════════════════

TEST(PerpendicularFootTest, PointAboveHorizontalLine) {
    // Linie entlang x-Achse, Punkt bei (3, 5) → Lotfuß bei (3, 0)
    Point p1(0, 0), p2(10, 0);
    Line L(&p1, &p2);
    Point P(3, 5);
    PerpendicularFoot F(&P, &L);

    EXPECT_NEAR(F.x(), 3.0, EPS);
    EXPECT_NEAR(F.y(), 0.0, EPS);
}

TEST(PerpendicularFootTest, PointOnLine) {
    Point p1(0, 0), p2(10, 0);
    Line L(&p1, &p2);
    Point P(4, 0); // liegt bereits auf der Linie
    PerpendicularFoot F(&P, &L);

    EXPECT_NEAR(F.x(), 4.0, EPS);
    EXPECT_NEAR(F.y(), 0.0, EPS);
}

TEST(PerpendicularFootTest, DiagonalLine) {
    // Linie durch (0,0) und (1,1), Punkt bei (1,0) → Lot bei (0.5, 0.5)
    Point p1(0, 0), p2(1, 1);
    Line L(&p1, &p2);
    Point P(1, 0);
    PerpendicularFoot F(&P, &L);

    EXPECT_NEAR(F.x(), 0.5, EPS);
    EXPECT_NEAR(F.y(), 0.5, EPS);
}

TEST(PerpendicularFootTest, UpdatesWhenPointMoves) {
    Point p1(0,0), p2(10, 0);
    Line L(&p1, &p2);
    Point P(3, 5);
    PerpendicularFoot F(&P, &L);

    P.moveTo(7, 5);
    EXPECT_NEAR(F.x(), 7.0, EPS);
}

// ════════════════════════════════════════════════════════════════════════════
// Chained constructions
// ════════════════════════════════════════════════════════════════════════════

/*TEST(ChainedTest, MidpointOfIntersectionAndPoint) {
    // Schnittpunkt S zweier Linien, dann Mittelpunkt zwischen S und einem
    // weiteren Punkt – transitiv abhängig
    Point p1(-1,0), p2(1,0);
    Point p3(0,-1), p4(0,1);
    Line L1(&p1, &p2), L2(&p3, &p4);
    IntersectionPoint S(&L1, &L2); // S = (0,0)

    Point Q(4, 0);
    Midpoint M(&S, &Q);       // M = (2, 0)

    EXPECT_NEAR(M.x(), 2.0, EPS);

    // Verschiebt sich L2, ändert sich S und damit auch M
    p3.moveTo(2, -1);
    p4.moveTo(2,  1);  // S jetzt bei (2, 0)
    EXPECT_NEAR(S.x(), 2.0, EPS);
    EXPECT_NEAR(M.x(), 3.0, EPS); // M = (2+4)/2 = 3
}*/

TEST(ChainedTest, PerpendicularFootOnDerivedLine) {
    // Lotfuß auf eine Parallele – zwei Konstruktionen hintereinander
    Point p1(0, 0), p2(4, 0);
    Line base(&p1, &p2);

    Point origin(0, 2);
    Parallel para(&origin, &base); // Parallele y=2

    Point P(3, 5);
    PerpendicularFoot F(&P, para.line()); // Lot von P auf y=2 → (3,2)

    EXPECT_NEAR(F.x(), 3.0, EPS);
    EXPECT_NEAR(F.y(), 2.0, EPS);
}
