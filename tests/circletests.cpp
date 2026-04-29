#include <gtest/gtest.h>
#include <cmath>
#include "constructions/constructions.h"
#include "geometry/geometry.h"

static constexpr double EPS = 1e-9;

// ════════════════════════════════════════════════════════════════════════════
// Circle
// ════════════════════════════════════════════════════════════════════════════

TEST(CircleTest, RadiusFromPoints) {
    Point center(0, 0), rp(3, 4);
    Circle C(&center, &rp);
    EXPECT_NEAR(C.radius(), 5.0, EPS);
}

TEST(CircleTest, FixedRadius) {
    Point center(0, 0);
    Circle C(&center, 7.0);
    EXPECT_NEAR(C.radius(), 7.0, EPS);
}

TEST(CircleTest, UpdatesWhenCenterMoves) {
    // Mit Radiuspunkt: Radius = geometrischer Abstand center→rp.
    // Bewegt sich center, ändert sich auch der Radius.
    Point center(0, 0), rp(5, 0);
    Circle C(&center, &rp); // r=5
    center.moveTo(2, 0);    // rp bleibt bei (5,0), Abstand = 3
    EXPECT_NEAR(C.radius(), 3.0, EPS);
    EXPECT_EQ(C.center(), &center);
}

TEST(CircleTest, UpdatesWhenRadiusPointMoves) {
    Point center(0, 0), rp(1, 0);
    Circle C(&center, &rp);
    rp.moveTo(0, 6);
    EXPECT_NEAR(C.radius(), 6.0, EPS);
}

TEST(CircleTest, ZeroRadius) {
    Point center(3, 3), rp(3, 3);
    Circle C(&center, &rp);
    EXPECT_NEAR(C.radius(), 0.0, EPS);
}

// ════════════════════════════════════════════════════════════════════════════
// LineCircleIntersection
// ════════════════════════════════════════════════════════════════════════════

TEST(LineCircleTest, TwoIntersections) {
    // Einheitskreis, x-Achse → Schnittpunkte bei (-1,0) und (1,0)
    Point center(0,0), rp(1,0);
    Circle C(&center, &rp);
    Point p1(-2, 0), p2(2, 0);
    Line L(&p1, &p2);
    LineCircleIntersection S(&L, &C);

    EXPECT_TRUE(S.first()->isValid());
    EXPECT_TRUE(S.second()->isValid());
    // first = linkerer Punkt
    EXPECT_NEAR(S.first()->x(),  -1.0, EPS);
    EXPECT_NEAR(S.first()->y(),   0.0, EPS);
    EXPECT_NEAR(S.second()->x(),  1.0, EPS);
    EXPECT_NEAR(S.second()->y(),  0.0, EPS);
}

TEST(LineCircleTest, Tangent) {
    // Linie y=1 tangiert Einheitskreis in (0,1)
    Point center(0,0), rp(1,0);
    Circle C(&center, &rp);
    Point p1(-2, 1), p2(2, 1);
    Line L(&p1, &p2);
    LineCircleIntersection S(&L, &C);

    EXPECT_TRUE(S.first()->isValid());
    EXPECT_FALSE(S.second()->isValid());
    EXPECT_NEAR(S.first()->x(), 0.0, 1e-6);
    EXPECT_NEAR(S.first()->y(), 1.0, 1e-6);
}

TEST(LineCircleTest, NoIntersection) {
    // Linie y=2 außerhalb des Einheitskreises
    Point center(0,0), rp(1,0);
    Circle C(&center, &rp);
    Point p1(-2, 2), p2(2, 2);
    Line L(&p1, &p2);
    LineCircleIntersection S(&L, &C);

    EXPECT_FALSE(S.first()->isValid());
    EXPECT_FALSE(S.second()->isValid());
}

