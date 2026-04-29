#ifndef STOICHEIA_TRIANGLE_H
#define STOICHEIA_TRIANGLE_H
#include "GeoObject.h"
#include "LinearObject.h"


class Triangle : public GeoObject {
    LinearObject* m_a = nullptr;
    LinearObject* m_b = nullptr;
    LinearObject* m_c = nullptr;
    double m_perimater = 0.0;
public:
    int recomputeCount = 0;

    Triangle(LinearObject* a, LinearObject* b, LinearObject* c);

    double perimeter() const;

    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    std::string toString() override;

};


#endif //STOICHEIA_TRIANGLE_H
