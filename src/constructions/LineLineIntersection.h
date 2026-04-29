#ifndef STOICHEIA_LINELINEINTERSECTION_H
#define STOICHEIA_LINELINEINTERSECTION_H

#include "IntersectionSet.h"

class LineLineIntersection : public IntersectionSet {
    LinearObject* m_line1;
    LinearObject* m_line2;

protected:
    void compute() override;

public:
    LineLineIntersection(LinearObject* line1, LinearObject* line2);
    void onSourceRemoved(GeoObject *src) override;

};


#endif //STOICHEIA_LINELINEINTERSECTION_H
