#pragma once

#include "Structs.h"
#include "geometryHelper.h"
#include "GeoObject.h"
#include "Point.h"


class LinearObject : public GeoObject {
public:
    int recomputeCount = 0; // for testing purpose

    LinearObject(LineParameters params);
    LinearObject(PointPairForLinearObject points);

    // Getter
    [[nodiscard]] auto dx() const -> double { return m_point2->x() - m_point1->x(); }
    [[nodiscard]] auto dy() const -> double { return m_point2->y() - m_point1->y(); }
    [[nodiscard]] auto length() const -> double { return m_length; }
    [[nodiscard]] auto point1() const -> Point* { return m_point1; }
    [[nodiscard]] auto point2() const -> Point* { return m_point2; }
    [[nodiscard]] auto parameters() const -> LineParameters { return m_lineParameters; }
    [[nodiscard]] auto points() const -> LinePoints { return { .p1=m_point1, .p2=m_point2 }; }

    // Punktprobe
    [[nodiscard]] auto contains(DoublePair point) const -> bool;
    [[nodiscard]] auto containsParameter(const double param_t) const -> bool { return containsT(param_t); }
    [[nodiscard]] auto projectParameter(DoublePair point) const -> double;

    // Objekt aktualisieren
    void onSourceRemoved(GeoObject *src) override;
    void recompute() override;
    void replaceSource(GeoObjectPair source) override;

protected:
    [[nodiscard]] virtual auto containsT(double param_t) const -> bool = 0;

    LineParameters m_lineParameters;
    Point* m_point1 = nullptr;
    Point* m_point2 = nullptr;
    double m_length = 0.0;
};
