#pragma once

#include "GeoObject.h"
#include "LinearObject.h"


class Triangle : public GeoObject {
    LinearObject* m_a = nullptr;
    LinearObject* m_b = nullptr;
    LinearObject* m_c = nullptr;
    double m_perimeter = 0.0;
public:
    int recomputeCount = 0;

    Triangle(LinearObject* a, LinearObject* b, LinearObject* c);

    double perimeter() const { return m_perimeter; };

    void onSourceRemoved(GeoObject *src) override { m_valid = false; };
    void recompute() override;
    std::string toString() override;

};