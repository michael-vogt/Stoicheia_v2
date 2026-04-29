#include <gtest/gtest.h>
#include <cmath>
#include "geometry/geometry.h"

static constexpr double EPS = 1e-9;

// ════════════════════════════════════════════════════════════════════════════
// 1. POINT – Grundverhalten
// ════════════════════════════════════════════════════════════════════════════

TEST(PointTest, InitialCoordinates) {
    Point p(3.0, 4.0);
    EXPECT_DOUBLE_EQ(p.x(), 3.0);
    EXPECT_DOUBLE_EQ(p.y(), 4.0);
}

TEST(PointTest, MoveTo) {
    Point p(0.0, 0.0);
    p.moveTo(5.0, 7.0);
    EXPECT_DOUBLE_EQ(p.x(), 5.0);
    EXPECT_DOUBLE_EQ(p.y(), 7.0);
}

TEST(PointTest, IsValidInitially) {
    Point p(1.0, 2.0);
    EXPECT_TRUE(p.isValid());
}

TEST(PointTest, NegativeCoordinates) {
    Point p(-3.5, -7.2);
    EXPECT_DOUBLE_EQ(p.x(), -3.5);
    EXPECT_DOUBLE_EQ(p.y(), -7.2);
}

// ════════════════════════════════════════════════════════════════════════════
// 2. LINE – Längenberechnung
// ════════════════════════════════════════════════════════════════════════════

TEST(LineTest, LengthOnCreation) {
    Point p1(0, 0), p2(3, 4);
    Line L(&p1, &p2);
    EXPECT_NEAR(L.length(), 5.0, EPS);
}

TEST(LineTest, LengthHorizontal) {
    Point p1(0, 0), p2(10, 0);
    Line L(&p1, &p2);
    EXPECT_NEAR(L.length(), 10.0, EPS);
}

TEST(LineTest, LengthVertical) {
    Point p1(0, 0), p2(0, 7);
    Line L(&p1, &p2);
    EXPECT_NEAR(L.length(), 7.0, EPS);
}

TEST(LineTest, LengthZero) {
    Point p1(3, 3), p2(3, 3);
    Line L(&p1, &p2);
    EXPECT_NEAR(L.length(), 0.0, EPS);
}

TEST(LineTest, LengthDiagonal) {
    Point p1(1, 1), p2(4, 5);
    Line L(&p1, &p2);
    EXPECT_NEAR(L.length(), 5.0, EPS);
}

// ════════════════════════════════════════════════════════════════════════════
// 3. PROPAGATION – Abhängigkeiten werden aktualisiert
// ════════════════════════════════════════════════════════════════════════════

TEST(PropagationTest, LineUpdatesWhenP2Moves) {
    Point p1(0, 0), p2(3, 4);
    Line L(&p1, &p2);

    p2.moveTo(6, 8);
    EXPECT_NEAR(L.length(), 10.0, EPS);
}

TEST(PropagationTest, LineUpdatesWhenP1Moves) {
    Point p1(0, 0), p2(5, 0);
    Line L(&p1, &p2);

    p1.moveTo(2, 0);
    EXPECT_NEAR(L.length(), 3.0, EPS);
}

TEST(PropagationTest, LineUpdatesOnMultipleMoves) {
    Point p1(0, 0), p2(1, 0);
    Line L(&p1, &p2);

    for (int i = 1; i <= 5; ++i) {
        p2.moveTo(i * 2.0, 0);
        EXPECT_NEAR(L.length(), i * 2.0, EPS);
    }
}

TEST(PropagationTest, TransitivePropagation) {
    // P1, P2 → Line → Triangle
    Point p1(0,0), p2(3,0), p3(0,4);
    Line ab(&p1, &p2);
    Line bc(&p2, &p3);
    Line ca(&p3, &p1);
    Triangle tri(&ab, &bc, &ca);

    double before = tri.perimeter();
    p2.moveTo(4, 0); // ab becomes length 4
    EXPECT_GT(tri.perimeter(), before);
}

TEST(PropagationTest, TwoLinesSharePoint) {
    Point p1(0,0), p2(3,4), p3(6,0);
    Line L1(&p1, &p2);
    Line L2(&p2, &p3);

    p2.moveTo(0, 0);
    EXPECT_NEAR(L1.length(), 0.0, EPS);
    EXPECT_NEAR(L2.length(), 6.0, EPS);
}

