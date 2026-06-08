#pragma once

#include "IntersectionSet.h"
#include "Structs.h"
#include "geometry/Circle.h"
#include "geometry/LinearObject.h"


class LineCircleIntersection : public IntersectionSet {
public:
    LineCircleIntersection(LinearObject* line, Circle* circle);

    // Getter
    [[nodiscard]] auto circle() const -> Circle* { return m_circle; }
    [[nodiscard]] auto line() const -> LinearObject* { return m_line; }

    // Quellen aktualisieren
    void onSourceRemoved(GeoObject *src) override;
    void replaceSource(GeoObjectPair source) override;

protected:
    void compute() override;

private:
    LinearObject* m_line = nullptr;
    Circle* m_circle = nullptr;
};