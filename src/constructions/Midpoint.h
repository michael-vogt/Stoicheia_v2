#pragma once

#include "../geometry/LinearObject.h"
#include "../geometry/Point.h"
#include "Structs.h"


class Midpoint : public Point {
public:
    Midpoint(Point *point1, Point *point2);
    explicit Midpoint(const LinearObject* line);

    // Getter
    [[nodiscard]] auto point1() const -> Point* { return m_point1; }
    [[nodiscard]] auto point2() const -> Point* { return m_point2; }

    // Objekt aktualisieren
    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    void replaceSource(GeoObjectPair source) override;

private:
    Point* m_point1;
    Point* m_point2;
};