// ════════════════════════════════════════════════════════════════════════════
// 4. BATCH UPDATE (UpdateGuard)
// ════════════════════════════════════════════════════════════════════════════

TEST(BatchUpdateTest, LineRecomputedOnlyOnce) {
    Point p1(0,0), p2(3,4);
    Line L(&p1, &p2);
    int before = L.recomputeCount;

    {
        UpdateGuard guard;
        p1.moveTo(1, 0);
        p2.moveTo(4, 4);
    }

    // nur ein recompute nach dem Guard
    EXPECT_EQ(L.recomputeCount, before + 1);
}

TEST(BatchUpdateTest, ResultCorrectAfterBatch) {
    Point p1(0,0), p2(0,0);
    Line L(&p1, &p2);

    {
        UpdateGuard guard;
        p1.moveTo(0, 0);
        p2.moveTo(3, 4);
    }

    EXPECT_NEAR(L.length(), 5.0, EPS);
}

TEST(BatchUpdateTest, NestedGuardsFlushOnce) {
    Point p1(0,0), p2(3,4);
    Line L(&p1, &p2);
    int before = L.recomputeCount;

    {
        UpdateGuard outer;
        {
            UpdateGuard inner;
            p1.moveTo(1, 0);
        }
        // inner guard zerstört: depth = 1, kein flush
        EXPECT_EQ(L.recomputeCount, before); // noch nicht!
        p2.moveTo(4, 4);
    }
    // outer guard zerstört: flush
    EXPECT_EQ(L.recomputeCount, before + 1);
}

TEST(BatchUpdateTest, TriangleRecomputedOnceWithGuard) {
    Point p1(0,0), p2(3,0), p3(0,4);
    Line ab(&p1, &p2), bc(&p2, &p3), ca(&p3, &p1);
    Triangle tri(&ab, &bc, &ca);
    int before = tri.recomputeCount;

    {
        UpdateGuard guard;
        p1.moveTo(1, 0);
        p2.moveTo(4, 0);
        p3.moveTo(1, 4);
    }

    EXPECT_EQ(tri.recomputeCount, before + 1);
}

// ════════════════════════════════════════════════════════════════════════════
// 5. SCENE – Ownership & remove()
// ════════════════════════════════════════════════════════════════════════════

TEST(SceneTest, CreateAndSize) {
    Scene scene;
    scene.create<Point>(0.0, 0.0);
    scene.create<Point>(1.0, 1.0);
    EXPECT_EQ(scene.size(), 2);
}

TEST(SceneTest, RemovePoint_LineBecomesInvalid) {
    Scene scene;
    auto* p1 = scene.create<Point>(0.0, 0.0);
    auto* p2 = scene.create<Point>(3.0, 4.0);
    auto* L  = scene.create<Line>(p1, p2);

    EXPECT_TRUE(L->isValid());
    scene.remove(p2);
    EXPECT_FALSE(L->isValid());
    EXPECT_EQ(scene.size(), 2); // p1 + L noch da
}

TEST(SceneTest, RemovePoint_SizeDecreases) {
    Scene scene;
    auto* p1 = scene.create<Point>(0.0, 0.0);
    scene.create<Point>(1.0, 1.0);
    EXPECT_EQ(scene.size(), 2);
    scene.remove(p1);
    EXPECT_EQ(scene.size(), 1);
}

TEST(SceneTest, RemoveLine_PointsUnaffected) {
    Scene scene;
    auto* p1 = scene.create<Point>(0.0, 0.0);
    auto* p2 = scene.create<Point>(3.0, 4.0);
    auto* L  = scene.create<Line>(p1, p2);

    scene.remove(L);
    EXPECT_TRUE(p1->isValid());
    EXPECT_TRUE(p2->isValid());
    EXPECT_EQ(scene.size(), 2);
}

TEST(SceneTest, RemoveCascade_DeletesDependents) {
    Scene scene;
    auto* p1 = scene.create<Point>(0.0, 0.0);
    auto* p2 = scene.create<Point>(3.0, 0.0);
    auto* p3 = scene.create<Point>(0.0, 4.0);
    auto* ab = scene.create<Line>(p1, p2);
    auto* bc = scene.create<Line>(p2, p3);
    auto* ca = scene.create<Line>(p3, p1);
    scene.create<Triangle>(ab, bc, ca);

    // 7 objects: 3 points + 3 lines + 1 triangle
    EXPECT_EQ(scene.size(), 7);

    // p2 löschen + alle transitiven Abhängigen:
    //   ab (hängt an p2), bc (hängt an p2), tri (hängt an ab & bc)
    // → p2, ab, bc, tri werden gelöscht
    scene.removeCascade(p2);

    // p1, p3, ca verbleiben = 3
    EXPECT_EQ(scene.size(), 3);
}