TEST(LineCircleTest, UpdatesWhenCircleGrows) {
    // Linie y=2, Kreis wächst von r=1 auf r=3 → dann 2 Schnittpunkte
    Point center(0,0), rp(1,0);
    Circle C(&center, &rp);
    Point p1(-5, 2), p2(5, 2);
    Line L(&p1, &p2);
    LineCircleIntersection S(&L, &C);

    EXPECT_FALSE(S.first()->isValid()); // r=1, kein Schnitt

    rp.moveTo(3, 0); // r=3 jetzt
    EXPECT_TRUE(S.first()->isValid());
    EXPECT_TRUE(S.second()->isValid());
}

TEST(LineCircleTest, UpdatesWhenLineMoves) {
    Point center(0,0), rp(5,0);
    Circle C(&center, &rp); // r=5
    Point p1(-6, 3), p2(6, 3);
    Line L(&p1, &p2);
    LineCircleIntersection S(&L, &C);

    EXPECT_TRUE(S.first()->isValid());

    // Linie nach y=6 verschieben → außerhalb
    p1.moveTo(-6, 6); p2.moveTo(6, 6);
    EXPECT_FALSE(S.first()->isValid());
}

TEST(LineCircleTest, StableOrderingAfterMove) {
    // first bleibt immer der linkere Punkt
    Point center(0,0), rp(5,0);
    Circle C(&center, &rp);
    Point p1(-6, 0), p2(6, 0);
    Line L(&p1, &p2);
    LineCircleIntersection S(&L, &C);

    double fx_before = S.first()->x(); // -5

    // Kreis nach rechts verschieben
    center.moveTo(2, 0); // Schnittpunkte: -3 und 7
    EXPECT_LT(S.first()->x(), S.second()->x()); // Ordnung stabil
    EXPECT_NE(S.first()->x(), fx_before); // Koordinaten haben sich geändert
}

TEST(LineCircleTest, VerticalLine) {
    // x=0 schneidet Einheitskreis bei (0,-1) und (0,1)
    Point center(0,0), rp(1,0);
    Circle C(&center, &rp);
    Point p1(0,-2), p2(0,2);
    Line L(&p1, &p2);
    LineCircleIntersection S(&L, &C);

    EXPECT_NEAR(S.first()->x(),  0.0, EPS);
    EXPECT_NEAR(S.first()->y(), -1.0, EPS);
    EXPECT_NEAR(S.second()->x(), 0.0, EPS);
    EXPECT_NEAR(S.second()->y(), 1.0, EPS);
}

// ════════════════════════════════════════════════════════════════════════════
// CircleCircleIntersection
// ════════════════════════════════════════════════════════════════════════════

TEST(CircleCircleTest, TwoIntersections) {
    // Zwei Einheitskreise bei (0,0) und (1,0)
    // Schnittpunkte bei (0.5, ±√3/2)
    Point c1(0,0), r1(1,0);
    Point c2(1,0), r2(2,0); // r=1
    Circle C1(&c1, &r1), C2(&c2, &r2);
    CircleCircleIntersection S(&C1, &C2);

    EXPECT_TRUE(S.first()->isValid());
    EXPECT_TRUE(S.second()->isValid());
    EXPECT_NEAR(S.first()->x(),  0.5, EPS);
    EXPECT_NEAR(S.second()->x(), 0.5, EPS);
    // y-Koordinaten sind ±√3/2, first hat kleineres y
    EXPECT_NEAR(S.first()->y(),  -std::sqrt(3.0)/2.0, 1e-9);
    EXPECT_NEAR(S.second()->y(),  std::sqrt(3.0)/2.0, 1e-9);
}

TEST(CircleCircleTest, ExternalNoIntersection) {
    Point c1(0,0), r1(1,0);
    Point c2(5,0), r2(6,0); // r=1, d=5 > r1+r2=2
    Circle C1(&c1, &r1), C2(&c2, &r2);
    CircleCircleIntersection S(&C1, &C2);

    EXPECT_FALSE(S.first()->isValid());
    EXPECT_FALSE(S.second()->isValid());
}

