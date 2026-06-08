#pragma once

#include "../geometry/LinearObject.h"
#include "../geometry/Point.h"


class PerpendicularFoot : public Point {
public:
    PerpendicularFoot(Point* point, LinearObject* line);

    [[nodiscard]] auto point() const -> Point* { return m_point; }
    [[nodiscard]] auto line() const -> LinearObject* { return m_line; }

    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    void replaceSource(GeoObjectPair source) override;

private:
    Point* m_point;
    LinearObject* m_line;
};