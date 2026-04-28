#ifndef STOICHEIA_PERPENDICULAR_H
#define STOICHEIA_PERPENDICULAR_H
#include "geometry/GeoObject.h"
#include "geometry/Line.h"
#include "geometry/Point.h"


class Perpendicular : public GeoObject {
    Point* m_origin;
    Line*  m_reference;

    Point m_phantom;
    Line  m_line;

    double px() const;
    double py() const;
public:
    Perpendicular(Point* origin, Line* reference);

    void recompute() override;

    void onSourceRemoved(GeoObject* src) override;

    Line* getLine();

    std::string toString() override;
};


#endif //STOICHEIA_PERPENDICULAR_H
