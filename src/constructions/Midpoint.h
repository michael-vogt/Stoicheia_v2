#pragma once

#include "../geometry/LinearObject.h"
#include "../geometry/Point.h"


class Midpoint : public Point {
public:
    Midpoint(Point *a, Point *b);
    explicit Midpoint(const LinearObject* line);

    // Getter
    Point* p1() const { return m_a; }
    Point* p2() const { return m_b; }

    // Objekt aktualisieren
    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    void replaceSource(GeoObject *oldSource, GeoObject *newSource) override;

    std::string toString() override;

private:
    Point* m_a;
    Point* m_b;
};