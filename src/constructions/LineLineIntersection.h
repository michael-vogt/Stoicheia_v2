#pragma once

#include "IntersectionSet.h"

class LineLineIntersection : public IntersectionSet {
    LinearObject* m_line1;
    LinearObject* m_line2;

protected:
    void compute() override;

public:
    LineLineIntersection(LinearObject* line1, LinearObject* line2);
    LinearObject* L1() const { return m_line1; }
    LinearObject* L2() const { return m_line2; }
    void onSourceRemoved(GeoObject *src) override;

    void replaceSource(GeoObject *oldSource, GeoObject *newSource) override;

    bool equals(const GeoObject &other) const override;

};