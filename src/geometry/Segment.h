#ifndef STOICHEIA_SEGMENT_H
#define STOICHEIA_SEGMENT_H
#include "LinearObject.h"


class Segment : public LinearObject {
protected:
    bool containsT(double t) const override;
public:
    using LinearObject::LinearObject;
    Point* start() const;
    Point* end() const;
};


#endif //STOICHEIA_SEGMENT_H
