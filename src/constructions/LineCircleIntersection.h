#pragma once

#include "IntersectionSet.h"


class LineCircleIntersection : public IntersectionSet {
public:
    LineCircleIntersection(LinearObject* line, Circle* circle);

    // Getter
    Circle* circle() const { return m_circle; }
    LinearObject* line() const { return m_line; }

    // Quellen aktualisieren
    void onSourceRemoved(GeoObject *src) override;
    void replaceSource(GeoObject *oldSource, GeoObject *newSource) override;

protected:
    void compute() override;

private:
    LinearObject* m_line = nullptr;
    Circle* m_circle = nullptr;
};