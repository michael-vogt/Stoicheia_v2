#ifndef STOICHEIA_LINECIRCLEINTERSECTION_H
#define STOICHEIA_LINECIRCLEINTERSECTION_H
#include "IntersectionSet.h"


class LineCircleIntersection : public IntersectionSet {
    Line* m_line = nullptr;
    Circle* m_circle = nullptr;

protected:
    void compute() override;

public:
    LineCircleIntersection(Line* line, Circle* circle);
    void onSourceRemoved(GeoObject *src) override;
};


#endif //STOICHEIA_LINECIRCLEINTERSECTION_H
