#pragma once

#include "IntersectionSet.h"
#include "../geometry/Circle.h"

class CircleCircleIntersection : public IntersectionSet {
public:
    CircleCircleIntersection(Circle* c1, Circle* c2);

    // Getter
    Circle* c1() const { return m_c1; }
    Circle* c2() const { return m_c2; }

    // Quellen aktualisieren
    void onSourceRemoved(GeoObject* src) override;
    void replaceSource(GeoObject *oldSource, GeoObject *newSource) override;

    bool equals(const GeoObject &other) const override;

protected:
    void compute() override;

private:
    Circle* m_c1 = nullptr;
    Circle* m_c2 = nullptr;
};