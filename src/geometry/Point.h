#pragma once

#include "GeoObject.h"


class Point : public GeoObject {
public:
    Point(double pos_x, double pos_y);

    // Getter
    [[nodiscard]] auto x() const -> double { return m_x; }
    [[nodiscard]] auto y() const -> double { return m_y; }

    // Objekt aktualisieren
    void moveTo(double new_x, double new_y);
    void recompute() override {}

private:
    double m_x;
    double m_y;
};