TEST(CircleCircleTest, InternalNoIntersection) {
    // Kleiner Kreis komplett im großen
    Point c1(0,0), r1(5,0); // r=5
    Point c2(1,0), r2(2,0); // r=1, d=1 < r1-r2=4
    Circle C1(&c1, &r1), C2(&c2, &r2);
    CircleCircleIntersection S(&C1, &C2);

    EXPECT_FALSE(S.first()->isValid());
}

TEST(CircleCircleTest, ExternalTangent) {
    // Zwei Kreise berühren sich von außen: d = r1+r2
    Point c1(0,0), r1(2,0); // r=2
    Point c2(3,0), r2(4,0); // r=1, d=3 = r1+r2
    Circle C1(&c1, &r1), C2(&c2, &r2);
    CircleCircleIntersection S(&C1, &C2);

    EXPECT_TRUE(S.first()->isValid());
    EXPECT_FALSE(S.second()->isValid());
    EXPECT_NEAR(S.first()->x(), 2.0, 1e-6);
    EXPECT_NEAR(S.first()->y(), 0.0, 1e-6);
}

TEST(CircleCircleTest, UpdatesWhenCircleMoves) {
    Point c1(0,0), r1(1,0);
    Point c2(5,0), r2(6,0); // zu weit weg
    Circle C1(&c1, &r1), C2(&c2, &r2);
    CircleCircleIntersection S(&C1, &C2);
    EXPECT_FALSE(S.first()->isValid());

    // C2 näher heranschieben
    c2.moveTo(1, 0); r2.moveTo(2, 0); // d=1, beide r=1 → Schnitt
    EXPECT_TRUE(S.first()->isValid());
}

// ════════════════════════════════════════════════════════════════════════════
// Chained: IntersectionPoint als Quelle für weitere Konstruktionen
// ════════════════════════════════════════════════════════════════════════════

TEST(ChainedCircleTest, MidpointOfTwoIntersections) {
    // Mittelpunkt der beiden Schnittpunkte = Lotfuß der Verbindungslinie
    // zweier Kreiszentren → muss bei (0.5, 0) liegen
    Point c1(0,0), r1(1,0);
    Point c2(1,0), r2(2,0);
    Circle C1(&c1, &r1), C2(&c2, &r2);
    CircleCircleIntersection S(&C1, &C2);

    Midpoint M(S.first(), S.second());
    EXPECT_NEAR(M.x(), 0.5, EPS);
    EXPECT_NEAR(M.y(), 0.0, EPS);
}

TEST(ChainedCircleTest, MidpointUpdatesTransitively) {
    Point c1(0,0), r1(1,0);
    Point c2(1,0), r2(2,0);
    Circle C1(&c1, &r1), C2(&c2, &r2);
    CircleCircleIntersection S(&C1, &C2);
    Midpoint M(S.first(), S.second());

    // C2 nach (0.5, 0) verschieben → d=0.5, neue Schnittpunkte symmetrisch um x=0.25
    // a = (0.25 + 1 - 1)/(1) = 0.25 → Schnittpunkte bei x=0.25, Mittelpunkt x=0.25
    c2.moveTo(0.5, 0); r2.moveTo(1.5, 0); // c2=(0.5,0), r2=1
    EXPECT_NEAR(M.x(), 0.25, 1e-9);
}

TEST(ChainedCircleTest, LineCircleIntersectionUsedAsPoint) {
    // Schnittpunkt Linie/Kreis als Endpunkt einer neuen Linie
    Point center(0,0), rp(5,0);
    Circle C(&center, &rp);
    Point p1(-6, 0), p2(6, 0);
    Line L(&p1, &p2);
    LineCircleIntersection S(&L, &C);

    // Neue Linie von first() nach center
    Line L2(S.first(), &center);
    EXPECT_NEAR(L2.length(), 5.0, EPS);

    // Kreis wächst → L2 wird länger
    rp.moveTo(10, 0);
    EXPECT_NEAR(L2.length(), 10.0, EPS);
}