#ifndef STOICHEIA_RAY_H
#define STOICHEIA_RAY_H
#include "LinearObject.h"


class Ray : public LinearObject {
protected:
    bool containsT(double t) const override;
public:
    using LinearObject::LinearObject;
    Point* origin() const;
    Point* direction() const;
};


#endif //STOICHEIA_RAY_H
