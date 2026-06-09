#pragma once

#include "IntersectionPoint.h"
#include "../geometry/GeoObject.h"
#include "Structs.h"
#include <array>


class IntersectionSet : public GeoObject {
public:
    auto first() -> IntersectionPoint*  { return m_pts.data(); };
    auto second() -> IntersectionPoint* { return &m_pts[1]; };

    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;

protected:
    void setResults(int count, DoublePair point1 = {.x=0.0, .y=0.0}, DoublePair point2 = {.x=0.0, .y=0.0});
    virtual void compute() = 0;

private:
    std::array<IntersectionPoint, 2> m_pts;
};