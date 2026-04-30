#ifndef STOICHEIA_LINEAROBJECT_H
#define STOICHEIA_LINEAROBJECT_H
#include "geometryHelper.h"
#include "GeoObject.h"
#include "Point.h"


class LinearObject : public GeoObject {
protected:
    double m_a;
    double m_b;
    double m_c;
    Point* m_p1 = nullptr;
    Point* m_p2 = nullptr;
    double m_length = 0.0;

    virtual bool containsT(double t) const = 0;
public:
    int recomputeCount = 0; // for testing purpose
    /*
     * Constructors and destructor
     */
    LinearObject(double a, double b, double c);
    LinearObject(Point* p1, Point* p2);

    /*
     * Getter
     */
    Point* p1() const { return m_p1; };
    Point* p2() const { return m_p2; };
    double dx() const { return m_p2->x() - m_p1->x(); };
    double dy() const { return m_p2->y() - m_p1->y(); };
    LineParameters parameters() const { return { m_a, m_b, m_c }; };
    LinePoints points() const { return { m_p1, m_p2 }; };
    double length() const { return m_length; };

    bool contains(double px, double py, double eps = 1e-9) const;
    double projectParameter(double px, double py) const;

    /*
     * Overridden methods
     */
    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    std::string toString() override;

};


#endif //STOICHEIA_LINEAROBJECT_H
