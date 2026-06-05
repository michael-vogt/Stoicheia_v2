#pragma once

#include "GeoObject.h"


class Point : public GeoObject {
public:
    Point(double x, double y);

    // Getter
    double x() const { return m_x; }
    double y() const { return m_y; }

    // Objekt aktualisieren
    void moveTo(double nx, double ny);
    void recompute() override {}

    bool equals(const GeoObject &other) const override;
    std::string toString() override;

private:
    double m_x;
    double m_y;
};
