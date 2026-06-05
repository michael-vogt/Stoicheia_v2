#pragma once

#include "geometryHelper.h"
#include "GeoObject.h"
#include "Point.h"


class LinearObject : public GeoObject {
public:
    int recomputeCount = 0; // for testing purpose

    LinearObject(double a, double b, double c);
    LinearObject(Point* p1, Point* p2);

    // Getter
    double dx() const { return m_p2->x() - m_p1->x(); }
    double dy() const { return m_p2->y() - m_p1->y(); }
    double length() const { return m_length; }
    Point* p1() const { return m_p1; }
    Point* p2() const { return m_p2; }
    LineParameters parameters() const { return { m_a, m_b, m_c }; }
    LinePoints points() const { return { m_p1, m_p2 }; }

    // Punktprobe
    bool contains(double px, double py, double eps = 1e-9) const;
    bool containsParameter(const double t) const { return containsT(t); }
    double projectParameter(double px, double py) const;

    // Objekt aktualisieren
    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    void replaceSource(GeoObject *oldSource, GeoObject *newSource) override;

protected:
    virtual bool containsT(double t) const = 0;

    double m_a;
    double m_b;
    double m_c;
    Point* m_p1 = nullptr;
    Point* m_p2 = nullptr;
    double m_length = 0.0;
};
