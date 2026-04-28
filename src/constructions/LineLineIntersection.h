#ifndef STOICHEIA_LINELINEINTERSECTION_H
#define STOICHEIA_LINELINEINTERSECTION_H

#include "IntersectionSet.h"

class LineLineIntersection : public IntersectionSet {
    Line* m_line1;
    Line* m_line2;

protected:
    void compute() override;

public:
    LineLineIntersection(Line* line1, Line* line2);
    void onSourceRemoved(GeoObject *src) override;

};


#endif //STOICHEIA_LINELINEINTERSECTION_H
