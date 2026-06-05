#pragma once

#include "../geometry/GeoObject.h"
#include "../geometry/Line.h"
#include "../geometry/LinearObject.h"
#include "../geometry/Point.h"


class Parallel : public GeoObject {
public:
    Parallel(Point* origin, LinearObject* reference);

    // Getter
    Point* origin() const           { return m_origin; }
    LinearObject* reference() const { return m_reference; }
    LinearObject* line()            { return &m_line; }

    // Objekt aktualisieren
    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    void replaceSource(GeoObject *oldSource, GeoObject *newSource) override;

private:
    // Steigung der Referenzlinie
    double dx() const;
    double dy() const;

    Point* m_origin;
    Point m_phantom;
    LinearObject* m_reference;
    Line m_line;
};