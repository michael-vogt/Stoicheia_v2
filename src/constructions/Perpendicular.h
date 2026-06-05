#pragma once

#include "../geometry/GeoObject.h"
#include "../geometry/Line.h"
#include "../geometry/LinearObject.h"
#include "../geometry/Point.h"


class Perpendicular : public GeoObject {
public:
    Perpendicular(Point* origin, LinearObject* reference);

    Point* origin() const { return m_origin; }
    LinearObject* reference() const { return m_reference; }
    LinearObject* line() { return &m_line; };

    void onSourceRemoved(GeoObject* src) override;
    void recompute() override;
    void replaceSource(GeoObject* oldSource, GeoObject* newSource) override;

private:
    // Komponenten des Normalenvektors der Referenzlinie
    double px() const;
    double py() const;

    Point* m_origin;
    LinearObject*  m_reference;
    Point m_phantom;
    Line  m_line;
};