#ifndef STOICHEIA_POINT_H
#define STOICHEIA_POINT_H
#include "GeoObject.h"


class Point : public GeoObject {
    double m_x;
    double m_y;
public:
    /*
     * Constructor
     */
    Point(double x, double y);

    /*
     * Getter
     */
    double x() const;
    double y() const;

    /*
     * Modifier
     */
    void moveTo(double nx, double ny);

    /*
     * Overridden methods
     */
    void recompute() override;
    std::string toString() override;

};


#endif //STOICHEIA_POINT_H