TEST(SceneTest, AfterRemove_RemainingObjectsStillUpdate) {
    Scene scene;
    auto* p1 = scene.create<Point>(0.0, 0.0);
    auto* p2 = scene.create<Point>(0.0, 0.0);
    auto* p3 = scene.create<Point>(3.0, 4.0);
    scene.create<Line>(p1, p2);   // L1 hängt an p1 und p2
    auto* L2 = scene.create<Line>(p2, p3); // L2 hängt an p2 und p3

    scene.remove(p1); // L1 invalid, L2 unberührt

    // L2: p2=(0,0) → p3=(3,4), Länge = 5
    EXPECT_TRUE(L2->isValid());
    EXPECT_NEAR(L2->length(), 5.0, EPS);

    // p3 verschieben → L2 aktualisiert sich noch
    p3->moveTo(0.0, 3.0); // p2=(0,0) → p3=(0,3), Länge = 3
    EXPECT_NEAR(L2->length(), 3.0, EPS);
}

// ════════════════════════════════════════════════════════════════════════════
// 6. DETACH – manuelle Verbindungstrennung
// ════════════════════════════════════════════════════════════════════════════

TEST(DetachTest, DetachStopsUpdates) {
    Point p1(0,0), p2(3,4);
    Line L(&p1, &p2);

    L.detach();
    int countBefore = L.recomputeCount;

    p2.moveTo(6, 8); // L ist nicht mehr verbunden
    EXPECT_EQ(L.recomputeCount, countBefore);
}

TEST(DetachTest, DetachClearsSourcesAndDependents) {
    Point p1(0,0), p2(3,4);
    Line L(&p1, &p2);

    L.detach();
    EXPECT_TRUE(L.dependents().empty());
}

// ════════════════════════════════════════════════════════════════════════════
// 7. EDGE CASES
// ════════════════════════════════════════════════════════════════════════════

TEST(EdgeCaseTest, PointMoveToSamePosition) {
    Point p1(3,4), p2(0,0);
    Line L(&p1, &p2);
    double before = L.length();

    p1.moveTo(3, 4); // keine echte Änderung
    EXPECT_NEAR(L.length(), before, EPS);
}

TEST(EdgeCaseTest, LargeCoordinates) {
    Point p1(0, 0), p2(1e9, 0);
    Line L(&p1, &p2);
    EXPECT_NEAR(L.length(), 1e9, 1.0);
}

TEST(EdgeCaseTest, NegativeCoordinates) {
    Point p1(-3, -4), p2(0, 0);
    Line L(&p1, &p2);
    EXPECT_NEAR(L.length(), 5.0, EPS);
}

TEST(EdgeCaseTest, MultiplePointsOnSameLine) {
    Point origin(0, 0);
    Point a(1, 0), b(2, 0), c(3, 0);

    Line L1(&origin, &a);
    Line L2(&origin, &b);
    Line L3(&origin, &c);

    origin.moveTo(1, 0);

    EXPECT_NEAR(L1.length(), 0.0, EPS);
    EXPECT_NEAR(L2.length(), 1.0, EPS);
    EXPECT_NEAR(L3.length(), 2.0, EPS);
}

TEST(EdgeCaseTest, EmptyGuardFlushIsNoOp) {
    // Kein pending → flush() soll kein Problem machen
    EXPECT_NO_THROW({
        UpdateGuard guard;
        // nichts tun
    });
}

TEST(EdgeCaseTest, TrianglePerimeterCorrect) {
    // 3-4-5 rechtwinkliges Dreieck
    Point p1(0,0), p2(3,0), p3(0,4);
    Line ab(&p1, &p2); // 3
    Line bc(&p2, &p3); // 5
    Line ca(&p3, &p1); // 4
    Triangle tri(&ab, &bc, &ca);

    EXPECT_NEAR(tri.perimeter(), 12.0, EPS);
}