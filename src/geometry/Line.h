#ifndef STOICHEIA_LINE_H
#define STOICHEIA_LINE_H
#include "geometryHelper.h"
#include "GeoObject.h"
#include "Point.h"


class Line : public GeoObject {
    double m_a;
    double m_b;
    double m_c;
    Point* m_p1;
    Point* m_p2;
public:
    int recomputeCount = 0; // for testing purpose
    /*
     * Constructors and destructor
     */
    Line(double a, double b, double c);
    Line(Point* p1, Point* p2);

    /*
     * Getter
     */
    Point* p1() const;
    Point* p2() const;
    LineParameters parameters() const;
    LinePoints points() const;
    double getLength() const;

    /*
     * Overridden methods
     */
    void recompute() override;
    std::string toString() override;

};


#endif //STOICHEIA_LINE_H
