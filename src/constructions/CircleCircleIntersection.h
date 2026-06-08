#pragma once

#include "IntersectionSet.h"
#include "../geometry/Circle.h"
#include "Structs.h"

class CircleCircleIntersection : public IntersectionSet {
public:
    CircleCircleIntersection(Circle* circle1, Circle* circle2);

    // Getter
    [[nodiscard]] auto circle1() const -> Circle* { return m_circle1; }
    [[nodiscard]] auto circle2() const -> Circle* { return m_circle2; }

    // Quellen aktualisieren
    void onSourceRemoved(GeoObject* src) override;
    void replaceSource(GeoObjectPair source) override;

protected:
    void compute() override;

private:
    Circle* m_circle1 = nullptr;
    Circle* m_circle2 = nullptr;
};