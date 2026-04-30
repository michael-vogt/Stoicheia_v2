#pragma once

#include "GeoObject.h"


class Point : public GeoObject {
    double m_x;
    double m_y;
public:
    /*
     * Constructor
     */
    Point(double x, double y) : m_x(x), m_y(y) {};

    /*
     * Getter
     */
    double x() const { return m_x; };
    double y() const { return m_y; };

    /*
     * Modifier
     */
    void moveTo(double nx, double ny);

    /*
     * Overridden methods
     */
    void recompute() override {};
    std::string toString() override;

};
