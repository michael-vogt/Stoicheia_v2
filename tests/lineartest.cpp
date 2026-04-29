#include <gtest/gtest.h>
#include <cmath>
#include "constructions/constructions.h"
#include "geometry/geometry.h"

static constexpr double EPS = 1e-9;

// ════════════════════════════════════════════════════════════════════════════
// Segment
// ════════════════════════════════════════════════════════════════════════════

TEST(SegmentTest, Length) {
    Point p1(0,0), p2(3,4);
    Segment s(&p1, &p2);
    EXPECT_NEAR(s.length(), 5.0, EPS);
}

TEST(SegmentTest, UpdatesWhenPointMoves) {
    Point p1(0,0), p2(1,0);
    Segment s(&p1, &p2);
    p2.moveTo(4, 0);
    EXPECT_NEAR(s.length(), 4.0, EPS);
}

TEST(SegmentTest, ContainsInteriorPoint) {
    Point p1(0,0), p2(4,0);
    Segment s(&p1, &p2);
    EXPECT_TRUE (s.contains(2.0, 0.0)); // Mittelpunkt
    EXPECT_TRUE (s.contains(0.0, 0.0)); // Startpunkt
    EXPECT_TRUE (s.contains(4.0, 0.0)); // Endpunkt
}

TEST(SegmentTest, DoesNotContainExteriorPoint) {
    Point p1(0,0), p2(4,0);
    Segment s(&p1, &p2);
    EXPECT_FALSE(s.contains(5.0, 0.0)); // jenseits p2
    EXPECT_FALSE(s.contains(-1.0, 0.0)); // jenseits p1
    EXPECT_FALSE(s.contains(2.0, 1.0)); // daneben
}

TEST(SegmentTest, StartAndEnd) {
    Point p1(1,2), p2(3,4);
    Segment s(&p1, &p2);
    EXPECT_EQ(s.start(), &p1);
    EXPECT_EQ(s.end(),   &p2);
}

// ════════════════════════════════════════════════════════════════════════════
// Ray
// ════════════════════════════════════════════════════════════════════════════

TEST(RayTest, ContainsOrigin) {
    Point p1(0,0), p2(1,0);
    Ray r(&p1, &p2);
    EXPECT_TRUE(r.contains(0.0, 0.0));
}

TEST(RayTest, ContainsPointBeyondP2) {
    // Halbgerade geht über p2 hinaus
    Point p1(0,0), p2(1,0);
    Ray r(&p1, &p2);
    EXPECT_TRUE(r.contains(5.0, 0.0));
    EXPECT_TRUE(r.contains(100.0, 0.0));
}

TEST(RayTest, DoesNotContainPointBeforeOrigin) {
    Point p1(0,0), p2(1,0);
    Ray r(&p1, &p2);
    EXPECT_FALSE(r.contains(-1.0, 0.0));
    EXPECT_FALSE(r.contains(-0.01, 0.0));
}

TEST(RayTest, OriginAndDirection) {
    Point p1(2,3), p2(5,7);
    Ray r(&p1, &p2);
    EXPECT_EQ(r.origin(),    &p1);
    EXPECT_EQ(r.direction(), &p2);
}

TEST(RayTest, UpdatesWhenOriginMoves) {
    // p1=(0,0), p2=(3,0): Halbgerade zeigt nach rechts
    Point p1(0,0), p2(3,0);
    Ray r(&p1, &p2);
    EXPECT_TRUE (r.contains(5.0, 0.0)); // vor dem Move: rechts ok
    EXPECT_FALSE(r.contains(-1.0, 0.0)); // vor dem Move: links nicht ok

    p1.moveTo(4, 0); // Ursprung nach rechts von p2=(3,0)
    // Richtung dreht sich: jetzt zeigt die Halbgerade nach LINKS
    EXPECT_TRUE (r.contains(2.0, 0.0)); // links vom neuen Ursprung → ok
    EXPECT_FALSE(r.contains(5.0, 0.0)); // rechts vom neuen Ursprung → nicht ok
}

