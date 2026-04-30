#pragma once

#include "IntersectionSet.h"


class LineCircleIntersection : public IntersectionSet {
    LinearObject* m_line = nullptr;
    Circle* m_circle = nullptr;

protected:
    void compute() override;

public:
    LineCircleIntersection(LinearObject* line, Circle* circle);
    void onSourceRemoved(GeoObject *src) override;
};