#ifndef STOICHEIA_LINE_H
#define STOICHEIA_LINE_H

#include "LinearObject.h"

class Line : public LinearObject {
protected:
    bool containsT(double) const override { return true; };
public:
    using LinearObject::LinearObject;

};


#endif //STOICHEIA_LINE_H