// ════════════════════════════════════════════════════════════════════════════
// Line – containsT immer true
// ════════════════════════════════════════════════════════════════════════════

TEST(LineTest, ContainsPointBeforeP1) {
    Point p1(0,0), p2(1,0);
    Line L(&p1, &p2);
    EXPECT_TRUE(L.contains(-5.0, 0.0));
}

TEST(LineTest, ContainsPointBeyondP2) {
    Point p1(0,0), p2(1,0);
    Line L(&p1, &p2);
    EXPECT_TRUE(L.contains(100.0, 0.0));
}

// ════════════════════════════════════════════════════════════════════════════
// Polymorphismus – Konstruktionen mit LinearObject*
// ════════════════════════════════════════════════════════════════════════════

TEST(LinearObjectTest, MidpointWorksWithSegment) {
    Point p1(0,0), p2(4,0);
    Segment s(&p1, &p2);
    Midpoint M(&s);
    EXPECT_NEAR(M.x(), 2.0, EPS);
}

TEST(LinearObjectTest, MidpointWorksWithRay) {
    Point p1(0,0), p2(6,0);
    Ray r(&p1, &p2);
    Midpoint M(&r); // Mittelpunkt zwischen Ursprung und Richtungspunkt
    EXPECT_NEAR(M.x(), 3.0, EPS);
}

TEST(LinearObjectTest, ParallelToSegment) {
    Point p1(0,0), p2(4,0);
    Segment base(&p1, &p2);
    Point origin(0, 2);
    Parallel para(&origin, &base);
    // Parallele hat denselben Richtungsvektor
    EXPECT_NEAR(para.line()->dy(), 0.0, EPS);
}

TEST(LinearObjectTest, PerpendicularToRay) {
    Point p1(0,0), p2(1,0);
    Ray r(&p1, &p2);
    Point origin(2, 0);
    Perpendicular perp(&origin, &r);
    // Senkrechter Richtungsvektor zur x-Richtung muss vertikal sein
    EXPECT_NEAR(perp.getLine()->dx(), 0.0, EPS);
}

TEST(LinearObjectTest, PerpendicularFootOnSegment) {
    Point p1(0,0), p2(4,0);
    Segment s(&p1, &p2);
    Point P(2, 3);
    PerpendicularFoot F(&P, &s);
    EXPECT_NEAR(F.x(), 2.0, EPS);
    EXPECT_NEAR(F.y(), 0.0, EPS);
}

TEST(LinearObjectTest, LineLineIntersectionWithSegments) {
    // Zwei Segmente deren Trägergeraden sich schneiden
    Point p1(0,0), p2(4,0);
    Point p3(2,-2), p4(2,2);
    Segment s1(&p1,&p2), s2(&p3,&p4);
    LineLineIntersection S(&s1, &s2);
    EXPECT_TRUE(S.first()->isValid());
    EXPECT_NEAR(S.first()->x(), 2.0, EPS);
    EXPECT_NEAR(S.first()->y(), 0.0, EPS);
}

TEST(LinearObjectTest, TriangleWithSegments) {
    Point p1(0,0), p2(3,0), p3(0,4);
    Segment ab(&p1,&p2), bc(&p2,&p3), ca(&p3,&p1);
    Triangle tri(&ab, &bc, &ca);
    EXPECT_NEAR(tri.perimeter(), 12.0, EPS);
}

TEST(LinearObjectTest, TriangleUpdatesTransitively) {
    Point p1(0,0), p2(3,0), p3(0,4);
    Segment ab(&p1,&p2), bc(&p2,&p3), ca(&p3,&p1);
    Triangle tri(&ab, &bc, &ca);
    p2.moveTo(4, 0); // ab: 4, bc: 5→sqrt(32)≈5.66, ca: 4
    EXPECT_NEAR(tri.perimeter(),
        4.0 + std::sqrt(32.0) + 4.0, 1e-6);
}