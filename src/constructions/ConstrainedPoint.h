#pragma once
#include "geometry/Point.h"
#include "geometry/LinearObject.h"
#include "geometry/Circle.h"

// Punkt der auf einer Geraden/Strahl/Strecke liegt.
// m_param ist der Projektionsparameter entlang des LinearObjects.
class ConstrainedPointOnLine : public Point {
public:
    ConstrainedPointOnLine(LinearObject* line, DoublePair point);

    [[nodiscard]] LinearObject* line() const { return m_line; }
    [[nodiscard]] double param() const { return m_param; }

    void setParam(double param_t);
    void recompute() override;
    void onSourceRemoved(GeoObject* src) override;
    void replaceSource(GeoObjectPair source) override;

private:
    LinearObject* m_line;
    double m_param; // Parameter t entlang der Geraden
};

// Punkt der auf einem Kreis liegt.
// m_angle ist der Winkel in Radiant.
class ConstrainedPointOnCircle : public Point {
public:
    ConstrainedPointOnCircle(Circle* circle, DoublePair point);

    [[nodiscard]] Circle* circle() const { return m_circle; }
    [[nodiscard]] double angle() const { return m_angle; }

    void setAngle(double angle);
    void recompute() override;
    void onSourceRemoved(GeoObject* src) override;
    void replaceSource(GeoObjectPair source) override;

private:
    Circle* m_circle;
    double m_angle; // Winkel in Radiant
};